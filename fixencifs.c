#include <stdio.h>
#include <stdlib.h>
#include "sys/image.h"
#include "sys/startup.h"

#define ENDIAN_RET32(x)		((((x) >> 24) & 0xff) | \
							(((x) >> 8) & 0xff00) | \
							(((x) & 0xff00) << 8) | \
							(((x) & 0xff) << 24))

#define ENDIAN_RET16(x)		((((x) >> 8) & 0xff) | \
							(((x) & 0xff) << 8))

							
static char* progname = 0;
static char* file = 0;
static int fixchksum = 0;					
static int find(FILE *fp, const unsigned char *p, int len, int count) {
	int                                     c;
	int                                     i;
	int                                     n;

	i = n = 0;
	while(i < len && (c = fgetc(fp)) != EOF) {
			n++;
			if(c == p[i]) {
					i++;
			} else {
					if(count != -1 && n >= count) {
							break;
					}
					i = c == p[0] ? 1 : 0;
			}
	}
	if(i < len) {
			return -1;
	}
	return ftell(fp) - i;
}

static void error(int level, char *p, ...) {
	va_list				ap;

	fprintf(stderr, "%s: ", progname);
	va_start(ap, p);
	vfprintf(stderr, p, ap);
	va_end(ap);

	fputc('\n', stderr);

	if(level == 0) {
		exit(EXIT_FAILURE);
	}
}

static int zero_ok (struct startup_header *shdr) {
	return(shdr->zero[0] == 0 &&
           shdr->zero[1] == 0 &&
           shdr->zero[2] == 0 );
}

static void process(FILE *fp)
{
	struct startup_header		shdr = { STARTUP_HDR_SIGNATURE };
	struct image_header			ihdr = { IMAGE_SIGNATURE };
	struct image_trailer	itlr,stlr;
	int spos;
	int ipos;
	int fsize;

	if((ipos = find(fp, (char*)&ihdr.signature, sizeof ihdr.signature, 0)) == -1) {
		rewind(fp);
		while(1){
			if((spos = find(fp, (char *)&shdr.signature, sizeof shdr.signature, -1)) == -1) {			
				shdr.signature = ENDIAN_RET32(shdr.signature);
				rewind(fp);
				if((spos = find(fp, (char *)&shdr.signature, sizeof shdr.signature, -1)) == -1) {
					error(1, "Unable to find startup header in %s", (char*)file);
					return;
				}
			}
			if(fread((char *)&shdr + sizeof shdr.signature, sizeof shdr - sizeof shdr.signature, 1, fp) != 1) {
				error(1, "Unable to read image %s", (char*)file);
				return;
			}
			if (!zero_ok(&shdr)) {
				//if (zero_check_enabled)
				continue;
				//if (verbose)
				//	printf("Warning: Non zero data in zero fields ignored\n");
				//break;
			} else {
				break;
			}
			break;
		}
	}
	fseek(fp, 0, SEEK_END);
	if(ftell(fp) != spos + shdr.stored_size)
	{
		printf("Recored file size is not correct! File size recorded (spos + shdr.stored_size) %d.  Expected %d\n", spos + shdr.stored_size, ftell(fp));
		if(fixchksum)
		{
			shdr.stored_size = ftell(fp) - spos;
			fseek(fp, spos, SEEK_SET);
			printf("Update - write %d bytes (expect write %d bytes)\n", fwrite(&shdr, sizeof shdr, 1, fp) * sizeof shdr, sizeof shdr);			
		}
	}

	fseek(fp, spos + shdr.startup_size, SEEK_SET);
	if((ipos = find(fp, ihdr.signature, sizeof ihdr.signature, -1)) == -1) {
		error(1, "Unable to find image header in %s", file);
		return;
	}
	if(fread((char *)&ihdr + sizeof ihdr.signature, sizeof ihdr - sizeof ihdr.signature, 1, fp) != 1) {
		error(1, "Unable to read image %s", file);
		return;
	}

	fseek(fp, 0 , SEEK_END);
	fsize = ftell(fp);

	if(fsize >= (spos + shdr.stored_size)){
		int             data;
		int             sum;
		int		counted=0;
		int len = shdr.stored_size;
		unsigned        max;

		fseek(fp, spos + shdr.stored_size - sizeof stlr, SEEK_SET);
		if(fread(&stlr, sizeof(stlr), 1, fp) != 1) {
			error(1, "Early end reading image trailer");
			return;
		}

		fseek(fp, spos , SEEK_SET);
		sum = 0;
		while(len > 4)
		{
			fread(&data, 4, 1, fp);
			sum += data;
			counted +=4;
			len -= 4;
		}
		//printf("Near finish sum: 0x%x\n", sum);
		sum = -1*sum;
		if(0 != (stlr.cksum - sum))
		{
			printf("\nStored checksum not correct!\n");
			printf("Expected checksum: %#lx\n", sum);
			printf("  NG: %02x %02x %02x %02x\n", stlr.cksum & 0xff, (stlr.cksum >> 8) & 0xff, (stlr.cksum >> 16) & 0xff, (stlr.cksum >> 24) & 0xff);
			printf("GOOD: %02x %02x %02x %02x\n", sum & 0xff, (sum >> 8) & 0xff, (sum >> 16) & 0xff, (sum >> 24) & 0xff);
			if(fixchksum)
			{
				fseek(fp, spos + shdr.stored_size-sizeof(stlr), SEEK_SET);
				stlr.cksum = sum;
				printf("Update - write %d bytes (expect write %d bytes)\n", fwrite(&stlr, sizeof stlr, 1, fp) * sizeof stlr, sizeof stlr);			
				fclose(fp);
			}
		}
	} else {
		printf("Image size is too small! Must at lease %d bytes.\n", spos + shdr.stored_size);
	}
	printf("Found ipos=%#lx spos=%#lx (%d)\n", ipos, spos, spos);
	printf("Compressed data offset=%#lx\n", spos + shdr.startup_size);
	printf("Image startup_size=%#lx (%ld) image_size=%#lx (%ld)\n", shdr.startup_size, shdr.startup_size, ihdr.image_size, ihdr.image_size);

}

int main(int argc, char **argv)
{
	FILE *fp;

	progname = argv[0];
	if(argc < 2){
		printf("Usage: %s <compressed ifs> [Y]\n", argv[0]);
		return 0;
	}
	file = (char*)argv[1];
	if(3 == argc && (0 == strcmp("Y", argv[2])))
	{
		fixchksum = 1;
	}
	if(0 != (fp = fopen(argv[1], 0==fixchksum?"r":"rw+")))
	{
		process(fp);
	}
	return 0;
}

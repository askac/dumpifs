#include <stdio.h>
#ifndef QNX
#include <lzo/lzo1x.h>
#include <error.h>
#else
#include <lzo1x.h>
#define error(p,q,r)	printf(r)
#endif

int main(int ac, char**av)
{
    FILE *fpr, *fpw;
    if(3 != ac)
    {
        printf("Usage %s <input> <output>\n", av[0]);
    }
    fpr = fopen(av[1], "rb");
    fpw = fopen(av[2], "wb");

	if(0 != fpr && 0 != fpw) {
		unsigned int len = 0;
		unsigned int	 out_len;
		unsigned int	 total_in_len = 0, total_out_len=0;
		int			status;
		unsigned char	work[LZO1X_999_MEM_COMPRESS];
		#define BUFFSIZE_LZO 65536L //0x10000
		unsigned char					buf[BUFFSIZE_LZO];
		unsigned char 				out_buf[BUFFSIZE_LZO+(BUFFSIZE_LZO/64 + 16 + 3)];
		
		for(;;) {
			int backCount=0;
			unsigned char packSize[2];
			printf("Read offset=%ld\n", ftell(fpr));
			len = fread(buf, 1, sizeof(buf), fpr);
			total_in_len += len;
			lzo_init();
			if(0 < len) {
retry:
				printf("buf[0-4] = %02X %02X %02X %02X, Package %d bytes \n", buf[0], buf[1], buf[2], buf[3], len);
				//status = ucl_nrv2b_99_compress(buf, len, out_buf, &out_len, NULL, 9, NULL, NULL);//= ucl_nrv2b_decompress_8(buf, len, out_buf, &out_len, NULL);
				status = lzo1x_999_compress(buf, len, out_buf, &out_len, work);
				if(status != 0) {
					error(1, 0, "compression failure.\n");
					break;
				}
				if(out_len > 0xffff) {
					printf("!!!! Packed size too big! Need retry!\n");
					printf("Read offset now=%ld\n", ftell(fpr));
					backCount++;
					fseek(fpr, -BUFFSIZE_LZO, SEEK_CUR);
					fseek(fpr, 4096 * (backCount - 1), SEEK_CUR);
					total_in_len -= len;
					printf("Read offset adjusted=%ld\n", ftell(fpr));
					len = fread(buf, 1, sizeof(buf) - 4096 * backCount, fpr);
                                        total_in_len += len;
                                        lzo_init(); 
					goto retry;
				}
				packSize[0] = (out_len >> 8) & 0xff;
				packSize[1] = out_len & 0xff;
				printf("out_len is %d (0x%02X 0x%02X)\n", out_len, packSize[0], packSize[1]);
				if( 2 == fwrite(packSize, 1, 2, fpw)) {
					total_out_len += (2 + fwrite(out_buf, 1, out_len, fpw));
					printf("Write offset=%ld\n", ftell(fpw));
				}
			}
			else
				break;
		}
		fclose(fpw);
		printf("Total read %d bytes from input file: %s\n", total_in_len, av[1]);
		printf("Total write %d bytes to output file: %s\n", total_out_len , av[2]);
		printf("Done.\n");
	}
}

#include <stdio.h>
#include <ucl/ucl.h>

#ifndef QNX
#include <error.h>
#else
#define error(p,q,r)	printf(r)
#endif

#define BUFFSIZE_UCL 0x10000 //imagefs reserve 2 bytes for length of compressed block

int main(int ac, char**av)
{
    FILE *fpr, *fpw;
    if(3 != ac)
    {
        printf("Usage %s <input> <output>\n", av[0]);
    }
	if (ucl_init() != UCL_E_OK)
    {
        printf("ucl_init() failed !!!\n");
        return 4;
    }
    fpr = fopen(av[1], "rb");
    fpw = fopen(av[2], "wb");

	if(0 != fpr && 0 != fpw) {
		unsigned int len = 0;
		unsigned int unprocessed_len = 0;
		ucl_uint	 out_len;
		ucl_uint	 total_in_len = 0, total_out_len=0;
		int			status;
		unsigned char in_buf[BUFFSIZE_UCL];
		unsigned char out_buf[BUFFSIZE_UCL + (BUFFSIZE_UCL / 8 + 256)];
		
		for(;;) {
			unsigned char packSize[2];
			len = unprocessed_len = fread(in_buf, 1, sizeof(in_buf), fpr);
			unsigned char* buf = in_buf;
			total_in_len += len;
			if(0 < len) {
				while(unprocessed_len != 0) {
					printf("buf[0-4] = %02X %02X %02X %02X, Package %d bytes \n", buf[0], buf[1], buf[2], buf[3], len);
					status = ucl_nrv2b_99_compress(buf, len, out_buf, &out_len, NULL, 9, NULL, NULL);//= ucl_nrv2b_decompress_8(buf, len, out_buf, &out_len, NULL);
					if(status != 0) {
						error(1, 0, "compression failure.\n");
						return -1;
					}
					if(out_len >= BUFFSIZE_UCL) {
						//Didn't compress, try smaller block value so we can use
						//2 byte lengths in file
						len -= 0x1000;
						continue;
					}

					packSize[0] = (out_len >> 8) & 0xff;
					packSize[1] = out_len & 0xff;
					printf("out_len is %d (0x%02X 0x%02X)\n", out_len, packSize[0], packSize[1]);
					if( 2 == fwrite(packSize, 1, 2, fpw)) {
						total_out_len += (2 + fwrite(out_buf, 1, out_len, fpw));
						printf("Write offset=%ld\n", ftell(fpw));
					}
					buf += len;
					unprocessed_len -= len;
					len = unprocessed_len;
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

    return 0;
}

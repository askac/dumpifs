#include <stdio.h>
#include <lzo/lzo1z.h>

/*void lzoDecompress()
{
	unsigned	len, til=0, tol=0;
	lzo_uint	out_len;
	int			status;

	if(lzo_init() != LZO_E_OK) {
		error(1,"decompression init failure");
		return;
	}
	printf("LZO Decompress @0x%0lx\n", ftell(fp));
	for(;;) {
		len = getc(fp) << 8;
		len += getc(fp);
		if(len == 0) break;
		fread(buf, len, 1, fp);
		printf("lzo1x_decompress(buf, %d...)\n");
		status = lzo1x_decompress(buf, len, out_buf, &out_len, NULL);
		if(status != LZO_E_OK) {
			error(1, "decompression failure");
			return;
		}
		til+=len;tol+=out_len;
		printf("LZO Decompress rd=%d, wr=%d\n", len, out_len);
		fwrite(out_buf, out_len, 1, fp2);
	}
	printf("Decompressed %d bytes-> %d bytes\n", til, tol);
}
*/
/*
    v2 = 0;
    nextImgPtr = (int)inputPtr;
    v7 = 0;
    v8 = 2048;                                  // Start offset of real content
    do
    {
      origImgStart = nextImgPtr;
      blockSize = totalSize & 0x1FF;
      if ( blockSize > 0 )
        blockSize = 512 - blockSize;
      // v10 - buffer memory
      // v10[0] = offset
      // v10[1] = ??
      // v10[2] = ??
      // v10[3] = ??
      // v10[4] = type
      // v10[5] = ??
      v10 = malloc(24u);
      v11 = blockSize + totalSize;
      if ( v2 < v11 )
        v2 = v11;
      v10[2] = v7++;
      *v10 = v8;
      v8 += v11;
      v10[1] = totalSize;
      v10[4] = compressType;
      v10[5] = contentOffset;
      nextImgPtr = origImgStart;
      if ( v7 == 254 )
        break;
      totalSize = *(_DWORD *)(origImgStart + 12);
      nextImgPtr = origImgStart + 4;
    }
    while ( totalSize );
*/

typedef struct
{
	unsigned int offset;
	unsigned int size;
} BLOCKINFO;

static BLOCKINFO *blockInfo[40];
static BLOCKINFO **currBlockInfo = blockInfo;

static addBlockInfo(BLOCKINFO *blk)
{
	*currBlockInfo = blk;
	currBlockInfo++;
}

int main(int argc, char **argv)
{
	FILE *fp, *fpOut;
	char buffer[0x200];
	unsigned int total=0;

	if(3 != argc) return 0;   

	fp = fopen(argv[1], "r");
	fpOut = fopen(argv[2], "w+");
	if(fp){
		int blockSize;
		int totalSize;
		int v2 = 0, maxBlkSize;
		int inputPtr = (int) fp;
		int nextImgPtr = (int)buffer;
		int v7 = 0;
		int v8 = 2048;                        // Start offset of real content
		unsigned int v10[6];
		int *origImgStart = (int*)buffer;
		int contentOffset;
		int *pRead;
		BLOCKINFO **readBlk;

		fread(buffer, 1, sizeof(buffer), fp);
		pRead = buffer;
		printf("Test... *((int*)pRead + 1) = %x, *((int*)pRead + 2) = %x\n", *((int*)pRead + 1), *((int*)pRead + 2));
		printf("Type=%c%c%c%c\n", (char)buffer[0], (char)buffer[1], (char)buffer[2], (char)buffer[3]);
		printf("Buffer @0x%X\n", (unsigned int)buffer);
		contentOffset = *((unsigned int *)buffer + 1);
		totalSize = *((unsigned int *)buffer + 2);
		printf("contentOffset=0x%x @ 0x%X  %d blocks. totalSize=%d(%x) @ 0x%X\n", contentOffset, (unsigned int *)buffer + 1, totalSize, totalSize, (unsigned int *)buffer + 2);
		for(pRead = (int*)&buffer[8];pRead < (buffer+sizeof(buffer));pRead++)
		{
			int gap, unitSize;
			BLOCKINFO *blk = (BLOCKINFO*) malloc(sizeof(BLOCKINFO));
			if(0 == *pRead) break;
			v2++;
			unitSize = ((0x200 - (*pRead & 0x1ff)) + *pRead);
			blk->offset = v7 + 0x800;
			blk->size = *pRead;
			printf("%d, unitSize=0x%x, offset=0x%x\n", *pRead, unitSize, v7 + 2048);
			addBlockInfo(blk);
			v7 += unitSize;
		}
		printf("Block count=%d Total Size=%d + 2048=%d (%x)\n", v2, v7, v7+2048, v7+2048);
		v7=0;maxBlkSize=0;
		if(lzo_init() != LZO_E_OK) {
			error(1, "decompression init failure");
			return 0;
		}

		for(readBlk=blockInfo;readBlk!=currBlockInfo;readBlk++)
		{
			char *extractBuffer = (char*)malloc((*readBlk)->size * 10);
			char *buf = (char*)malloc((*readBlk)->size);
			unsigned out_len = 0;
			printf("Block#%02d bs=0x%x, offset=0x%x\n", v7++, (*readBlk)->size, (*readBlk)->offset);
			fseek(fp, (*readBlk)->offset, SEEK_SET);
			if(maxBlkSize < (*readBlk)->size)
				maxBlkSize = (*readBlk)->size;

			if(0<fread(buf, 1, (*readBlk)->size, fp))
			{
				printf("lzo1z_decompress(buf(%x), (*readBlk)->size=%d, extractBuffer(%x), ", buf, (*readBlk)->size, extractBuffer);
				lzo1z_decompress(buf, (*readBlk)->size, extractBuffer, &out_len, NULL);
				//lzo1z_decompress(&buf[4], buf[0] | (int)buf[1]<<8, extractBuffer, &out_len, NULL);
				printf("out_len=%d, NULL);\n", out_len);
				//fwrite(buf, 1, (*readBlk)->size, fpOut);
				fwrite(extractBuffer, 1, out_len, fpOut);
			}
			//free(*readBlk);
			//free(extractBuffer);
			//free(buf);
			printf("Next\n");
		}
		fclose(fp);
		fclose(fpOut);
		printf("Max block size is %d (0x%x)\n", maxBlkSize, maxBlkSize);
/*		do
		{
			int c;
			origImgStart = (int*)nextImgPtr;
			printf("a origImgStart @%x\n", origImgStart);
			blockSize = totalSize & 0x1FF;
			if ( blockSize > 0 )
				blockSize = 512 - blockSize;
			//printf("blocksize=%x, totalSize=%x\n\n\n", blockSize, totalSize);
			// v10 - buffer memory
			// v10[0] = offset
			// v10[1] = ??
			// v10[2] = ??
			// v10[3] = ??
			// v10[4] = type
			// v10[5] = ??
			//v10 = (int*)malloc(24u);

			printf("b origImgStart @%x\n", origImgStart);

			v11 = blockSize + totalSize;
			if ( v2 < v11 )
				v2 = v11;
			v10[2] = v7++;
			*v10 = v8;
			v8 += v11;
			v10[1] = totalSize;
			v10[4] = 4;//compressType;
			v10[5] = contentOffset;
			//sub_1033E0((int)v10);
			nextImgPtr = origImgStart;
			if ( v7 == 254 )
				break;
			printf("c origImgStart @%x\n", *(int *)(origImgStart + 12));
			totalSize = *(int *)(origImgStart + 12);
			nextImgPtr = origImgStart + 4;

			for(c=0;c<6;c++)
			{
				printf("[%d] %02x,%02x,%02x,%02x  ", c, ((unsigned char*)&v10[c])[0], ((unsigned char*)&v10[c])[1], ((unsigned char*)&v10[c])[2], ((unsigned char*)&v10[c])[3]);
				printf("size 0x%x\n", totalSize);
			}
		}
		while ( totalSize );*/
	}
}


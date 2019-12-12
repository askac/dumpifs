#include <stdio.h>
#include <stdlib.h>
#include <lz4.h>
#include <lz4frame.h>

typedef struct
{
	unsigned int offset;
	unsigned int size;
} BLOCKINFO;

static BLOCKINFO *blockInfo[80];
static BLOCKINFO **currBlockInfo = blockInfo;

static void addBlockInfo(BLOCKINFO *blk)
{
	*currBlockInfo = blk;
	currBlockInfo++;
}


int main(int argc, char **argv)
{
	FILE *fp, *fpOut;
	char buffer[0x200];
	unsigned int total=0;
	LZ4_streamDecode_t lz4StreamDecode_body;
	LZ4_streamDecode_t* lz4StreamDecode = &lz4StreamDecode_body;
	LZ4_setStreamDecode(lz4StreamDecode, NULL, 0);

	if(3 > argc)
	{
		printf("Usage %s <src lz4> <dest> [offset]\n", argv[0]);
		return 0;   
	}

	fp = fopen(argv[1], "r");
	fpOut = fopen(argv[2], "w+");
	if(fp){
		int blockSize;
		unsigned int totalSize;
		int v2 = 0, maxBlkSize;
		int inputPtr = (int) fp;
		int nextImgPtr = (int)buffer;
		int v7 = 0;
		int StartOffset = 0x2000;//0x800;                        // Start offset of real content
		unsigned int v10[6];
		int *origImgStart = (int*)buffer;
		int contentOffset;
		int *pRead;
		BLOCKINFO **readBlk;
		
		if(4 == argc)
			StartOffset=atoi(argv[3]);

		fread(buffer, 1, sizeof(buffer), fp);
		pRead = buffer;
		printf("Test... *((int*)pRead + 1) = %x, *((int*)pRead + 2) = %x\n", *((int*)pRead + 1), *((int*)pRead + 2));
		printf("Type=%c%c%c%c\n", (char)buffer[0], (char)buffer[1], (char)buffer[2], (char)buffer[3]);
		printf("Buffer @0x%X\n", (unsigned int)buffer);
		contentOffset = *((unsigned int *)buffer + 1);
		totalSize = *((unsigned int *)buffer + 2);
		printf("contentOffset=0x%x @ 0x%X  %d blocks. totalSize=%d(%x) @ 0x%X\n", contentOffset, (unsigned int *)buffer + 1, (unsigned int) totalSize, (unsigned int) totalSize, (unsigned int *)buffer + 2);
		for(pRead = (int*)&buffer[8];pRead < (buffer+sizeof(buffer));pRead++)
		{
			int gap, unitSize;
			BLOCKINFO *blk = (BLOCKINFO*) malloc(sizeof(BLOCKINFO));
			if(0 == *pRead) break;
			v2++;
			unitSize = ((0x200 - (*pRead & 0x1ff)) + *pRead);
			blk->offset = v7 + StartOffset;
			blk->size = *pRead;
			//printf("%d, unitSize=0x%x, offset=0x%x\n", *pRead, unitSize, v7 + 2048);
			addBlockInfo(blk);
			v7 += unitSize;
		}
		printf("Block count=%d Total Size=%d + 2048=%d (%x)\n", v2, v7, v7+2048, v7+2048);
		v7=0;maxBlkSize=0;
		//if(lzo_init() != LZO_E_OK) {
		//	error(1, "decompression init failure");
		//	return 0;
		//}

		for(readBlk=blockInfo;readBlk!=currBlockInfo;readBlk++)
		{
			#define BUFFER_RATIO	80
			char *extractBuffer = (char*)malloc((*readBlk)->size * BUFFER_RATIO);
			char *buf = (char*) malloc((*readBlk)->size);
			unsigned out_len = 0;
			printf("Blk#%02d bs=0x%x, offset=0x%x\n", v7++, (*readBlk)->size, (*readBlk)->offset);
			fseek(fp, (*readBlk)->offset, SEEK_SET);
			if(maxBlkSize < (*readBlk)->size)
				maxBlkSize = (*readBlk)->size;

			if(0<fread(buf, 1, (*readBlk)->size, fp))
			{
				//char* const decPtr = decBuf[decBufIndex];
				int decBytes = LZ4_decompress_safe(
				buf, extractBuffer, (*readBlk)->size, (*readBlk)->size * BUFFER_RATIO);
				
				printf("LZ4_decompress_safe(buf, exbuf, readSize=%d, max) out=%d\n", (*readBlk)->size, decBytes);
				/*if(decBytes < 0)
				{
					decBytes+=(*readBlk)->size;
					//memcpy(extractBuffer, buf, (*readBlk)->size);
				}*/
				if(decBytes <= 0) {
					printf("End at offset 0x%x\n",  (unsigned int)(*readBlk)->offset);
					break;
				}
				out_len = decBytes;
				//printf("lzo1z_decompress(buf(%x), (*readBlk)->size=%d, extractBuffer(%x), ", buf, (*readBlk)->size, extractBuffer);
				//out_len = LZ4_decompress_safe(buf, extractBuffer, (*readBlk)->size, 0xffff);
				//printf("out_len=%d, NULL);\n", out_len);
				fwrite(extractBuffer, 1, out_len, fpOut);
			}
			free(*readBlk);
			free(extractBuffer);
			//free(buf);
			//printf("Next\n");
		}
		fclose(fp);
		fclose(fpOut);
		printf("Max block size is %d (0x%x)\n", maxBlkSize, maxBlkSize);
	}
}


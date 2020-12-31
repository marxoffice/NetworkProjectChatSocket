#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Constants.h"

#ifndef TOTALTEST
Byte SrcAddr[6] = {0x01,0x02,0x03,0x04,0x05,0x06};
Byte DestAddr[6] = {0x11,0x12,0x13,0x14,0x15,0x16};
Byte DataType[2] = {0x88,0x66};
Byte* Payload = "Hello World, This is a test of DIX Frame in Sending state.";
#endif

// this struct show the frame struct
// in linklayer frame is bytes
// typedef struct
// {
// 	Byte DestAddr[6];
// 	Byte SrcAddr[6];
// 	Byte DataType[2];
// 	Byte* Payload;
// 	Byte FCS[4];
// } frame_t;

// CRC32 algorithm
static unsigned int crc32tab[256] = {
 0x00000000L, 0x77073096L, 0xee0e612cL, 0x990951baL,
 0x076dc419L, 0x706af48fL, 0xe963a535L, 0x9e6495a3L,
 0x0edb8832L, 0x79dcb8a4L, 0xe0d5e91eL, 0x97d2d988L,
 0x09b64c2bL, 0x7eb17cbdL, 0xe7b82d07L, 0x90bf1d91L,
 0x1db71064L, 0x6ab020f2L, 0xf3b97148L, 0x84be41deL,
 0x1adad47dL, 0x6ddde4ebL, 0xf4d4b551L, 0x83d385c7L,
 0x136c9856L, 0x646ba8c0L, 0xfd62f97aL, 0x8a65c9ecL,
 0x14015c4fL, 0x63066cd9L, 0xfa0f3d63L, 0x8d080df5L,
 0x3b6e20c8L, 0x4c69105eL, 0xd56041e4L, 0xa2677172L,
 0x3c03e4d1L, 0x4b04d447L, 0xd20d85fdL, 0xa50ab56bL,
 0x35b5a8faL, 0x42b2986cL, 0xdbbbc9d6L, 0xacbcf940L,
 0x32d86ce3L, 0x45df5c75L, 0xdcd60dcfL, 0xabd13d59L,
 0x26d930acL, 0x51de003aL, 0xc8d75180L, 0xbfd06116L,
 0x21b4f4b5L, 0x56b3c423L, 0xcfba9599L, 0xb8bda50fL,
 0x2802b89eL, 0x5f058808L, 0xc60cd9b2L, 0xb10be924L,
 0x2f6f7c87L, 0x58684c11L, 0xc1611dabL, 0xb6662d3dL,
 0x76dc4190L, 0x01db7106L, 0x98d220bcL, 0xefd5102aL,
 0x71b18589L, 0x06b6b51fL, 0x9fbfe4a5L, 0xe8b8d433L,
 0x7807c9a2L, 0x0f00f934L, 0x9609a88eL, 0xe10e9818L,
 0x7f6a0dbbL, 0x086d3d2dL, 0x91646c97L, 0xe6635c01L,
 0x6b6b51f4L, 0x1c6c6162L, 0x856530d8L, 0xf262004eL,
 0x6c0695edL, 0x1b01a57bL, 0x8208f4c1L, 0xf50fc457L,
 0x65b0d9c6L, 0x12b7e950L, 0x8bbeb8eaL, 0xfcb9887cL,
 0x62dd1ddfL, 0x15da2d49L, 0x8cd37cf3L, 0xfbd44c65L,
 0x4db26158L, 0x3ab551ceL, 0xa3bc0074L, 0xd4bb30e2L,
 0x4adfa541L, 0x3dd895d7L, 0xa4d1c46dL, 0xd3d6f4fbL,
 0x4369e96aL, 0x346ed9fcL, 0xad678846L, 0xda60b8d0L,
 0x44042d73L, 0x33031de5L, 0xaa0a4c5fL, 0xdd0d7cc9L,
 0x5005713cL, 0x270241aaL, 0xbe0b1010L, 0xc90c2086L,
 0x5768b525L, 0x206f85b3L, 0xb966d409L, 0xce61e49fL,
 0x5edef90eL, 0x29d9c998L, 0xb0d09822L, 0xc7d7a8b4L,
 0x59b33d17L, 0x2eb40d81L, 0xb7bd5c3bL, 0xc0ba6cadL,
 0xedb88320L, 0x9abfb3b6L, 0x03b6e20cL, 0x74b1d29aL,
 0xead54739L, 0x9dd277afL, 0x04db2615L, 0x73dc1683L,
 0xe3630b12L, 0x94643b84L, 0x0d6d6a3eL, 0x7a6a5aa8L,
 0xe40ecf0bL, 0x9309ff9dL, 0x0a00ae27L, 0x7d079eb1L,
 0xf00f9344L, 0x8708a3d2L, 0x1e01f268L, 0x6906c2feL,
 0xf762575dL, 0x806567cbL, 0x196c3671L, 0x6e6b06e7L,
 0xfed41b76L, 0x89d32be0L, 0x10da7a5aL, 0x67dd4accL,
 0xf9b9df6fL, 0x8ebeeff9L, 0x17b7be43L, 0x60b08ed5L,
 0xd6d6a3e8L, 0xa1d1937eL, 0x38d8c2c4L, 0x4fdff252L,
 0xd1bb67f1L, 0xa6bc5767L, 0x3fb506ddL, 0x48b2364bL,
 0xd80d2bdaL, 0xaf0a1b4cL, 0x36034af6L, 0x41047a60L,
 0xdf60efc3L, 0xa867df55L, 0x316e8eefL, 0x4669be79L,
 0xcb61b38cL, 0xbc66831aL, 0x256fd2a0L, 0x5268e236L,
 0xcc0c7795L, 0xbb0b4703L, 0x220216b9L, 0x5505262fL,
 0xc5ba3bbeL, 0xb2bd0b28L, 0x2bb45a92L, 0x5cb36a04L,
 0xc2d7ffa7L, 0xb5d0cf31L, 0x2cd99e8bL, 0x5bdeae1dL,
 0x9b64c2b0L, 0xec63f226L, 0x756aa39cL, 0x026d930aL,
 0x9c0906a9L, 0xeb0e363fL, 0x72076785L, 0x05005713L,
 0x95bf4a82L, 0xe2b87a14L, 0x7bb12baeL, 0x0cb61b38L,
 0x92d28e9bL, 0xe5d5be0dL, 0x7cdcefb7L, 0x0bdbdf21L,
 0x86d3d2d4L, 0xf1d4e242L, 0x68ddb3f8L, 0x1fda836eL,
 0x81be16cdL, 0xf6b9265bL, 0x6fb077e1L, 0x18b74777L,
 0x88085ae6L, 0xff0f6a70L, 0x66063bcaL, 0x11010b5cL,
 0x8f659effL, 0xf862ae69L, 0x616bffd3L, 0x166ccf45L,
 0xa00ae278L, 0xd70dd2eeL, 0x4e048354L, 0x3903b3c2L,
 0xa7672661L, 0xd06016f7L, 0x4969474dL, 0x3e6e77dbL,
 0xaed16a4aL, 0xd9d65adcL, 0x40df0b66L, 0x37d83bf0L,
 0xa9bcae53L, 0xdebb9ec5L, 0x47b2cf7fL, 0x30b5ffe9L,
 0xbdbdf21cL, 0xcabac28aL, 0x53b39330L, 0x24b4a3a6L,
 0xbad03605L, 0xcdd70693L, 0x54de5729L, 0x23d967bfL,
 0xb3667a2eL, 0xc4614ab8L, 0x5d681b02L, 0x2a6f2b94L,
 0xb40bbe37L, 0xc30c8ea1L, 0x5a05df1bL, 0x2d02ef8dL
};

static void _create_CRCTable() {
	// get crc table
	unsigned int crc32, idx = 0, bit = 0;
	for(idx = 0; idx < 256; idx++) {
		crc32 = idx;
		for(bit = 0; bit < 8; bit++) {
			if(crc32&1)
				crc32 = (crc32 >> 1)^(0xEDB88320);
			else crc32 = crc32 >> 1;
		}
		crc32tab[idx] = crc32;
	}  
}
static unsigned int get_crc32(const unsigned char *buf, unsigned int size) {
	// calcuate crc32 value
	unsigned int i, crc;
	crc = 0xFFFFFFFF;
	for (i = 0; i < size; i++)
		crc = crc32tab[(crc ^ buf[i]) & 0xff] ^ (crc >> 8);
	return crc^0xFFFFFFFF;
}

int build_frame(Byte* frame, Byte* DestAddr, Byte* SrcAddr, Byte* DataType, Byte* Payload,int PayloadLength){
	// assert 46Byte <= len(Payload) <= 1500 Byte
	// int PayloadLength = strlen(Payload);
	memcpy(frame,DestAddr,6);
	memcpy(frame+6,SrcAddr,6);
	// memcpy(frame+12,DataType,2);
	Hex2Byte(frame+12, (Hex)PayloadLength);
	memcpy(frame+14,Payload,PayloadLength);
	
	// Warning: C language uses little-endian mode by default,
	// but big-endian mode is used by default on some microcontrollers
	// and some CPU-architecture(RISC MIPS)
	// 0x12345678 ==> big-endian{0x12,0x34,0x56,0x78}
	//                little-endian{0x78,0x56,0x34,0x12}
	// Network uses Big-endian mode :)
	unsigned int FCS = get_crc32(frame,PayloadLength+14);
	translate_uint32_Byte(frame+14+PayloadLength,(Byte*)&FCS);
	
	return 18+PayloadLength; // return the lenth of frame
}

int get_length_frame(Byte* frame){
	Hex len;
	Byte2Hex(&len,frame+12,2);
	return (int)len;
}
int get_dest(Byte* DestAddr, Byte* frame){
	// get DestAddr
	memcpy(DestAddr,frame,6);
	return 0;
}

int get_Src(Byte* SrcAddr, Byte* frame){
	// get_SrcAddr
	memcpy(SrcAddr,frame+6,6);
	return 0;
}

int parse_frame(Byte* Payload, Byte* frame, Byte localAddr[6]){
	printf("\n==========parse frame start===========\n");
	// check frame and get Payload
	// if the frame is mistake return 0
	// else return the length of Payload
	// int FrameLength = strlen(frame);
	
	int PayloadLength = get_length_frame(frame);
	int FrameLength = PayloadLength + 18;
	memcpy(Payload,frame+14,PayloadLength);
	
	if(PayloadLength > 1500){
		printf("Payload length mistake\n");
		goto Failure;
	}

	unsigned int FCS,FrameFCS;
	translate_uint32_Byte((Byte*)&FrameFCS,frame+FrameLength-4);
	FCS = get_crc32(frame,FrameLength-4);
	if(FrameFCS != FCS){
		printf("frame FCS mistake\n");
		goto Failure;
	} else {
		printf("FCS valid:%d\n",FCS);
	}
	
	Byte DestAddr[6] = {0x0};
	get_dest(DestAddr, frame);
	int flag = CompareBytes(DestAddr,localAddr,6);
	if(flag == 0){
		printf("No, the local frame addr can't fit\n");
		goto Failure;
	} else {
		printf("MAC Addr valid\n");
	}

	printf("==========parse frame success: PayloadLength:%d===========\n",PayloadLength);

	return PayloadLength;

Failure:
	Payload[0] = '\0';
	return 0;
}

// int frame_stream(Byte* frames[], Byte* DestAddr, Byte* SrcAddr, Byte* DataType, Byte* Payload){
// 	// build FrameList from ByteStream(length > 1500)
// 	int PayloadLength = strlen(Payload);
// 	int FrameCnt = PayloadLength / 1500;
// 	int i,j;
// 	Byte PayloadTmp[1501],*FrameP;
	
// 	for(i=0;i<FrameCnt;i++){
// 		// max length of frame:1500+18
// 		memcpy(&PayloadTmp[0], Payload+i*1500, 1500);
// 		FrameP = (Byte*)malloc(1518*sizeof(Byte));
// 		build_frame(FrameP,DestAddr,SrcAddr,DataType,PayloadTmp,strlen(Payload));
// 		frames[i] = FrameP;
// 	}
// 	int LastPayloadLength = PayloadLength - FrameCnt*1500;
// 	if(LastPayloadLength != 0){
// 		// payload length mod 1500 != 0, add a frame(length<1500)
// 		memcpy(&PayloadTmp[0], Payload+i*1500, LastPayloadLength);
// 		while(LastPayloadLength < 46){
// 			// length<46, then set 0x7E padding
// 			PayloadTmp[PayloadLength++] = 0x7E;
// 		}
// 		PayloadTmp[PayloadLength] = '\0';
// 		FrameP = (Byte*)malloc(64*sizeof(Byte));
// 		build_frame(FrameP,DestAddr,SrcAddr,DataType,PayloadTmp,strlen(Payload));
// 		frames[++FrameCnt] = FrameP; 
// 	}
// 	return FrameCnt;
// }


#ifndef TOTALTEST
int main(int argc, char* argv[]){
	printf("\n\n==============build and send frame==============\n\n");
	// first step build and send(save file) frame
	_create_CRCTable(); // create CRCtable
	print_format(Payload,"%c",strlen(Payload),"Payload in char:"); // print payload
	printf("Payload Length:%d\n",strlen(Payload));
	Byte* frame = (Byte*)malloc(1516* sizeof(Byte));
	
	int len = build_frame(frame,DestAddr,SrcAddr,DataType,Payload,strlen(Payload));
	// build a frame from payload and get the length of frame
	printf("frame Length: %d\n",len);
	print_format(frame,"%02x ",len,"frame in hex:");
	unsigned int CRC = get_crc32(frame,len-4); // re-calculate CRC and check if it is equal to frame-end
	printf("Calculated CRC value:%x,  please check the last 4 items in frame\n",CRC);
	
	// FILE *fp = NULL; // save file
	// fp = fopen("test.frame", "wb");
	// fprintf(fp, frame);
	// free(frame);
	// fclose(fp);
	// printf("Save this frame in file test.frame successful\n");
	WriteFileBytes("test.frame",frame,len);

	printf("\n\n==============receive and parse frame==============\n\n");
	// step 2 receive(read file) and parse frame
	Byte buff[1516];
	// FILE *FrameData = NULL; // read file
	// FrameData = fopen("test.frame","r");
	// fread(buff, 1516, 1, FrameData);
	// fclose(FrameData);
	ReadFileBytes("test.frame",buff,len);
	int FrameLen = strlen(buff);
	printf("\nRead a frame from file test.frame, the length: %d\n",FrameLen);
	print_format(buff,"%02x ",FrameLen,"frame in hex:"); // print frame in hex mode

	Byte* Data = (Byte*)malloc(1500* sizeof(Byte));
	int flag = parse_frame(Data,buff,DestAddr); // parse frame and get the length of payload
	if(flag){
		printf("Payload length: %d\n",flag);
		print_format(Data,"%c",flag,"Payload print in char:"); // print payload in char mode
	} else printf("frame has some mistake\n"); // flag == 0; frame has mistake
	free(Data);
	return 0;
}
#endif

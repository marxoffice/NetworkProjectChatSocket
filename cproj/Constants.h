#ifndef MARX_CONSTANT_AND_FUNC    // parograma once
#define MARX_CONSTANT_AND_FUNC



typedef unsigned char Byte; // 1 Byte = 8 bit
typedef unsigned short Hex; // 1 Hex = 16bit
#define MAX_MSG 1024

Byte Tmp;
Hex hexTmp;

unsigned int MTU = 1400;
const Byte defaultQos = 0xaa;
const Hex defaultID = 0x07;
const Hex defaultFrag = 0b0100000000000000; // DF=1 MF=0 Offset=0
const Hex defaultMF =   0b0010000000000000; // DF=0 MF=1 Offset=0
const Byte defaultTTL = 0x2f; // int ttl = 47
const Byte zeroByte = 0x00;
const Hex zeroHex = 0x0000;

const Byte TCPprotocol = 0x06;
const Byte UDPprotocol = 0x11;
// read RFC 1700 for the number of protocol. TCP=6 UDP=17

// Masks for bit-field
const Byte IHLMask = 0x0f;
const Byte VersionMask = 0xf0;
const Hex DFMask = 0x4000;
const Hex MFMask = 0x2000;
const Hex OffsetMask = 0x1fff;

int print_format(Byte* src, char* format,int size, char* welcome){
	// print src one by one in format
	if(welcome)
		printf(welcome);
	int i;
	for(i=0;i<size;i++){
		printf(format,src[i]);
	}
	printf("\n");
	return 0;
}

int CompareBytes(Byte* a,Byte* b,int len){
	while(len--){
		if (*a != *b)
			return 0;
	}
	return 1;
}

void ReadFileBytes(char* FileName, Byte* Data, int Length){
	FILE *fp = NULL;                            // file pointer
	fp = fopen(FileName,"rb");
	fread(Data, Length, 1, fp);
	fclose(fp);
	printf("\nRead File: %s load data\n",FileName);
}

int WriteFileBytes(char* FileName, Byte* Data, int Length){
	FILE *fp = NULL;                            // file pointer
	fp = fopen(FileName,"wb");
	fwrite(Data, Length, 1, fp);
	fclose(fp);
	printf("\nWrite File: %s save data\n",FileName);
}


/**
 * in fact this func also can use in udp mode
 * checksum compute func
 * int len: Byte length of byte[]
 * 
 * when in generate checksum mode, let checksum=0
 * func return checksum in hex, then you can add it to buffer
 * 
 * when in check checksum mode, do nothing
 * func return 0 for right, other for false
 */
Hex checksum(Byte* buffer, int len){
    unsigned long cksum = 0;
    while(len>1){
		Hex buf1 = *buffer++;
		buf1 = buf1<<8;
		buf1 = buf1 + (*buffer++);
		cksum += buf1;
        len -= sizeof(Hex);
    }
    if(len){
        cksum += *(Byte*)buffer;
    }
    cksum = (cksum>>16) + (cksum & 0xffff); 
    cksum += (cksum>>16);
    return (Hex)(~cksum);
}


// deal with byte order
// c use small-endian while network use big-endian
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
void swap_helper(Byte* addr1,Byte* addr2){
	Tmp = *addr1;
	*addr1 = *addr2;
	*addr2 = Tmp;
}
void translate_uint32_Byte(Byte* dest,Byte* src){
	// translate between uint32 and Byte
	memcpy(dest,src,4);
	swap_helper(dest,dest+3);
	swap_helper(dest+1,dest+2);
}

void Hex2Byte(Byte* b, Hex h){
	memcpy(b,&h,2);
	swap_helper(b,b+1);
}

void Byte2Hex(Hex* h, Byte* b,int len){
	// assert len > 0;
	int itr = len;
	while(itr){
		Hex* currDest = h + (len-itr) / 2;
		Byte* currSrc = b +(len-itr);
		memcpy(currDest,currSrc,2);
		swap_helper((Byte*)currDest,(Byte*)currDest+1);
		itr -= sizeof(Hex); // itr -= 2
		// printf("%x\n",*currDest);
	}
}



#else
void translate_uint32_Byte(Byte* dest,Byte* src){
	// translate between uint32 and Byte
	memcpy(dest,src,4);
}

void Hex2Byte(Byte* b, Hex h){
	memcpy(&b,&h,2);
}

void Byte2Hex(Hex* h, Byte* b, int len){
	memcpy(h,b,len);
}


#endif

#endif

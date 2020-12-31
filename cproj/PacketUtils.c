#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Constants.h"

#ifndef TOTALTEST
Byte localSrcIPAddr[4] = {0x01,0x02,0x03,0x04};
Byte localDestIPAddr[4] = {0x01,0x02,0x03,0x04};
Byte* Payload = "Hello World, This is a test of IPv4 packet in Sending state.";
Byte BigPayload[3000];
#endif

/**
 * build a ipv4 packet
 * 
 */
int build_packet(Byte* packet, Byte Qos, Hex ID, Hex frag, Byte TTL,
	 Byte protocol, Byte SrcIPAddr[4], Byte DestIPAddr[4],Byte* payload, int PayloadLength){
	// assert len(Payload) <= MTU
	// int PayloadLength = strlen(payload);
	Byte Version = 0x40;           // IPv4
	Byte HeaderLength = 0x05; // default header length
	Hex TotalLength = HeaderLength*4 + PayloadLength; // total length
	Hex cksum = 0;
	
	Byte temp = Version | HeaderLength;
	// Step1 build a packet header
	memcpy(packet,&temp,1);
	memcpy(packet+1,&Qos,1);
	Hex2Byte(packet+2,TotalLength);
	Hex2Byte(packet+4,ID);
	Hex2Byte(packet+6,frag);
	memcpy(packet+8,&TTL,1);
	memcpy(packet+9,&protocol,1);
	Hex2Byte(packet+10,cksum);
	memcpy(packet+12,&SrcIPAddr[0],4);
	memcpy(packet+16,&DestIPAddr[0],4);

	// Step2 compute header checksum and feed them in header
	cksum = checksum(packet,(4*(int)HeaderLength));
	printf("CHECKSUM COMPUTED:%x\n",cksum);
	Hex2Byte(packet+10,cksum);

	// Step3 link payload
	memcpy(packet+20,payload,PayloadLength);

	return (int)TotalLength;
}



/**
 * get packet header, check sksum and return header length
 * if return 0 there must be some mistake, such as cksum invalid
 */
int get_header(Byte* header, Byte* packet){
	Byte FirstByte;
	memcpy(&FirstByte,packet,1); // get first byte which contains Version|IHL
	int HeaderLength = (FirstByte & IHLMask)<<2; // IHL is x 32bit, so that we plus 4
	memcpy(header,packet,HeaderLength);        // get header
	Hex cksum = checksum(header,HeaderLength);
	printf("Hex cksum:%x\n",cksum);
	if(cksum){
		// checksum is invalid
		HeaderLength = 0;
		header[0] = '\0';
	}
	return HeaderLength;
}

/**
 * get src ip and dest ip from ip header
 * make sure input header is valid
 * 
 */
int get_addr_from_header(Byte DestIPAddr[4], Byte SrcIPAddr[4], Byte* header){
	memcpy(&SrcIPAddr[0],header+12,4);
	memcpy(&DestIPAddr[0],header+16,4);
	return 0;
}

int get_Qos_from_header(Byte* Qos, Byte* header){
	memcpy(Qos,header+1,1);
	return 1;
}

int get_TotalLen_from_header(Hex* TotalLength, Byte* header){
	Byte2Hex(TotalLength,header+2,2);
	return (int)*TotalLength;
}

int get_ID_from_header(Hex* ID,Byte* header){
	Byte2Hex(ID,header+4,2);
	return 0;
}

int get_frag_from_header(int* DF, int* MF, Hex* Offset,Byte* header){
	Byte2Hex(Offset,header+6,2); // get frag
	*DF = ((*Offset) & DFMask);
	*MF = ((*Offset) & MFMask);
	*Offset = ((*Offset) & OffsetMask);
	return 0;
}

/**
 * build frag(Hex) from int[DF,MF,Offset]
 */
int build_frag(int DF, int MF, int Offset, Hex* frag){
	Hex df,mf;
	Hex off = Offset>>3;    // off is x 8B(64bit)
	if(DF){
		df = mf = off = defaultFrag; // do not frag df=1 mf=0 off=0 
	} else if(MF) {                  // frag but not the last one
		df = zeroHex;                // df=0 mf=1 off=off
		mf = defaultMF;
		off = (Hex)off;
	} else {                         // frag and last one
		df = zeroHex;                // df=0 mf=0 off=off
		mf = zeroHex;
		off = (Hex)off;
	}
	*frag = (df | mf | off);
	return 0;
}

int get_payload_from_packet(Byte* Payload, Byte* packet, int HeaderLength, int TotalLength){
	memcpy(Payload,packet+HeaderLength,TotalLength-HeaderLength);
	return 0;
}



/**
 * get payload of a packet
 */
int parse_packet(Byte* Payload, Byte SrcIPAddr[4], Byte DestIPAddr[4],int* DF, int* MF, Hex* Offset, Byte* packet,Byte localIPAddr[4]){
	printf("\n=====parse a packet===== \n");
	Byte* header = (Byte*)malloc(60*sizeof(Byte));
	int HeaderLength = get_header(header,packet);
	if(HeaderLength < 20){
		if(HeaderLength == 0)
			printf("invalid checksum in packet\n");
		else
			printf("invalid packet Header!!!\n");	
		
		goto Failure;
	}

	get_addr_from_header(DestIPAddr,SrcIPAddr,header);
	int flag = CompareBytes(DestIPAddr,localIPAddr,4);
	if(flag == 0){
		printf("No, the ip addr can't fit\n");
		goto Failure;
	}

	Hex ID;
	get_ID_from_header(&ID,header);
	printf("Packet ID %04x\n",ID);

	Hex TotalLen;
	int intTL = get_TotalLen_from_header(&TotalLen,header);

	get_frag_from_header(DF,MF,Offset,header);
	if(*DF){
		printf("DF=1, this is a single packet\n");
	} else if(*MF){
		printf("DF=0 MF=1, this is a small-packet\n");
	} else {
		printf("DF=0 MF=0, this a small-packet,and it is the end small-packet\n");
	}

	int PayloadLength = intTL - HeaderLength;
	get_payload_from_packet(Payload,packet,HeaderLength,intTL);
	printf("====parse packet success: payloadlen=%d ====\n",PayloadLength);

	return PayloadLength;

Failure: // something wrong in packet
	printf("====parse packet not success ====\n",PayloadLength);
	Payload[0] = '\0';
	return 0;
}

int print_IP(Byte addr[4],char* LeftPad){
	printf(LeftPad);
	printf("%d.%d.%d.%d\n",addr[0],addr[1],addr[2],addr[3]);
}

/**
 * build a lot of packet from a big payload
 * but if len(payload) < MTU then build only one packet
 **/
int build_packet_arr(Byte* PacketArr[],int ArrLen[], Byte Qos, Hex ID, Byte TTL, Byte protocol, Byte SrcIPAddr[4], Byte DestIPAddr[4],Byte* payload, int PayloadLength){
	int itr;
	Byte* Tmp;
	Hex frag;
	if(PayloadLength <= MTU){   // not frag
		printf("Not frag\n");
		Tmp = (Byte*)malloc(1500*sizeof(Byte));
		build_frag(1,0,0,&frag);
		ArrLen[0] = build_packet(Tmp, Qos, ID, frag, TTL, protocol, SrcIPAddr, DestIPAddr, payload, PayloadLength);
		PacketArr[0] = Tmp;
		return 1;
	} else {
		int PacketCnt = (PayloadLength / MTU);
		int ByteLeft = PayloadLength % MTU;

		for(itr=0;itr<PacketCnt;itr++){
			// build full packet which len is MTU
			Tmp = (Byte*)malloc(1500*sizeof(Byte));
			if(ByteLeft){            // these are all middle packet, not last one
				build_frag(0,1,MTU*itr,&frag);
			} else if(itr == PacketCnt-1){ // last one
				build_frag(0,0,MTU*itr,&frag);
			} else {                       // middle one
				build_frag(0,1,MTU*itr,&frag);
			}
			ArrLen[itr] = build_packet(Tmp, Qos, ID, frag, TTL, protocol, SrcIPAddr, DestIPAddr, payload+itr*(MTU), MTU);
			PacketArr[itr] = Tmp;
		}
		if(ByteLeft){
			Tmp = (Byte*)malloc(1500*sizeof(Byte));
			build_frag(0,0,MTU*itr,&frag);
			ArrLen[itr] = build_packet(Tmp, Qos, ID, frag, TTL, protocol, SrcIPAddr, DestIPAddr, payload+itr*(MTU), ByteLeft);
			PacketArr[itr] = Tmp;
			PacketCnt++;
		}
		return PacketCnt;
	}
}

typedef struct {
	Byte* payload;
	int len;
	int offset;
}FragHelper;

int FlagComp(const void*p1, const void*p2) {   // offset small, place small
	return(*(FragHelper*)p2).offset > (*(FragHelper*)p1).offset ? -1 : 1;
}

/**
 * get a payload from packet[], use fragment assembling method
 * if PackCnt == 0; do not assembling
 * make sure the payload have enough memory
 * such as len(Payload) == (MTU+20)*PackCnt
 */
int parse_packet_arr(Byte* packet[], int PacketCnt, Byte* Payload, int* PayloadLen,Byte localIPAddr[4]){
	Byte parseSrcIPAddr[4], parseDestIPAddr[4];
	int parseDF, parseMF,i;
	Hex parseOffset;
	int PayloadLength;
	Byte* Tmp;

	if(PacketCnt == 1){     // no frag
		Byte* parsePayload = (Byte*)malloc(1500*sizeof(Byte));
		PayloadLength = parse_packet(parsePayload, parseSrcIPAddr, parseDestIPAddr, &parseDF, &parseMF, &parseOffset, packet[0],localIPAddr);
		memcpy(Payload,parsePayload,PayloadLength);
		*PayloadLen = PayloadLength;
		free(parsePayload);
	} else {                // assembling
		FragHelper PayLoads[PacketCnt];

		// Byte* payloads[PacketCnt];
		// int Lengths[PacketCnt];

		for(i=0;i<PacketCnt;i++){
			Byte* parsePayload = (Byte*)malloc(1500*sizeof(Byte));
			PayloadLength = parse_packet(parsePayload, parseSrcIPAddr, parseDestIPAddr, &parseDF, &parseMF, &parseOffset, packet[i],localIPAddr);
		
			PayLoads[i].payload = parsePayload;
			PayLoads[i].len = PayloadLength;
			PayLoads[i].offset = parseOffset;

			printf("Payload length is:%d\n",PayloadLength);
			// print_format(parsePayload,"%02x ",PayloadLength,"Payload in hex:");
			// print_format(parsePayload,"%c",PayloadLength,"Payload in char:");
			// print_IP(parseDestIPAddr,"Dest IP ADDR:");
			// print_IP(parseSrcIPAddr,"SRC IP ADDR:");
		}

		qsort(&PayLoads[0], PacketCnt, sizeof(PayLoads[0]),FlagComp);
		int TotalLen = 0;
		for(i=0;i<PacketCnt;i++){
			Tmp = PayLoads[i].payload;
			PayloadLength = PayLoads[i].len;
			memcpy(Payload+TotalLen,Tmp,PayloadLength);
			TotalLen += PayloadLength;
		}
		*PayloadLen = TotalLen;
	}
}


#ifndef TOTALTEST
int main(int argc, char* argv[]){
	printf("==========Test Start==========\n");
	Byte* packet = (Byte*)malloc(1500*sizeof(Byte));
	int PayloadLength = strlen(Payload);
	int TotalLength = build_packet(packet, defaultQos, defaultID, defaultFrag,
	 					defaultTTL, UDPprotocol, localSrcIPAddr, localDestIPAddr,Payload,PayloadLength);
	print_format(packet,"%02x ",TotalLength,"packet in hex:");

	Byte* header = (Byte*)malloc(60*sizeof(Byte));
	int hl = get_header(header,packet);
	printf("Header Length %d\n",hl);
	print_format(header,"%02x ",hl,"header in hex:");

	Byte parseSrcIPAddr[4], parseDestIPAddr[4];
	int parseDF, parseMF;
	Hex parseOffset;

	Byte* parsePayload = (Byte*)malloc(1500*sizeof(Byte));
	PayloadLength = parse_packet(parsePayload, parseSrcIPAddr, parseDestIPAddr, &parseDF, &parseMF, &parseOffset, packet,localSrcIPAddr);
	print_format(parsePayload,"%02x ",PayloadLength,"Payload in hex:");
	print_format(parsePayload,"%c",PayloadLength,"Payload in char:");

	print_IP(parseDestIPAddr,"Dest IP ADDR:");
	print_IP(parseSrcIPAddr,"SRC IP ADDR:");
	printf("==========Test End==========\n");


	printf("==========Test frag function =========\n");
	int BigLen = sizeof(BigPayload);
	memset(BigPayload,'Z',BigLen);
	
	// Let the start Payload is different from others
	// when in parse state, if the payload start with "U1U"
	// and the second payload start with "U2U" it mean assembling success
	BigPayload[0] = 'U';
	BigPayload[1] = '1';
	BigPayload[2] = 'U';

	BigPayload[1401] = 'U';
	BigPayload[1402] = '2';
	BigPayload[1403] = 'U';

	Byte* PacketArr[100];
	int LenArr[100];
	int ArrLen = build_packet_arr(PacketArr, LenArr,defaultQos, defaultID, 
					defaultTTL, UDPprotocol, localSrcIPAddr, localDestIPAddr,BigPayload,BigLen);

	int parseBigLen;	
	Byte* parseBigPayload = (Byte*)malloc(ArrLen*1500*sizeof(Byte));
	parse_packet_arr(PacketArr,ArrLen,parseBigPayload,&parseBigLen,localSrcIPAddr);
	printf("Big payloads length:%d\n",parseBigLen);
	print_format(parseBigPayload,"%c",parseBigLen,"Payload in char:");

	printf("==========frag test success =========\n");

	return 0;
}
#endif
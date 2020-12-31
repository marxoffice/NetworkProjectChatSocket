#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Constants.h"

// #include <linux/udp.h>

#ifndef TOTALTEST
Hex localSrcPortAddr = 0x1122;
Hex localDestPortAddr = 0x1122;
Byte localSrcIPAddr[4] = {0x01,0x02,0x03,0x04};
Byte localDestIPAddr[4] = {0x01,0x02,0x03,0x04};
Byte* Payload = "Hello World, This is a test of UDP segment in Sending state.";
#endif

void print_port(Hex port,char* LeftPad){
	printf(LeftPad);
	printf("%d",port);
    if(port < 1024){
        printf(" BE CAREFUL, THIS PORT IS A WELL-KNOWN PORT\n");
    } else {
        printf(" This is a simple port\n");
    }
}

/**
 * build a ipv4 packet
 * 
 */
int build_segment(Byte* segment, Hex SrcPort, Hex DestPort, Byte SrcIPAddr[4], Byte DestIPAddr[4],Byte* payload){
	// assert len(Payload) <= 65515

    int PayloadLen = strlen(payload);
    Hex TotalLen = (Hex)(PayloadLen + 8);
    Hex cksum = 0;

    // Step1 build fake header
    Byte* FakeSegment = (Byte*)malloc(65536*sizeof(Byte));
    memcpy(FakeSegment,&SrcIPAddr[0],4);
    memcpy(FakeSegment+4,&DestIPAddr[0],4);
    memcpy(FakeSegment+8,&zeroByte,1);
    memcpy(FakeSegment+9,&UDPprotocol,1);
    Hex2Byte(FakeSegment+10,TotalLen);

    // Step2 build real header
    Hex2Byte(FakeSegment+12,SrcPort);
    Hex2Byte(FakeSegment+14,DestPort);
    Hex2Byte(FakeSegment+16,TotalLen);
    Hex2Byte(FakeSegment+18,cksum);

    // Step3 link payload
    memcpy(FakeSegment+20,payload,PayloadLen);

    // Step4 compute checksum and feed them
    cksum = checksum(FakeSegment,(int)TotalLen+12);
    Hex2Byte(FakeSegment+18,cksum);

    // Step5 cut fake header
    memcpy(segment,FakeSegment+12,TotalLen);

    // print_format(FakeSegment,"%02x ",TotalLen+12,"udp fakesegment in hex:");
    free(FakeSegment);

	return (int)TotalLen;
}

int get_length_udp(Hex* length, Byte* segment){
    Byte2Hex(length,segment+4,2);
    return (int)*length;
}

int get_port_udp(Hex* SrcPortAddr, Hex* DestPortAddr, Byte* segment){
    Byte2Hex(SrcPortAddr,segment,2);
    Byte2Hex(DestPortAddr,segment+2,2);
    return 1;
}

int parse_segment(Byte* payload, Hex* TotalLength,Hex* SrcPort, Hex* DestPort, Byte SrcIPAddr[4], Byte DestIPAddr[4], Byte* segment, Hex localPortAddr){
    printf("\n=========Parse a segment=========\n");
    int TotalLen = get_length_udp(TotalLength,segment);
    int PayloadLen = TotalLen - 8;

    // before parse we should build Fake header
    Byte* FakeSegment = (Byte*)malloc(65536*sizeof(Byte));
    memcpy(FakeSegment,&SrcIPAddr[0],4);
    memcpy(FakeSegment+4,&DestIPAddr[0],4);
    memcpy(FakeSegment+8,&zeroByte,1);
    memcpy(FakeSegment+9,&UDPprotocol,1);
    Hex2Byte(FakeSegment+10,TotalLen);
    memcpy(FakeSegment+12,segment,TotalLen);

    // compute checksum and check
    Hex cksum = checksum(FakeSegment,TotalLen+12);
    free(FakeSegment);
    if(cksum){ // cksum is invalid
        printf("checksum is invalid in segment\n");
        goto Failure;
    }

    // get addr and payload
    get_port_udp(SrcPort,DestPort,segment);
    if(*DestPort != localPortAddr){
        printf("No, this is not a real port\n");
        goto Failure;
    }

    memcpy(payload,segment+8,PayloadLen);
    printf("======== parse udp segment success ======\n");


	return PayloadLen;

Failure: // something wrong in segment
	printf("====parse not success ====\n");
	payload[0] = '\0';
	return 0;
}


#ifndef TOTALTEST
int main(){
    printf("==========Test Start==========\n");

    Byte* segment = (Byte*) malloc(65535*sizeof(Byte));
    int TotalLen = build_segment(segment, localSrcPortAddr, localDestPortAddr, localSrcIPAddr, localDestIPAddr,Payload);
    print_format(segment,"%02x ",TotalLen,"udp segment in hex:");

    Byte* parsePayload = (Byte*)malloc(65515*sizeof(Byte));
    Hex parseTotalLength, parseSrcPort, parseDestPort;
    int PayloadLength = parse_segment(parsePayload, &parseTotalLength,&parseSrcPort, &parseDestPort, localSrcIPAddr, localDestIPAddr, segment,localSrcPortAddr);

    print_format(parsePayload, "%02x ",PayloadLength,"Payload in hex:");
	print_format(parsePayload, "%c",PayloadLength,"Payload in char:");

    print_port(parseDestPort,"Dest port ADDR:");
	print_port(parseSrcPort,"SRC port ADDR:");
	printf("==========Test End==========\n");
    return 0;
}
#endif
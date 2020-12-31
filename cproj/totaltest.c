#ifndef TOTALTEST   // close the main function and som constants 
#define TOTALTEST   // of *Utils.c(Frame,Packet,UDP)


#include "FrameUtils.c"
#include "PacketUtils.c"
#include "UDPUtils.c"
#include "Constants.h"

Hex localSrcPortAddr = 0x1122;
Hex localDestPortAddr = 0x1122;
Byte localSrcIPAddr[4] = {0x01,0x02,0x03,0x04};
Byte localDestIPAddr[4] = {0x01,0x02,0x03,0x04};
Byte SrcAddr[6] = {0x01,0x02,0x03,0x04,0x05,0x06};
Byte DestAddr[6] = {0x11,0x12,0x13,0x14,0x15,0x16};
Byte DataType[2] = {0x88,0x66};


int BeforeSend(Byte* Payload,Byte* frames[],int frameLen[]){
    int i;
    // Send Step1 segment
    Byte* segment = (Byte*) malloc(65535*sizeof(Byte));
    int TotalLen = build_segment(segment, localSrcPortAddr, localDestPortAddr, localSrcIPAddr, localDestIPAddr,Payload);

    // Send Step2 packet and frag
    Byte* PacketArr[100];
    int LenArr[100];
    int ArrLen = build_packet_arr(PacketArr, LenArr,defaultQos, defaultID, 
					defaultTTL, UDPprotocol, localSrcIPAddr, localDestIPAddr,segment,TotalLen);

    
    for(i=0;i<ArrLen;i++){
        Byte* frame = (Byte*)malloc(1516* sizeof(Byte));
        
        // Send Step3 Frame 
        int FrameLen = build_frame(frame,DestAddr,SrcAddr,DataType,PacketArr[i],LenArr[i]);

        frames[i] = frame;
        frameLen[i] = FrameLen;
    }
    return ArrLen;
}

int BeforeRecv(Byte* frame, Byte* data){
    // Recv Step1 frame
    Byte* Tmp = (Byte*)malloc(1516*sizeof(Byte));

    int flag = parse_frame(Tmp,frame,DestAddr);

    Byte* packarr[100];
    packarr[0] = Tmp;

    // Recv Step2 Packet
	int parseBigLen;
	Byte parseIPPayload[1500];
	parse_packet_arr(packarr,1,parseIPPayload,&parseBigLen,localSrcIPAddr);

    // Recv Step3 segment
    Hex parseTotalLength, parseSrcPort, parseDestPort;
    int PayloadLength = parse_segment(data, &parseTotalLength,&parseSrcPort, &parseDestPort, localSrcIPAddr, localDestIPAddr, parseIPPayload,localSrcPortAddr);
    return PayloadLength;
}


#ifndef MARX_SOCKET_TEST
Byte* Payload = "Hello World, This is a total test of UDP-IP-FRAME.";

/**
 * without socket
 * full test of frame send-receive
 *              packet send-receive
 *              udp send-receive
 */
int main(){
    Payload = (Byte*)malloc(65535*sizeof(Byte));
    ReadFileBytes("test.txt",Payload,65535);
    printf("%d\n",strlen(Payload));

    printf("==========Total Test Start==========\n");
    int i;
    // Send Step1 segment
    Byte* segment = (Byte*) malloc(65535*sizeof(Byte));
    int TotalLen = build_segment(segment, localSrcPortAddr, localDestPortAddr, localSrcIPAddr, localDestIPAddr,Payload);

    // Send Step2 packet and frag
    Byte* PacketArr[100];
    int LenArr[100];
    int ArrLen = build_packet_arr(PacketArr, LenArr,defaultQos, defaultID, 
					defaultTTL, UDPprotocol, localSrcIPAddr, localDestIPAddr,segment,TotalLen);

    Byte* acceptPacketArr[100];
    Byte* Tmp;
    for(i=0;i<ArrLen;i++){
        Byte* frame = (Byte*)malloc(1516* sizeof(Byte));
        Byte* readFrame = (Byte*)malloc(1516* sizeof(Byte));
        Byte* Tmp = (Byte*)malloc(1516* sizeof(Byte));
        
        // Send Step3 Frame 
        int FrameLen = build_frame(frame,DestAddr,SrcAddr,DataType,PacketArr[i],LenArr[i]);

        // This part is the real transmission process
        // the method you use is not important,
        // you can use file (like below codes) , pipe, socket and so on
        // the only rule you should obey is that (send == receive)
        // for example if you write it in BIGEDIAN but read in LITTLEENDIAN
        // this will cause fault.
        WriteFileBytes("test.frame",frame,FrameLen);

        ReadFileBytes("test.frame",readFrame,FrameLen);

        // Recv Step1 frame
        int flag = parse_frame(Tmp,readFrame,DestAddr);

        acceptPacketArr[i] = Tmp;
        free(frame);
        free(readFrame);
    }

    // Recv Step2 Packet
	int parseBigLen;
	Byte* parseIPPayload = (Byte*)malloc(ArrLen*1500*sizeof(Byte));
	parse_packet_arr(acceptPacketArr,ArrLen,parseIPPayload,&parseBigLen,localSrcIPAddr);
	printf("Big payloads length:%d\n",parseBigLen);
	// print_format(parseIPPayload,"%c",parseBigLen,"Payload in char:");

    // Recv Step3 segment
    Byte* parsePayload = (Byte*)malloc(65515*sizeof(Byte));
    Hex parseTotalLength, parseSrcPort, parseDestPort;
    int PayloadLength = parse_segment(parsePayload, &parseTotalLength,&parseSrcPort, &parseDestPort, localSrcIPAddr, localDestIPAddr, parseIPPayload,localSrcPortAddr);

    // print payload
    print_format(parsePayload, "%02x ",PayloadLength,"Payload in hex:");
	print_format(parsePayload, "%c",PayloadLength,"Payload in char:");

    print_port(parseDestPort,"Dest port ADDR:");
	print_port(parseSrcPort,"SRC port ADDR:");
	printf("==========Total Test End==========\n");

    return 0;
}

#endif

#endif
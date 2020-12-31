app: app.c
	gcc app.c -o ./bin/app -lpthread
frame: FrameUtils.c
	gcc FrameUtils.c -o ./bin/frame
packet: PacketUtils.c
	gcc PacketUtils.c -o ./bin/packet
segment: UDPUtils.c
	gcc UDPUtils.c -o ./bin/segment
test: totaltest.c
	gcc totaltest.c -o ./bin/test
SendRecv: sender.c receiver.c
	gcc sender.c -o ./bin/sender
	gcc receiver.c -o ./bin/receiver

clean:
	rm -rf ./bin/*
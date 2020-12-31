# 计算机网络project：网络分层及socket传输

> 通过自己实现链路层、网络层、传输层的封装，来体验和领会网络分层的思想。由于本项目的目的是理解网络层次和结构，使用纯C语言开发(socket部分调用了linux的socket系统库，其余都是C标准库)，并且代码不考虑效率，以功能实现为目的，不会使用C语言的魔法语法

## 本项目已实现的功能

- 链路层以太网帧Frame的封装

- 网络层Ipv4包packet的封装

- 传输层udp段segement封装

- socket传输，这里主要参考课上给的socket接口资料(基于linux的socket库)和网络上的资料，本部分必须在linux上运行

## 本项目主要解决的问题

- 校验算法：参考了教材和部分博客实现了CRC和反码校验
- 分片和重组：由于传输有大小限制，所以需要进行分片和重组工作(注意：由于IP没有重传机制，不建议使用IP分片进行Linux网络编程)。
- 字节顺序问题：由于各种操作系统和编程语言字节序不同，但网络统一使用大端顺序，所以务必保证字节顺序满足大端需求，以至于PC1的数据发送到PC2还可以正常解析。例如PC1发送0x12345678，若PC2理解为0x78563412，则数据传输正确，但表示错误。

## 展示

### 1.构建和解开Frame测试

```assembly
## 先编译再运行
make frame
./bin/frame

## 可以看到 frame的封装和解包都正确
```

![FrameTest](https://gitee.com/marxoffice/marxtuku/raw/master/img/FrameTest.png)

### 2.构建和解开Packet测试

```assembly
make packet
./bin/packet

## 第一张图可以看到短payload封装packet正确
## 第二张图可以看到大的payload，ip分片和合并也正确（标志字符"U1U"出现在"U2U"前面）
```

![image-20201231133457524](https://gitee.com/marxoffice/marxtuku/raw/master/img/PacketTest.png)

![image-20201231133638602](https://gitee.com/marxoffice/marxtuku/raw/master/img/PacketTest2.png)

### 3.构建和解开UDP测试

```assembly
make segment
./bin/segment

## udp segment测试正确
```

![image-20201231133836801](https://gitee.com/marxoffice/marxtuku/raw/master/img/UdpTest.png)

### 4.完整测试(使用文件传输，并调用以上1-3测试，输入大，需要分片)

```assembly
make test
./bin/test
## 这是一个大输入量的测试，可以测试分片和重组，以及从UDP->Packet->Frame，下面四张图都是输出
## 第一张图 可以看出分了10个packet，并封装每个packet到frame，开始的前几个frame长度都是1420
## 第二张图 可以看出frame和packet解析正常
## 第三张图 可以看出udp解析正常，并且获得了结果：先以16进制输出，再输出char，char的顺序是对的
## 第四张图 可以看出 传输和解析都正确，结果输出也是对的
```

![image-20201231134837936](https://gitee.com/marxoffice/marxtuku/raw/master/img/test1.png)

![image-20201231135000520](https://gitee.com/marxoffice/marxtuku/raw/master/img/test2.png)

![image-20201231135206734](https://gitee.com/marxoffice/marxtuku/raw/master/img/test3.png)

![image-20201231135334726](https://gitee.com/marxoffice/marxtuku/raw/master/img/test4.png)

### 5.单步发送测试(使用socket传输，并调用1-3测试)

```assembly
make SendRecv

## 在一个终端上作为接收者
./bin/receiver

## 在另一个终端上(可以是同一台电脑，也可以不是，只要IP地址填对就可以)
./bin/sender 127.0.0.1 helloworld   ## 同一台电脑
./bin/sender ipaddr helloworld      ## 不同电脑,注意，跨网络需要公网IP才行
```

![image-20201231135844861](https://gitee.com/marxoffice/marxtuku/raw/master/img/samepc.png)

![image-20201231140222458](https://gitee.com/marxoffice/marxtuku/raw/master/img/DifferentPC.png)

### 6.交互聊天测试(使用socket传输，调用1-3测试，构建交互聊天程序)

```assembly
## 这个应用调用了测试5的单步发送程序，本质上和之前没什么区别
## 单步发送程序，发送一个message就结束了
## 这里使用Pthread来让一个程序 同时可以接收和发送
## 这个应用既可以本地测试 127.0.0.1
## 也可以跨网测试 注意：跨网时如果没有公网IP，可能找不到
make
./bin/app IPServer ServerPort MyPort

## 这一部分截图较多，所以录制了演示视频sock_chat
```
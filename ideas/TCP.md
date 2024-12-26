# 一、TCP理论学习和实践

## 1、理论

TCP状态转移，粗体虚线是服务器，粗体实线是客户端，细实线是异常（其他）状态转移。

![tcp status machine](/home/xs/basic_skill/computer-network/ideas/imgs/tcp status machine.png)

![img](/home/xs/basic_skill/computer-network/ideas/imgs/v2-ed2b08886f66eb2b8257aa82e9d15504_r.jpg)

## 2、实践

### 2.1 三次握手

宿主机IP：10.10.64.180；子网掩码：2555.255.240.0；MAC：00:d8:61:33:05:22

虚拟机IP：10.10.64.173；子网掩码：2555.255.240.0；MAC：08:00:27:d6:c0:7a

宿主机和虚拟机可以互相ping通。

按照《Linux高性能服务器编程（游双）》中**3.3节**的命令来实践TCP三次握手的实验。

//  虚拟机windows上安装TCP&UDP测试工具，找不到，都是流氓软件，烦死了。

// 2024-12-25 10:58 找到一个百度网盘链接，终于装上了。

##### （1）创建服务器进程，在80端口监听

![image-20241225110045475](/home/xs/basic_skill/computer-network/ideas/imgs/image-20241225110045475.png)

##### （2）宿主机telnet虚拟机

宿主机设置tcpdump包筛选

```shell
sudo tcpdump -i eno1 '(src 10.10.64.180 and dst 10.10.64.173)or(src 10.10.64.173 and dst 10.10.64.180)'
```

宿主机telnet虚拟机

![image-20241225110840324](/home/xs/basic_skill/computer-network/ideas/imgs/image-20241225110840324.png)

虚拟机服务器进程显示：来自10.10.64.180:45972的进程

![image-20241225110645614](/home/xs/basic_skill/computer-network/ideas/imgs/image-20241225110645614.png)

捕获到的packets：

```shell
1、11:03:22.366225 IP bogon.45972 > bogon.http: Flags [S], seq 2057489536, win 64240, options [mss 1460,sackOK,TS val 1165263521 ecr 0,nop,wscale 7], length 0
2、11:03:22.367613 IP bogon.http > bogon.45972: Flags [S.], seq 3564628434, ack 2057489537, win 65535, options [mss 1460,nop,wscale 8,nop,nop,sackOK], length 0
3、11:03:22.367660 IP bogon.45972 > bogon.http: Flags [.], ack 1, win 502, length 0
```

`bogon` 是 `tcpdump` 输出中的一个标记，表示源或目标 IP 地址属于无效、私有或保留地址。这种地址不应该出现在公共互联网通信中，通常是由配置错误、NAT 问题或内部网络通信造成的。

第一个报文：主机占据45972端口的进程 向 宿主机占据http(80)端口的进程建立**SYN**连接，序列号2057489536，以及附带一些用于建立TCP连接需要用到的信息options

第二个报文：虚拟机回应宿主机的TCP，序列号为服务器侧的，确认号是对第一个报文的序列号加1，即

2057489536+1==2057489537. 该报文为**SYN-ACK**

第三个报文：宿主机对第二个报文的确认，**ACK**

###### G哥总结：

这些数据包展示了 TCP 连接建立的过程（即 **三次握手**）：

1. 客户端发起连接请求（SYN）。
2. 服务器响应确认请求并同意建立连接（SYN-ACK）。
3. 客户端确认服务器的响应，连接建立完成（ACK）。

在此之后，数据传输就可以开始了。

**Flags [.]** 表示该包为 **ACK 包**，用于确认对方发送的消息或数据包。

##### (3) 输入内容

称呼宿主机为客户端（client），虚拟机为服务器（service）。

(1) telnet界面输入^]按回车

![image-20241225113854470](/home/xs/basic_skill/computer-network/ideas/imgs/image-20241225113854470.png)

(2) 服务器端显示^]，点击HEX，数值为5e 5d 0d 0a，按照ASCII编码对应的字符分别为 '^', ']', '\r', '\n'

(3) 捕获到的packet：

```shell
11:20:30.583754 IP bogon.45972 > bogon.http: Flags [P.], seq 1:5, ack 1, win 502, length 4: HTTP
11:20:30.627334 IP bogon.http > bogon.45972: Flags [.], ack 5, win 8212, length 0

```

[P.]：PUSH+ACK，推送数据以及ACK确认，序列号为1:5，因为是四个字符，一个字符一个字节的大小，共四字节。最后HTTP表示上层协议是HTTP。

第二行是服务器对刚才数据包的确认，ack为客户端数据序列号加1，刚好是5.

### 2.2 四次挥手

```shell
1、12:48:57.999661 IP bogon.45972 > bogon.http: Flags [F.], seq 25, ack 13, win 502, length 0
2、12:48:58.000900 IP bogon.http > bogon.45972: Flags [.], ack 26, win 8212, length 0
3、12:48:58.001421 IP bogon.http > bogon.45972: Flags [F.], seq 13, ack 26, win 8212, length 0
4、12:48:58.001460 IP bogon.45972 > bogon.http: Flags [.], ack 14, win 502, length 0
```

第一个报文：客户端向服务器发送关闭连接报文，**FIN+ACK**，（因为我刚才又发送了几条数据，所以seq变成25了）
第二个报文：服务器对客户端关闭连接报文的确认，**ACK**

第三个报文：服务器对客户端发送关闭连接报文，**FIN+ACK**

第四个报文：客户端对服务器关闭连接报文的确认，**ACK**

## 3、工具学习

#### 3.1 tcpdump




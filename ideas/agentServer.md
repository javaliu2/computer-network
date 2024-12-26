### 代理服务器实践

虚拟机和宿主机处于同一个网段，虚拟机设置代理服务器为宿主机，宿主机上使用squid作为代理服务器

安装使用squid的手册(https://ubuntu.com/server/docs/how-to-install-a-squid-server)

| role   | IP           | netmask       | 网络号     |
| ------ | ------------ | ------------- | ---------- |
| 宿主机 | 10.10.64.180 | 255.255.240.0 | 10.10.64.0 |
| 虚拟机 | 10.10.65.0   | 255.255.240.0 | 10.10.64.0 |

折腾一上午，在windows上安装wget，执行命令，但是输出和书里的不一样，没有显示连接到代理服务器，可能是因为这是windows系统的关系？不知道，反正代理服务器的设置没有问题，虚拟机可以通过宿主机的代理服务器进行网络访问。![image-20241226112013984](/home/xs/basic_skill/computer-network/ideas/imgs/agentServer.md)

宿主机代理服务器squid，/etc/squid/squid.conf配置：

```shell
acl localnet src 10.10.64.0/20
http_access allow localnet
```

虚拟机设置代理服务器：

![image-20241226112547775](/home/xs/basic_skill/computer-network/ideas/imgs/image-20241226112547775.png)

### liunx 命令

netstat（Network Statistics），浏览系统上网络进程的状态。

G哥介绍：

`netstat`（Network Statistics）是一个网络分析工具，用于显示网络连接、路由表、接口统计信息、伪连接等信息。它通常用于检查网络连接的状态，诊断网络问题，或了解系统的网络活动。`netstat` 命令可以显示出当前的网络连接、监听端口、路由信息等内容。

### 常用 `netstat` 用法

#### 1. **查看所有连接（包括监听和非监听）**

```bash
netstat -a
```

- 显示所有的网络连接（包括正在监听的端口）。
- 输出内容包括：协议类型、接收队列、发送队列、本地地址、外部地址和连接状态。

#### 2. **查看所有监听端口**

```bash
netstat -l
```

- 仅显示当前系统上正在监听的端口（即等待建立连接的端口）。

#### 3. **显示网络连接状态（包括协议）**

```bash
netstat -tuln
```

- `-t`：仅显示 TCP 连接。
- `-u`：仅显示 UDP 连接。
- `-l`：仅显示监听状态的端口。
- `-n`：显示数字形式的地址和端口号（不解析为主机名和服务名）。

举例输出：

```
Proto Recv-Q Send-Q Local Address           Foreign Address         State
tcp        0      0 0.0.0.0:8080           0.0.0.0:*               LISTEN
tcp6       0      0 :::80                   :::*                    LISTEN
```

#### 4. **显示正在使用的端口及其对应的进程**

```bash
netstat -tulnp
```

- `-p`：显示进程 ID 和程序名称。
- 例如：你可以看到监听端口和对应的进程名称及 PID（进程 ID）。

举例输出：

```
Proto Recv-Q Send-Q Local Address           Foreign Address         State       PID/Program name
tcp        0      0 0.0.0.0:80              0.0.0.0:*               LISTEN      1234/nginx
tcp6       0      0 :::8080                 :::*                    LISTEN      5678/java
```

#### 5. **显示详细的网络接口统计信息**

```bash
netstat -i
```

- `-i`：显示网络接口的统计信息，例如接收和发送的数据包、错误等。

举例输出：

```
Kernel Interface table
Iface      MTU    RX-OK RX-ERR RX-DRP RX-OVR  TX-OK TX-ERR TX-DRP TX-OVR Flg
eth0      1500   1000000      0      0      0   500000      0      0      0 BMRU
lo        65536  10000000      0      0      0   10000000      0      0      0 LRU
```

#### 6. **查看路由表**

```bash
netstat -r
```

- `-r`：显示系统的路由表信息，展示网络流量的转发路径。

举例输出：

```
Kernel IP routing table
Destination     Gateway         Genmask         Flags Metric Ref    Use Iface
192.168.1.0     0.0.0.0         255.255.255.0   U     0      0        0 eth0
0.0.0.0         192.168.1.1     0.0.0.0         UG    0      0        0 eth0
```

#### 7. **查看特定端口的连接**

```bash
netstat -an | grep 8080
```

- `-an`：显示所有的连接，不解析服务名（数字形式），然后用 `grep` 过滤出端口号 `8080` 相关的连接。

例如，这会显示所有涉及端口 `8080` 的连接（本地和远程端口）。

#### 8. **显示指定协议的网络连接**

```bash
netstat -t
```

- `-t`：显示所有的 TCP 网络连接。

```bash
netstat -u
```

- `-u`：显示所有的 UDP 网络连接。

#### 9. **显示所有连接及其状态的概要**

```bash
netstat -s
```

- `-s`：显示网络协议的统计信息，按协议分类（如 TCP、UDP、ICMP 等）。可以帮助查看各类协议的状态和错误信息。

举例输出：

```
Tcp:
    123456789 packets sent
    12345 segments retransmited
    23456789 packets received
    12345 segments received
    234 packets reassembled ok
    345 segments send out of order
```

#### 10. **显示连接的延迟信息**

```bash
netstat -i -c
```

- `-c`：持续显示网络接口统计信息，直到手动中止（Ctrl + C）。常用于实时监控。

### 示例输出解析

```bash
Proto Recv-Q Send-Q Local Address           Foreign Address         State
tcp        0      0 192.168.1.100:80       0.0.0.0:*               LISTEN
tcp        0      0 192.168.1.100:443      0.0.0.0:*               LISTEN
tcp        0      0 192.168.1.100:22       192.168.1.200:49234     ESTABLISHED
```

- **Proto**：协议（如 TCP 或 UDP）。
- **Recv-Q** 和 **Send-Q**：接收队列和发送队列的大小。
- **Local Address**：本地机器的 IP 地址和端口。
- **Foreign Address**：远程机器的 IP 地址和端口。
- **State**：连接的状态（如 LISTEN、ESTABLISHED、TIME_WAIT 等）。

### 总结

`netstat` 是一个非常强大的工具，用于查看系统的网络连接状态，了解哪些端口和服务在监听，哪些连接是活跃的，是否有潜在的网络问题等。通过它，你可以轻松地获取网络接口、连接、路由等信息，是系统管理员和网络工程师常用的网络诊断工具之一。

如果你对某些选项或输出有疑问，随时告诉我，我可以为你进一步解释！
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
// arp: address resolution protocol
#include <arpa/inet.h>  
#include <stdio.h>
/**
 * sender@xslab.org
 * recipient@xslab.org
 */

int main() {

    const char* host_name = "mail.xslab.org";
    /**
     * Make Internet host address in network byte order by combining the
     * network number NET with the local address HOST.
     * 按照网络字节序存储因特网主机地址, 其包含网络号和主机号.
     */
    struct in_addr **addr_list;

    int i = 0;
    char dest_ip[16]; // Mail server IP address, 255.255.255.255, 共15个字符, 加上'\0', 共16个
    
    /**
     * Description of data base entry for a single host.
     * 单个主机的数据库条目描述.
     */
    struct hostent *host;
    // Get IP from domain name
    if ((host = gethostbyname(host_name)) == NULL)
    {
        herror("gethostbyname");
        exit(EXIT_FAILURE);
    }

    addr_list = (struct in_addr **) host->h_addr_list;
    while (addr_list[i] != NULL)
        ++i;
    /**
     * inet_ntoa(in_addr IN): Convert Internet number in IN to ASCII representation. 
     * The return value is a pointer to an internal array containing the string.
     * 将变量IN中因特尔号码转换为ASCII表示. 返回值是一个指向 保存ip地址字符串数组 的指针.
     */
    printf("before inet_ntoa(): %d\n", *addr_list[i-1]);  // 16842879 == 0x01 01 00 00 7F，网络序，大端字节序

    strcpy(dest_ip, inet_ntoa(*addr_list[i-1]));

    printf("address count: %d\n", i);
    printf("mail server ip address: %s\n", dest_ip);

    return 0;
}

    
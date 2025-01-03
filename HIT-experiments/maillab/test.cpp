#include <netdb.h>
#include <stdlib.h>
#include <string.h>
// arp: address resolution protocol
#include <arpa/inet.h>  
#include <stdio.h>


int main() {

    const char* host_name = "mail.xslab.org";
    struct in_addr **addr_list;
    int i = 0;
    char dest_ip[16]; // Mail server IP address
    
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
    strcpy(dest_ip, inet_ntoa(*addr_list[i-1]));

    printf("mail server ip address: %s", dest_ip);

    return 0;
}

    
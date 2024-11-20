#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define NTP_SERVER_PORT 123

typedef struct
{
    uint8_t li_vn_mode; // Eight bits. li, vn, and mode.
    // li. Two bits. Leap indicator.
    // vn. Three bits. Version number of the protocol.
    // mode. Three bits. Client will pick mode 3 for client.

    uint8_t stratum;         // Eight bits. Stratum level of the local clock.
    uint8_t poll;            // Eight bits. Maximum interval between successive messages.
    uint8_t precision;       // Eight bits. Precision of the local clock.
    uint32_t rootDelay;      // 32 bits. Total round trip delay time.
    uint32_t rootDispersion; // 32 bits. Max error aloud from primary clock source.
    uint32_t refId;          // 32 bits. Reference clock identifier.
    uint32_t refTm_s;        // 32 bits. Reference time-stamp seconds.
    uint32_t refTm_f;        // 32 bits. Reference time-stamp fraction of a second.
    uint32_t origTm_s;       // 32 bits. Originate time-stamp seconds.
    uint32_t origTm_f;       // 32 bits. Originate time-stamp fraction of a second.
    uint32_t rxTm_s;         // 32 bits. Received time-stamp seconds.
    uint32_t rxTm_f;         // 32 bits. Received time-stamp fraction of a second.
    uint32_t txTm_s;         // 32 bits and the most important field the client cares about.Transmit time - stamp seconds.
    uint32_t txTm_f;         // 32 bits. Transmit time-stamp fraction of a second.
} ntp_packet;

// Function to get a setted package
static ntp_packet *get_Package_Setted()
{
    ntp_packet *packet = (ntp_packet *)malloc(sizeof(ntp_packet));
    packet->li_vn_mode = 0x1B;
    packet->stratum = 0;
    packet->poll = 0;
    packet->precision = 0;
    packet->rootDelay = 0;
    packet->rootDispersion = 0;
    packet->refId = 0;
    packet->refTm_s = 0;
    packet->refTm_f = 0;
    packet->origTm_s = 0;
    packet->origTm_f = 0;
    packet->rxTm_s = 0;
    packet->rxTm_f = 0;
    packet->txTm_s = 0;
    packet->txTm_f = 0;
    return packet;
}

char *get_IP(char *url)
{
    // Get IP from URL
    printf("\n[Debug] Getting IP from URL\n");
    struct addrinfo hints, *res;
    int err;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET; // IPV4
    hints.ai_socktype = SOCK_STREAM;

    if ((err = getaddrinfo(url, NULL, &hints, &res)) != 0)
    {
        printf("\n[Error] Failed to resolve hostname: %s\n", gai_strerror(err));
        return 1;
    }

    char ipstr[INET_ADDRSTRLEN];
    struct sockaddr_in *addr = (struct sockaddr_in *)res->ai_addr;
    inet_ntop(AF_INET, &(addr->sin_addr), ipstr, sizeof(ipstr));

    printf("\n[Debug] %s mapped to %s\n", url, ipstr);
    freeaddrinfo(res);
    return ipstr;
}

int main(void)
{
    // Get URL
    char url[2049];
    printf("Enter the URL to NTP request: ");
    scanf("%s", url);
    printf("\n[Debug] Your url: %s\n", url);

    char *ip = get_IP(url);

    // Start UDP Socket
    printf("\n[Debug] Starting UDP Socket\n");
    int sockfd;
    struct sockaddr_in server_addr;
    socklen_t addr_len = sizeof(server_addr);

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        printf("\n[Debug] Cannot get this Ip\n");
        return 1;
    }
    printf("[Debug] Configurating address");
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(NTP_SERVER_PORT); // Converte para ordem de bytes de rede
    if (inet_pton(AF_INET, "12002", &server_addr.sin_addr) <= 0)
    {
        close(sockfd);
        return 1;
    }

    // Recieve Message

    // Format Message

    return 0;
}
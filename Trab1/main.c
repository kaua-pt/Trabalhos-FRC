#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <time.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define NTP_SERVER_PORT 123
#define NTP_TIMESTAMP_DELTA 2208988800U
#define TIMEOUT_MS 20000

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
    printf("\n[Debug] Creating Package\n");

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
    printf("\n[Debug] Package Created\n");
    return packet;
}

int create_UDP_Socket(struct sockaddr_in server_addr, char *ip)
{
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0)
    {
        printf("\n[ERROR] Cannot get this Ip\n");
        return -1;
    }

    return sock;
}

char *get_IP(const char *url)
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
        printf("\n[ERROR] Failed to resolve hostname: %s\n", gai_strerror(err));
        return NULL;
    }

    char *ipstr = (char *)malloc(INET_ADDRSTRLEN);
    if (ipstr == NULL)
    {
        printf("[ERROR] Memory allocation failed\n");
        freeaddrinfo(res);
        return NULL;
    }

    struct sockaddr_in *addr = (struct sockaddr_in *)res->ai_addr;
    inet_ntop(AF_INET, &(addr->sin_addr), ipstr, INET_ADDRSTRLEN);

    printf("\n[Debug] %s mapped to %s\n", url, ipstr);

    freeaddrinfo(res);
    return ipstr;
}

struct sockaddr_in configure_Address(const char *ip)
{
    printf("\n[Debug] Setting address to send\n");
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(NTP_SERVER_PORT); 

    printf("\n[INFO] My ip: %s\n", ip);

    // POSSIBLE FAIL
    int convert_Ip_Result = inet_pton(AF_INET, ip, &server_addr.sin_addr);
    if (convert_Ip_Result <= 0)
        printf("\n[ERROR] Invalid IP address format\n");

    return server_addr;
}

int send_Message(int sock, ntp_packet *packet, struct sockaddr_in server_addr)
{
    ssize_t sent_bytes = sendto(sock, packet, sizeof(ntp_packet), 0,
                                (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (sent_bytes < 0)
    {
        printf("\n[ERROR] Error to send message\n");
        free(packet);
        close(sock);
        return 0;
    }
    printf("\n[Debug] Message sent successfully\n");
    return 1;
}

void transmit_Message(ntp_packet *packet)
{
    printf("\n[Debug] Translating message\n");
    uint32_t txTm_s = ntohl(*(uint32_t *)(packet + 43));
    txTm_s -= NTP_TIMESTAMP_DELTA;

    time_t timestamp = (time_t)txTm_s;
    struct tm *tm_info = localtime(&timestamp);

    char time_str[26];
    strftime(time_str, sizeof(time_str), "%a %b %d %H:%M:%S %Y", tm_info);
    printf("\nData/hora: %s\n", time_str);
}

int main(int argc, char *argv[])
{
    // Get URL
    if (argc != 2)
    {
        return 1;
    }
    const char *url = argv[1];

    printf("\n[Debug] Your url: %s\n", url);

    // Getting IP address
    char *ip = get_IP(url);
    if (ip == NULL)
        return 1;
    printf("\n[Debug] Mapped to %s\n", ip);


    char attempts = 2;

    while (attempts)
    {
        attempts--;

        // Setting address and starting socket
        printf("\n[Debug] Starting UDP Socket\n");
        struct sockaddr_in server_addr = configure_Address(ip);
        
        int sock = create_UDP_Socket(server_addr, ip);

        if (sock == -1)
            return 1;

        // Message
        ntp_packet *packet = get_Package_Setted();

        // Send message
        if (!send_Message(sock, packet, server_addr))
        {
            close(sock);
            continue;
        }

         // Receive message with timeout
        fd_set read_fds;
        struct timeval timeout;
        FD_ZERO(&read_fds);
        FD_SET(sock, &read_fds);
        timeout.tv_sec = 0;
        timeout.tv_usec = TIMEOUT_MS * 1000;

        // recebendo nd
        int select_result = select(sock + 1, &read_fds, NULL, NULL, &timeout);
        if (select_result == -1)
        {
            printf("\n[ERROR] Error with select\n");
            close(sock);
            return 1;
        }
        else if (select_result == 0)
        {
            printf("\n[ERROR] Timeout\n");
            close(sock);
            continue; // try again
        }

        socklen_t server_addr_len = sizeof(server_addr);
        if (recvfrom(sock, packet, sizeof(ntp_packet), 0,
                     (struct sockaddr *)&server_addr, &server_addr_len) < 0)
        {
            printf("\n[ERROR] Failed to receive message\n");
            close(sock);
            return 1;
        }

        printf("\n[Debug] Message received successfully\n");

        close(sock);

        // Without test
        transmit_Message(packet);
        return 0;
    }

    if(!attempts)
        printf("\nData/hora: não foi possível contactar servidor\n");

    return 0;
}
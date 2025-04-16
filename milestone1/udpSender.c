/**
 * @file udpSender.c
 * @author Yuchen Zhou (yzhou276@jh.edu)
 * @brief 
 * @version 0.1
 * @date 2025-04-15
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>

/* structure definition */

#define NUM_SAMPLES 512 // Number of samples in the packet
typedef struct dataPacket
{
    /* data */
    uint32_t packetID;
    int16_t  sdrData[NUM_SAMPLES];
} dataPacket;

#define PACKET_SIZE (sizeof(dataPacket)) // 1028

/* Prototype */
void usage(const char *executableName);

int main(int argc, char const *argv[])
{
    char* ip        = NULL;
    int port        = -1;
    int num_packets = -1;
    int opt         = 0;

    // Parse command line arguments
    while ((opt = getopt(argc, (char * const *)argv, "i:p:n:")) != -1) {
        switch (opt) {
            case 'i':
                ip = optarg; break;
            case 'p':
                port = atoi(optarg); break;
            case 'n':
                num_packets = atoi(optarg); break;
            default:
                usage(argv[0]); return -1;
        }
    }

    if (ip == NULL || port < 0 || num_packets < 0) {
        usage(argv[0]);
    }

    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    // create a sockaddr_in structure to hold the server address
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    if (inet_pton(AF_INET, ip, &server_addr.sin_addr) <= 0) {
        perror("Invalid IP address or IP Address not supported");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("PACKET_SIZE: %d\n", PACKET_SIZE);
    dataPacket packet;
    packet.packetID = 0;
    memset(packet.sdrData, 0, sizeof(packet.sdrData)); // Initialize sdrData to 0
    for(int i = 0; i < NUM_SAMPLES; i++) // Fill the sdrData array with some sample data
    {
        packet.sdrData[i] = i;
    }

    // send packets to the server
    for (int i = 0; i < num_packets; i++) 
    {
        packet.packetID = i;
        ssize_t bytes_sent = sendto(sockfd, &packet, PACKET_SIZE, 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
        if (bytes_sent < 0) {
            perror("Error sending packet");
            close(sockfd);
            exit(EXIT_FAILURE);
        }
        printf("Sent packet ID: %d\n", i);
    }

    printf("Sent %d packets to %s : %d\n", num_packets, ip, port);
    close(sockfd);

    return 0;
}

void usage(const char *executableName)
{
    printf("Usage: %s -i <ip> -p <port> -n <num_packets>\n", executableName);
    printf("  ip: The IP address of the server.\n");
    printf("  port: The port number of the server. The port must be greater than 0\n");
    printf("  num_packets: The number of packets to send. num_packets must be greater than 0\n");
    exit(EXIT_FAILURE);
}


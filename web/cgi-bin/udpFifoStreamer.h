/**
 * @file udpFifoStreamer.h
 * @author Yuchen Zhou (yzhou276@jh.edu)
 * @brief Header file for the UDP FIFO streamer
 * @details This file contains the function prototypes and definitions for the UDP FIFO streamer.
 * @version 0.1
 * @date 2025-04-20
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#ifndef _UDP_FIFO_STREAMER_H_
#define _UDP_FIFO_STREAMER_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <time.h>
#include <pthread.h>
#include <signal.h>
#include <semaphore.h>

/**
 * @brief The transmit path of the IP core is not enabled.
 * PS is not able to transmit data to PL.
 */
#define AXI4_STREAM_FIFO_BASE_ADDR       0x43c10000 // Base address of the AXI FIFO

// refer to AXI4-Stream FIFO LogiCORE IP Product Guide (PG080) 
// https://docs.amd.com/r/en-US/pg080-axi-fifo-mm-s/Register-Space
#define AXI4_STREAM_FIFO_RDFR_OFFSET     0x18 // Receive data FIFO reset offset
#define AXI4_STREAM_FIFO_RDFO_OFFSET     0x1C // Receive data FIFO occupancy offset
#define AXI4_STREAM_FIFO_RDFD_OFFSET     0x20 // Receive data FIFO data offset
#define AXI4_STREAM_FIFO_RLR_OFFSET      0x24 // Receive Length offset

#define AXI4_STREAM_FIFO_RDFR_RESET_MASK 0x000000FF // Reset mask for receive data FIFO
#define AXI4_STREAM_FIFO_RDFR_RESET_KEY  0x000000A5 // Reset key for receive data FIFO

/**
 * @brief UDP default settings
 */
#define DEFAULT_UDP_DEST_PORT   25344           // Default UDP port
#define DEFAULT_DEST_IP         "192.168.1.3"   // Default destination IP address

/**
 * @brief Data packet structure
 */
#define NUM_SAMPLES 256 // Number of samples in the packet
typedef struct dataPacket
{
    uint32_t packetID;                  // Packet ID
    int32_t  sdrData[NUM_SAMPLES];      // SDR data (256 samples)
} dataPacket;

#define PACKET_SIZE (sizeof(dataPacket)) // 1028 bytes

/**  Function Prototype */
volatile unsigned int * get_a_pointer(unsigned int phys_addr);
volatile unsigned int * get_pointer_to_axi_fifo();
void fifo_reset(volatile unsigned int *axi_fifo_base);
unsigned int fifo_get_current_occupancy(volatile unsigned int *axi_fifo_base);
unsigned int fifo_get_data(volatile unsigned int *axi_fifo_base);
void usage(const char *executableName);



#endif /* _UDP_FIFO_STREAMER_H_ */
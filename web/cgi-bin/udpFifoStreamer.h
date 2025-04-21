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

/**
 * @brief The transmit path of the IP core is not enabled.
 * PS is not able to transmit data to PL.
 */
#define AXI4_STREAM_FIFO_BASE_ADDR 0x43c10000 // Base address of the AXI FIFO

// refer to AXI4-Stream FIFO LogiCORE IP Product Guide (PG080) 
// https://docs.amd.com/r/en-US/pg080-axi-fifo-mm-s/Register-Space
#define AXI4_STREAM_FIFO_RDFR_OFFSET    0x18 // Receive data FIFO reset offset
#define AXI4_STREAM_FIFO_RDFO_OFFSET    0x1C // Receive data FIFO occupancy offset
#define AXI4_STREAM_FIFO_RDFD_OFFSET    0x20 // Receive data FIFO data offset
#define AXI4_STREAM_FIFO_RLR_OFFSET     0x24 // Receive Length offset

#define AXI4_STREAM_FIFO_RDFR_RESET_MASK 0x000000FF // Reset mask for receive data FIFO
#define AXI4_STREAM_FIFO_RDFR_RESET_KEY  0x000000A5 // Reset key for receive data FIFO


#endif /* _UDP_FIFO_STREAMER_H_ */
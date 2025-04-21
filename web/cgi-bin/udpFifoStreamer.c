/**
 * @file udpStreamer.c
 * @author Yuchen Zhou (yzhou276@jh.edu)
 * @brief update the UDP sender to read data from the FIFO and send it over UDP
 * @details This program reads data from the AXI4 Stream FIFO and sends it over UDP to a specified IP address and port.
 * @version 0.1
 * @date 2025-04-20
 * 
 * @copyright Copyright (c) 2025
 * 
 */

/* Include */
#include "udpFifoStreamer.h"

/* Function Prototype */

// the below code uses a device called /dev/mem to get a pointer to a physical
// address.  We will use this pointer to read/write the custom peripheral
volatile unsigned int * get_a_pointer(unsigned int phys_addr)
{

	int mem_fd = open("/dev/mem", O_RDWR | O_SYNC); 
    if (mem_fd < 0) {
        perror("Failed to open /dev/mem");
        return NULL;
    }
	void *map_base = mmap(0, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, mem_fd, phys_addr); 
    if (map_base == MAP_FAILED) {
        perror("mmap failed");
        close(mem_fd);
        return NULL;
    }
	volatile unsigned int *peripheral_base = (volatile unsigned int *)map_base; 
    close(mem_fd); // close the file descriptor after mapping
	return (peripheral_base);
}
/**
 * @file fifo_reader.c
 * @author Yuchen Zhou (yzhou276@jh.edu)
 * @brief 
 * @version 0.1
 * @date 2025-04-19
 * 
 * @copyright Copyright (c) 2025
 * 
 */

/* Include */
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

/* definition */

/** 
 * @brief definition of the radio tuner peripheral
 */
#define RADIO_PERIPH_ADDRESS 0x43c00000
#define RADIO_TUNER_FAKE_ADC_PINC_OFFSET    0
#define RADIO_TUNER_TUNER_PINC_OFFSET       1
#define RADIO_TUNER_CONTROL_REG_OFFSET      2
#define RADIO_TUNER_TIMER_REG_OFFSET        3
#define RADIO_TUNER_CTRL_RESET_MASK         0x00000001 // mask for reset bit in the control register
#define RADIO_TUNER_CTRL_RESET_ON           0x1         // reset on value for the control register
#define RADIO_TUNER_CTRL_RESET_OFF          0x0         // reset off value for the control register
#define RADIO_TUNER_CTRL_STREAM_EN_MASK     0x00000002 // mask for stream enable bit in the control register
#define RADIO_TUNER_CTRL_STREAM_EN_ON       0x1         // stream enable on value for the control register
#define RADIO_TUNER_CTRL_STREAM_EN_OFF      0x0         // stream enable off value for the control register

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
	volatile unsigned int *radio_base = (volatile unsigned int *)map_base; 
    close(mem_fd); // close the file descriptor after mapping
	return (radio_base);
}

// sanity check
void print_reg(volatile unsigned int *periph_base, unsigned int offset)
{
    unsigned int reg_value = *(periph_base + offset);
    printf("Register value at offset %u: %u\n", offset, reg_value);
}

// ----------------------------------------------------------------------------------------------------

/**
 * @brief Get a pointer to the AXI FIFO base address in user space
 * 
 * @return volatile unsigned int* Pointer to the AXI FIFO base address
 */
volatile unsigned int * get_pointer_to_axi_fifo()
{
    volatile unsigned int *axi_fifo_base = get_a_pointer(AXI4_STREAM_FIFO_BASE_ADDR);
    if (axi_fifo_base == NULL) {
        fprintf(stderr, "Failed to map AXI FIFO base address\n");
        return NULL;
    }
    return axi_fifo_base;
}

void fifo_reset(volatile unsigned int *axi_fifo_base)
{
    // reset the FIFO by writing the reset key to the reset register
    axi_fifo_base[AXI4_STREAM_FIFO_RDFR_OFFSET/4] = AXI4_STREAM_FIFO_RDFR_RESET_KEY;
}

unsigned int fifo_get_current_occupancy(volatile unsigned int *axi_fifo_base)
{
    // get the current occupancy of the FIFO by reading the occupancy register
    unsigned int occupancy = axi_fifo_base[AXI4_STREAM_FIFO_RDFO_OFFSET/4];
    return occupancy;
}

unsigned int fifo_get_data(volatile unsigned int *axi_fifo_base)
{
    // get the data from the FIFO by reading the data register
    return axi_fifo_base[AXI4_STREAM_FIFO_RDFD_OFFSET/4];
}

// ----------------------------------------------------------------------------------------------------

/**
 * @brief Get a pointer to the radio tuner base address in user space
 * 
 * @return volatile unsigned int* Pointer to the radio tuner base address
 */
volatile unsigned int * get_radio_tuner()
{
    volatile unsigned int *radio_base = get_a_pointer(RADIO_PERIPH_ADDRESS);
    if (radio_base == NULL) {
        fprintf(stderr, "Failed to map radio tuner base address\n");
        return NULL;
    }
    return radio_base;
}

/**
 * @brief disable the radio tuner
 * 
 * @param radio_base 
 */
void disable_radio_tuner(volatile unsigned int *radio_base)
{
    // disable the radio tuner by writing 1 to the reset bit in the control register
    *(radio_base + RADIO_TUNER_CONTROL_REG_OFFSET) |= RADIO_TUNER_CTRL_RESET_MASK;
    //print_reg(radio_base, RADIO_TUNER_CONTROL_REG_OFFSET);
}

/**
 * @brief enable the radio tuner
 * 
 * @param radio_base 
 */
void enable_radio_tuner(volatile unsigned int *radio_base)
{
    // enable the radio tuner by writing 0 to the reset bit in the control register
    *(radio_base + RADIO_TUNER_CONTROL_REG_OFFSET) &= ~RADIO_TUNER_CTRL_RESET_MASK;
    //print_reg(radio_base, RADIO_TUNER_CONTROL_REG_OFFSET);
}

/**
 * @brief Control the radio tuner stream flow
 * 
 * @param radio_base 
 * @param enable 
 */
void set_radio_tuner_stream(volatile unsigned int *radio_base, int enable)
{
    if (enable) {
        // enable the radio tuner stream by writing 1 to the stream enable bit in the control register
        *(radio_base + RADIO_TUNER_CONTROL_REG_OFFSET) |= RADIO_TUNER_CTRL_STREAM_EN_MASK;
    } else {
        // disable the radio tuner stream by writing 0 to the stream enable bit in the control register
        *(radio_base + RADIO_TUNER_CONTROL_REG_OFFSET) &= ~RADIO_TUNER_CTRL_STREAM_EN_MASK;
    }
    //print_reg(radio_base, RADIO_TUNER_CONTROL_REG_OFFSET);
}

/**
 * @brief Set the DDS frequency of the radio tuner
 * 
 * @param ptrToRadio Pointer to the radio tuner base address
 * @param tune_frequency Frequency to set in MHz
 */
void radioTuner_tuneRadio(volatile unsigned int *ptrToRadio, float tune_frequency)
{
	float pinc = tune_frequency*(float)(1<<27)/125.0e6;
	*(ptrToRadio+RADIO_TUNER_TUNER_PINC_OFFSET)=(int)pinc;
}

/**
 * @brief Set the ADC frequency of the radio tuner
 * 
 * @param ptrToRadio Pointer to the radio tuner base address
 * @param freq Frequency to set in MHz
 */
void radioTuner_setAdcFreq(volatile unsigned int* ptrToRadio, float freq)
{
	float pinc = freq*(float)(1<<27)/125.0e6;
	*(ptrToRadio+RADIO_TUNER_FAKE_ADC_PINC_OFFSET) = (int)pinc;
}

int main(int argc, char const *argv[])
{
    printf("Linux SDR milestone 2 - Radio + AXI4 Stream FIFO\n");
    // get a pointer to the radio tuner peripheral
    volatile unsigned int *radio_base = get_radio_tuner();
    if (radio_base == NULL) {
        fprintf(stderr, "Failed to get radio tuner base address\n");
        return -1;
    }
    volatile unsigned int *axi_fifo_base = get_pointer_to_axi_fifo();
    if (axi_fifo_base == NULL) {
        fprintf(stderr, "Failed to get AXI4 Stream FIFO base address\n");
        return -1;
    }

    enable_radio_tuner(radio_base);
    printf("Tuning Radio to 30MHz\n\r");
    radioTuner_tuneRadio(radio_base, 30000000); // tune to 30MHz
    printf("Fake ADC frequency to 30.01MHz\r\n");
    radioTuner_setAdcFreq(radio_base, 30001000); // set fake ADC frequency to 30.01MHz
    printf("Enable the radio tuner stream\r\n");
    set_radio_tuner_stream(radio_base, 1); // enable the radio tuner stream

    printf("I am going to read 10 seconds worth of data now\n");

    unsigned int numSamplesRead = 0;
    unsigned int targetSamples  = 480000;
    struct timespec start_time, end_time;

    fifo_reset(axi_fifo_base); // reset the FIFO

    clock_gettime(CLOCK_MONOTONIC, &start_time); // get the start time

    while(numSamplesRead < targetSamples)
    {
        unsigned int occupancy = fifo_get_current_occupancy(axi_fifo_base); // get the current occupancy of the FIFO 
        while(occupancy > 0 && numSamplesRead < targetSamples) // read data from the FIFO until it is empty or we have read enough samples
        {
            fifo_get_data(axi_fifo_base);
            occupancy--; // decrement the occupancy
            numSamplesRead++; // increment the number of samples read
        }
        if(numSamplesRead == targetSamples) // check if we have read enough samples
        {
            break; // exit the loop if we have read enough samples
        }
        usleep(50000); // sleep for 50ms to avoid busy waiting
    }

    clock_gettime(CLOCK_MONOTONIC, &end_time); // get the end time
    double elapsed_time = (end_time.tv_sec - start_time.tv_sec);
    elapsed_time += (end_time.tv_nsec - start_time.tv_nsec) / 1e9; // calculate the elapsed time in seconds
    printf("Elapsed time: %.9f seconds\n", elapsed_time);

    printf("I have read %u samples from the FIFO\n", numSamplesRead);
    printf("Disabling the radio tuner stream\r\n");
    set_radio_tuner_stream(radio_base, 0);  // disable the radio tuner stream
    printf("Disabling the radio tuner\r\n");
    disable_radio_tuner(radio_base);        // disable the radio tuner
    printf("Disabling the radio FIFO\r\n");
    fifo_reset(axi_fifo_base);              // reset the FIFO
    munmap((void *)radio_base, 4096);       // unmap the radio tuner base address
    munmap((void *)axi_fifo_base, 4096);    // unmap the AXI4 Stream FIFO base address

    return 0;
}

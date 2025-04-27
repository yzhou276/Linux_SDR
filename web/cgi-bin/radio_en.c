/**
 * @file radio_en.c
 * @author Yuchen Zhou (yzhou276@jh.edu)
 * @brief cgi-bin program to en/disable the radio tuner
 * @details 
 * @date 2025-04-26
 * 
 * @copyright Copyright (c) 2025
 * 
 */

/* Include */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

/* Definitions */
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

/* Function Prototypes */
volatile unsigned int * get_a_pointer(unsigned int phys_addr);
volatile unsigned int * get_radio_tuner();
void disable_radio_tuner(volatile unsigned int *radio_base);
void enable_radio_tuner(volatile unsigned int *radio_base);
void set_radio_tuner_stream(volatile unsigned int *radio_base, int enable);
void radioTuner_setMixerFreq(volatile unsigned int *ptrToRadio, float tune_frequency);
void radioTuner_setAdcFreq(volatile unsigned int* ptrToRadio, float freq);
void cgi_callback(void);

/* Global Variables */
int enable_radio = 0; // enable radio tuner flag

int main(void)
{
    cgi_callback(); // call the cgi callback function to handle the request

    // get a pointer to the radio tuner peripheral
    volatile unsigned int *radio_base = get_radio_tuner();
    if (radio_base == NULL) {
        fprintf(stderr, "Failed to get radio tuner base address\n");
        return -1;
    }
    
    if(enable_radio) {
        enable_radio_tuner(radio_base); // enable the radio tuner

    } else {
        disable_radio_tuner(radio_base); // disable the radio tuner
    }

    munmap((void *)radio_base, 4096); // unmap the radio tuner base address

    return 0;

}

void cgi_callback(void)
{
    // set the content type to HTML
    printf("Content-Type: text/html\n\n");
    // print the HTML header
    printf("<html>\n");
    printf("<head>\n");
    printf("<title>Radio Control</title>\n");
    printf("</head>\n");
    printf("<body>\n");
    printf("<h1>Control the Radio Tuner</h1>\n");

    char* len_str  = getenv("CONTENT_LENGTH"); // get the content length from the environment variable
    int len = len_str ? atoi(len_str) : 0; // convert the content length to an integer

    char* data = malloc(len + 1); // allocate memory for the data
    if (data == NULL) {
        printf("Failed to allocate memory for data<br>\n");
        return;
    }

    // read POST data from stdin
    if (fread(data, 1, len, stdin) != len) {
        printf("Failed to read POST data<br>\n");
        free(data);
        return;
    }

    data[len] = '\0'; // null-terminate the string

    //printf("POST data: %s<br>\n", data); // print the POST data for debugging
    printf("<h2>");

    // parse the POST data
    if (strstr(data, "action=set")) {
        printf("Radio Set (Enabled)\n");
        enable_radio = 1; // set the flag to enable the radio tuner
    } else if (strstr(data, "action=reset")) {
        printf("Radio Reset (Disabled)\n");
        enable_radio = 0; // set the flag to disable the radio tuner
    } else {
        printf("Error: Unknown action.\n");
    }
    free(data); // free the allocated memory

    printf("</h2>");
    printf("</body>\n");
    printf("</html>\n");
}


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
 * @brief Set the mixer frequency of the radio tuner
 * 
 * @param ptrToRadio Pointer to the radio tuner base address
 * @param tune_frequency Frequency to set in MHz
 */
void radioTuner_setMixerFreq(volatile unsigned int *ptrToRadio, float tune_frequency)
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
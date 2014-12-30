/**
 * @file	jamgpio.c
 * @brief	Raspberry Pi GPIO functions for JTAG programming
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h> //mmap

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define BCM2708_PERI_BASE        0x20000000
#define GPIO_BASE                (BCM2708_PERI_BASE + 0x200000) /* GPIO controller */
#define GPIO_LEN                 0xB4 // need only to map B4 registers

//gpio registers
static const unsigned int GPFSET0 = 7;
static const unsigned int GPFCLR0 = 10;
static const unsigned int GPFLEV0 = 13;
static const unsigned int GPFSEL0 = 0;
static const unsigned int GPFSEL1 = 1;
static const unsigned int GPFSEL2 = 2;

#define TCK 7  //bcm GPIO 7, P1 pin 26 (out)
#define TDI 8  //bcm GPIO 8, P1 pin 24 (out)
#define TMS 25 //bcm GPIO 25, P1 pin 22 (out)
#define TDO 24 //bcm GPIO 24, P1 pin 18 (in)
#define TCK_P1PIN 26 //bcm GPIO 7, P1 pin 26 (out)
#define TDI_P1PIN 24 //bcm GPIO 8, P1 pin 24 (out)
#define TMS_P1PIN 22 //bcm GPIO 25, P1 pin 22 (out)
#define TDO_P1PIN 18 //bcm GPIO 24, P1 pin 18 (in)

#include "jamgpio.h"

volatile unsigned int *gpio = NULL;

void gpio_init()
{
	int mem_fd = 0;
	void *regAddrMap = MAP_FAILED;

	/* open /dev/mem.....need to run program as root i.e. use sudo or su */
	if (!mem_fd)
	{
		if ((mem_fd = open("/dev/mem", O_RDWR|O_SYNC) ) < 0)
		{
			perror("can't open /dev/mem");
			exit (1);
		}
	}

	/* mmap IO */
	regAddrMap = mmap(
	  NULL,             //Any address in our space will do
	  GPIO_LEN,         //Map length
	  PROT_READ|PROT_WRITE|PROT_EXEC,// Enable reading & writing to mapped memory
	  MAP_SHARED|MAP_LOCKED,       //Shared with other processes
	  mem_fd,           //File to map
	  GPIO_BASE         //Offset to base address
	);

	if (regAddrMap == MAP_FAILED)
	{
		perror("mmap error");
		close(mem_fd);
		exit (1);
	}

	if(close(mem_fd) < 0)
	{ //No need to keep mem_fd open after mmap
	  //i.e. we can close /dev/mem
		perror("couldn't close /dev/mem file descriptor");
		exit(1);
	}
	
	gpio = (volatile unsigned int*) regAddrMap;

	printf("[RPi] Raspberry PI gpio mapped at 0x%p\n", gpio);
}

void gpio_exit()
{
	//unmap GPIO registers (physical memory)  from process memory
	if(munmap((void*)gpio, GPIO_LEN) < 0)
	{
		perror("munmap (gpio) failed");
		exit(1);
	}
}

/*
 * gpio_setPinDir() - sets the direction of a pin to either input or 
 * output
 * 
 * @param pinnum  GPIO pin number as per the RPI's  BCM2835's standard definition
 * @param dir     pin direction can be INPUT for input or OUTPUT for output
 */
void gpio_setPinDir(unsigned int pinnum, const unsigned int dir)
{
	if (gpio == NULL)
		return;

	if (dir == OUTPUT)
	{
		switch(pinnum/10) {	
			case 0:
				*(gpio + GPFSEL0) &= ~(7<<(((pinnum)%10)*3));
				*(gpio + GPFSEL0) |=  (1<<(((pinnum)%10)*3));
				break;
			case 1:
				*(gpio + GPFSEL1) &= ~(7<<(((pinnum)%10)*3));
				*(gpio + GPFSEL1) |=  (1<<(((pinnum)%10)*3));
				break;
			case 2:
				*(gpio + GPFSEL2) &= ~(7<<(((pinnum)%10)*3));
				*(gpio + GPFSEL2) |=  (1<<(((pinnum)%10)*3));
				break;
			default:
				break;
		}
	}
	else
	{
		switch(pinnum/10) {	
			case 0:
				*(gpio + GPFSEL0) &= ~(7<<(((pinnum)%10)*3));
				break;
			case 1:
				*(gpio + GPFSEL1) &= ~(7<<(((pinnum)%10)*3));
				break;
			case 2:
				*(gpio + GPFSEL2) &= ~(7<<(((pinnum)%10)*3));
				break;
			default:
				break;
		}
	}
}

/*
 * gpio_readPin() - reads the state of a GPIO pin and returns its value
 * 
 * @param pinnum  the pin number of the GPIO to read
 *
 * @return pin value. Either 1 (HIGH) if pin state is high or 0 (LOW) if pin is low
 */
unsigned int gpio_readPin(unsigned int pinnum)
{
	unsigned int retVal = 0;
	
	if ((*(gpio + GPFLEV0) & (1 << pinnum)) != 0)
		retVal = 1;
	
	return retVal;
}

/*
 * gpio_writePin() - sets (to 1) or clears (to 0) the state of an
 * output GPIO. This function has no effect on input GPIOs.
 * For faster output GPIO pin setting/clearing..use inline functions
 * 'writePinHigh()' & 'writePinLow()' defined in the header file 
 * 
 * @param pinnum GPIO number as per RPI and BCM2835 standard definition
 * @param pinstate value to write to output pin... either HIGH or LOW
 */
void gpio_writePin(unsigned int pinnum, const unsigned int pinstate)
{
	if(pinstate == HIGH)
		*(gpio + GPFSET0) = (1 << pinnum);
	else
		*(gpio + GPFCLR0) = (1 << pinnum);
}

void gpio_init_jtag()
{
	/* Initialize the JTAG input and output pins */
	gpio_setPinDir(TCK, OUTPUT);
	gpio_setPinDir(TDI, OUTPUT);
	gpio_setPinDir(TMS, OUTPUT);
	gpio_setPinDir(TDO, INPUT);
	printf("[RPi] Using the following GPIO pins for JTAG programming:\n");
	printf("[RPi]   TCK on BCM GPIO %d P1 pin %d\n", TCK, TCK_P1PIN);
	printf("[RPi]   TDI on BCM GPIO %d P1 pin %d\n", TDI, TDI_P1PIN);
	printf("[RPi]   TDO on BCM GPIO %d P1 pin %d\n", TDO, TDO_P1PIN);
	printf("[RPi]   TMS on BCM GPIO %d P1 pin %d\n", TMS, TMS_P1PIN);
}

void gpio_close_jtag()
{
	/* Put the used I/O pins back in a safe state */
	gpio_writePin(TCK, LOW);
	gpio_writePin(TDI, LOW);
	gpio_writePin(TMS, LOW);
	gpio_setPinDir(TCK, INPUT);
	gpio_setPinDir(TDI, INPUT);
	gpio_setPinDir(TMS, INPUT);
	gpio_setPinDir(TDO, INPUT);
}

void gpio_set_tdi()
{
	gpio_writePin(TDI, HIGH);
}

void gpio_clear_tdi()
{
	gpio_writePin(TDI, LOW);
}

void gpio_set_tms()
{
	gpio_writePin(TMS, HIGH);
}

void gpio_clear_tms()
{
	gpio_writePin(TMS, LOW);
}

void gpio_set_tck()
{
	gpio_writePin(TCK, HIGH);
}

void gpio_clear_tck()
{
	gpio_writePin(TCK, LOW);
}

unsigned int gpio_get_tdo()
{
	return gpio_readPin(TDO);
}

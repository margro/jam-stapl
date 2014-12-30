/**
 * @file	jamgpio.h
 * @brief	Raspberry Pi GPIO functions for JTAG programming
 */

// two possible states for pin direction
#define INPUT  0
#define OUTPUT 1
// two possible states for output pins
#define LOW  0
#define HIGH 1

/**
 * gpio_init() - Open a connection to the GPIO memory mapped I/O
 */
void gpio_init();

/**
 * gpio_exit() - Close the connection to the memory mapped I/O
 */
void gpio_exit();

/**
 * gpio_setPinDir() - sets the direction of a pin to either input or 
 * output
 * 
 * @param pinnum  GPIO pin number as per the RPI's  BCM2835's standard definition
 * @param dir     pin direction can be INPUT for input or OUTPUT for output
 */
void gpio_setPinDir(unsigned int pinnum, const unsigned int dir);

/**
 * gpio_readPin() - reads the state of a GPIO pin and returns its value
 * 
 * @param pinnum  the pin number of the GPIO to read
 *
 * @return pin value. Either 1 (HIGH) if pin state is high or 0 (LOW) if pin is low
 */
unsigned int gpio_readPin(unsigned int pinnum);

/**
 * gpio_writePin() - sets (to 1) or clears (to 0) the state of an
 * output GPIO. This function has no effect on input GPIOs.
 * For faster output GPIO pin setting/clearing..use inline functions
 * 'writePinHigh()' & 'writePinLow()' defined in the header file 
 * 
 * @param pinnum GPIO number as per RPI and BCM2835 standard definition
 * @param pinstate value to write to output pin... either HIGH or LOW
 */
void gpio_writePin(unsigned int pinnum, const unsigned int pinstate);

/**
 * gpio_init_jtag() - Initialize the JTAG input and output pins
 */
void gpio_init_jtag();

/**
 * gpio_close_jtag() - Puts the JTAG input and output pins back in a safe state (input)
 */
void gpio_close_jtag();

void gpio_set_tdi();
void gpio_clear_tdi();
void gpio_set_tms();
void gpio_clear_tms();
void gpio_set_tck();
void gpio_clear_tck();
unsigned int gpio_get_tdo();

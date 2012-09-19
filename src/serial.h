/* 
 * serial.h
 * Simple serial I/O for AVR - uses USART 0
 * void init_usart(usigned int baudrate) : Initializes stdio to USART0, 
 * so that standard I/O routines can be used, such as getchar(), putchar(), etc. 
 */

#ifndef SERIAL_H
#define SERIAL_H

// Constant Definitions
// --------------------
#define LF '\n'           /* line feed */
#define CR '\r'           /* carriage return */

// Note: F_CPU is defined on the command line for the compiler
#define UBRR_V(Baud)	((F_CPU/16/Baud) - 1)

// values for baud registers, based on clock frequency.
#define Baud115200 (UBRR_V(115200))
#define Baud57600  (UBRR_V(57600))
#define Baud38400  (UBRR_V(38400))
#define Baud19200  (UBRR_V(19200))
#define Baud9600   (UBRR_V(9600))

// Function Prototypes
// -------------------
void init_usart(unsigned int baudrate);

#endif /* SERIAL_H */

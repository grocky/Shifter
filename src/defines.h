/** @file defines.h
 * @brief This file defines the pin mappings and critical definitions
 *
 * @author  Rocky Gray Jr.
 *
 * @date    3/27/2012
 */
#ifndef DEFINES_H
#define DEFINES_H 1

#include <avr/io.h>

//#define F_CPU 16000000L
///Uncomment #DEBUG to print out system information.
//#define DEBUG 1
///Uncomment #SIMULATE in order to use the rpm regulator simulation
//#define SIMULATE 1

/** @name User Input Defines */
//@{
#define BTN_IP_DDR      DDRD    ///<Button input DDR
#define BTN_IP_PORT     PORTD   ///<Button input Port
#define BTN_IP_PIN      PIND    ///<Button input Port Pins
#define USHIFT_PIN      PD4     ///<Upshift pin\n Connect to Digital Pin 4
#define DSHIFT_PIN      PD5     ///<Downshift pin\n Connect to Digital Pin 5
#define SEMIAUTO_PIN    PD6     ///<Mode pin\n Connect to Digital Pin 6
#define AUTOMATIC_PIN   PD7     ///<Mode pin\n Connect to Digital Pin 7
#define RELEASED        0       ///<Button is released
#define PRESSED         1       ///<Button is pressed
#define DB_DELAY        5       ///<Debounce delay (ms)
#define ADC_DDR         DDRC    ///<ADC DDR
#define ADC_PORT        PORTC   ///<ADC Port
#define ADC_PIN         PINC    ///<ADC Port Pins
#define GAS_PEDAL       PC0     ///<Gas pedal input pin
//@}

/** @name Solenoid Defines */
//@{
#define SOLEN_OP_DDR    DDRB    ///<Solenoid output DDR
#define SOLEN_OP_PORT   PORTB   ///<Solenoid output Port
#define SOLEN_OP_PIN    PINB    ///<Solenoid output Port Pins
#define SOLEN_UP        PB1     ///<Solenoid Up, pulls solenoid in
#define SOLEN_DN        PB2     ///<Solenoid Down, pushes solenoid out
#define SOLEN_DLY       25      ///<Amount of time to hold Solenoid
//@}

/** @name ECU Defines */
//@{
#define ECU_DDR         DDRD    ///<Tachometer input DDR 
#define ECU_PORT        PORTD   ///<Tachometer input Port
#define ECU_PIN         PIND    ///<Tachometer input Port Pins
#define TACH_PIN        PD2     ///<Tachometer pin\n Connect to Digital Pin 2
#define IGNITION_INT    PD3     ///<Ignition interrupt pin\n Connect to Digital Pin 3
#define IGNITION_DLY    10      ///<Ignition kill duration
#define PULSE_ROT       2       ///<Number of pulses per rotation
#define RPM_HIST_LEN    10      ///<Length of RPM history
#define RPM_MAX         5200    ///<Maximum rpm for down shifting
//@}

/** @name Timer Defines */
//@{
#define TIMER0_FREQ     1000    ///<Button pin check frequency (Hz).
#define PRESCALER0      64      ///<Prescaler needed for Timer0.
#define TIMER1_FREQ     1       ///<RPM sample frequency (Hz).
#define PRESCALER1      256     ///<Prescaler needed for Timer1.
//@}

/** @name LED Defines */
//@{
#define BOARD_DDR       DDRB    ///<Board light DDR
#define BOARD_PORT      PORTB   ///<Board light Port
#define BOARD_PIN       PINB    ///<Board light Pin
#define ROLL_DELAY      300     ///<Roll time (ms)
#define BOARD_LIGHT     PB5     ///<On board light
//@}

/** @name Gear Defines */
//@{
#define MAX_GEARS       5       ///<Maximum number of gears
#endif /* DEFINES_H */

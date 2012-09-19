/**
 *  @file SAE_AutoShifter.h
 *  @brief This header file contains constant definitions and function 
 *  prototypes.
 *
 *  This project uses the Arduino Duemilanove board which uses the ATMEGA 328P 
 *  microcontroller with a 16MHz crystal oscillator. Pin mappings described
 *  below should only be used for this board or ones with the same pin 
 *  mappings. You can change the Port and Pin definitions in the 
 *  @c defines.c file.
 *
 *  @see defines.c
 *
 *  @author  Rocky Gray Jr.
 *
 *  @date    3/7/2012
 */
#ifndef SAE_AUTOSHIFTER_H
#define SAE_AUTOSHIFTER_H 1

#include <stdio.h>
#include <stdint.h>
#include <avr/interrupt.h>
#include "defines.h"
#include "delay_rg.h"
#include "usr_button.h"
#include "tachometer.h"
#include "gear.h"

#ifdef DEBUG
uint8_t canPrint = 0;   ///<DEBUG variable - print flag
#endif

typedef struct shifter Shifter;

Shifter *shifter_singleton();

void shifter_init(Shifter *shifter, Gear *g);

/** @defgroup simu Simulator
 *  Variables needed for the simulator
 *  @{
 */
#ifdef SIMULATE
uint8_t throttle_pos;      ///<Throttle position updated by ADC
#endif
//@}

void t_set_rpms(Shifter *shifter, uint16_t x);

void t_set_ave(Shifter *shifter, uint16_t x);

uint16_t g_upper(Shifter *shifter);

uint16_t g_lower(Shifter *shifter);

Gear *g_prev(Shifter *shifter);

Gear *g_next(Shifter *shifter);

Usr_Btns *down_btn(Shifter *shifter);

Usr_Btns *up_btn(Shifter *shifter);
/**
 * shift()
 * Shift gear in the desired @c direction
 *
 * This function actuates the solenoid in the desired direction while sending
 * a signal to the ECU to retard ignition. 
 *
 * If #SIMULATE is defined, then the simulator will decrease and increase the
 * rpms upon upshifting and downshifting, respectively. This emulates the 
 * sharp change in rpms when the engine changes gears.
 *
 * @param direction   The direction to shift
 */
void shift(uint8_t direction, Shifter *s);
//@}

/** 
 *  @brief  Update the button states.
 *
 *  Fires at #TIMER0_FREQ Hz.
 */
ISR(TIMER0_COMPA_vect);

/** 
 *  @brief  Tachometer sample time. 
 *
 *  Fires at #TIMER1_FREQ Hz. (1ms) 
 *  @par Converts the number of pulses counted from the external
 *  interrupt into a value that represents the instantaneous rpms of
 *  the engine. It also places the current rpms into 
 *  the history of rpms.
 */
ISR(TIMER1_COMPA_vect);

#ifndef SIMULATE
/**
 *  @brief  Count pulses from the tachometer.
 *  Fires at the rising edge of signal
 *  @par Counts the pulses from the tachometer until TIMER1 fires.
 */
ISR(INT0_vect);
#else
/**
 * @ingroup simu
 * @brief Converts position of external pedal to the change of rpms.
 *
 * This function converts the position of an external electric pedal using
 * the ADC. It uses the position of the pedal to describe the derivative of
 * the engine's rpms over time.
 *
 * @note The 5V electric pedal we used did not output a full 5V range. So the
 * ADC scale used in this function represents the .8V - 4.2V range provided by
 * the pedal. 
 */
ISR(ADC_vect);
#endif  /* SIMULATE */
#endif  /* SAE_AUTOSHIFTER_H */

/**
 * @file delay_rg.h
 * @brief This file defines delay functions that extend the usefullness of
 * the avr-gcc delay functions.
 * 
 * @author  Rocky Gray Jr.
 * 
 * @date   2/6/2012
 */
#ifndef _DELAY_RG_H
#define _DELAY_RG_H 1

#include <util/delay.h>
#define DLAY_MAX_MS (262.14/F_CPU)-1
#define DLAY_MAX_US (768/F_CPU)-1

/**
 * @brief Delays for @c ms milliseconds.
 *
 * Allows for longer delays. The longest delay that _delay_ms() can provide
 * is 262.14ms / F_CPU(in MHz). 
 *
 * @param   ms  length of delay in miliseconds
 * @return  none
 */
void delay_ms(uint16_t ms);

/**
 * @brief Delays for @c us microseconds.
 *
 * Allows for longer delays. The longest delay that _delay_us() can provide
 * is 768us / F_CPU(in MHz).
 *
 * @param   us  length of delay in microseconds
 * @return  none
 */
void delay_us(uint16_t us);

#endif /* _DELAY_RG_H */

/**
 * @file delay_rg.c
 * @brief This file defines delay functions that extend the usefullness of
 * the avr-gcc delay functions.
 * 
 * @author  Rocky Gray Jr.
 * 
 * @date   2/6/2012
 */
#include "delay_rg.h"

void delay_ms(uint16_t ms)
{
    if(ms > DLAY_MAX_MS)
    {
        while(ms)
        {
            _delay_ms(1);
            --ms;
        }
    }else
        _delay_ms(ms);
}

void delay_us(uint16_t us)
{
    if(us > DLAY_MAX_US)
    {
        while(us)
        {
            _delay_us(1);
            --us;
        }
    }else
        _delay_us(us);
}

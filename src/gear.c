/**
 *  @file gear.c
 *  @brief This file defines the gear object and related functions.
 *
 *  @author  Rocky Gray Jr.
 *
 *  @date    4/21/2012
 */
#include "gear.h"

//Gear constructor
void Gear_Construct(Gear *this, uint8_t num, uint16_t lower,
                        uint16_t upper, uint16_t cruise, Gear *p,Gear *n)
{
    this->g_num = num;
    this->lowerB = lower;
    this->upperB = upper;
    this->cruise = cruise;
    this->prev = p;
    this->next = n;
}

//Set increase array
void Gear_IncreaseLevels(Gear *this, uint16_t levels[])
{
    for(int i=0; i<MAX_GEARS;++i)
        this->increase[i] = levels[i];
}

//Set decrease array
void Gear_DecreaseLevels(Gear *this, uint16_t levels[])
{
    for(int i=1; i<MAX_GEARS;++i)
        this->decrease[i] = levels[i];
}

//Return gear number
uint8_t gear_num(Gear *gear_)
{
    return gear_->g_num;
}

//Return upper range
uint16_t gear_upper(Gear *gear_)
{
    return gear_->upperB;
}

//Return lower range
uint16_t gear_lower(Gear *gear_)
{
    return gear_->lowerB;
}

Gear *prev(Gear *gear_)
{
    return gear_->prev;
}

Gear *next(Gear *gear_)
{
    return gear_->next;
}

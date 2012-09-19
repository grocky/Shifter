/**
 *  @file
 *  @brief This header file contains constant definitions and function 
 *  prototypes.
 *
 *  This project uses the Arduino Uno board. It uses the ATMEGA 328P 
 *  microcontroller with a 16MHz crystal oscillator. Pin mappings described
 *  below should only be used for this board or ones with the same pin 
 *  mappings
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

uint8_t cur_adc;
uint8_t canPrint = 0;   ///<DEBUG variable - print flag
///Mode of the system
enum Mode {manual, semi_man, automated}mode;///< Mode switch

/** @defgroup tacho Tachometer
 *  Holds the tach pulses and rpm conversion.
 *  @c rpms update at #TIMER1_FREQ
 *  @{
 */
typedef struct
{
    uint16_t pulse;
    uint16_t rpms;
    uint8_t  index;
    uint16_t rpms_hist[RPM_HIST_LEN];
    uint16_t ave;
} Tach;

Tach tach;  ///<Tachometer object

/**
 * update_ave_rpms()
 * Updates the Tach rpm variable
 */
inline void update_ave_rpms()
{
    uint32_t sum = 0;

    for(int i = 0; i < RPM_HIST_LEN; ++i)
        sum += tach.rpms_hist[i];

    tach.ave = sum/RPM_HIST_LEN;
}

/**
 * average_rpms()
 * Returns the Tach's average rpm variable
 */
inline uint16_t average_rpms()
{
    return tach.ave;
}
inline uint16_t cur_rpms()
{
    return tach.rpms;
}

//@}

/** @defgroup usrBtns User Buttons
 *  The states of the buttons, debounce values, and related functions.
 *  @{
 */
typedef struct
{
    uint8_t state;  ///< Button state. Either #PRESSED or #RELEASED
    uint16_t count; ///< Button Press count.
} Usr_Btns;

Usr_Btns up_shift,  ///< Upshift button
         dn_shift;  ///< Downshift button

/**
 * btn_state()
 *
 * @return the @c state of @c u_btn
 */
inline uint8_t btn_state(Usr_Btns *u_btn)
{
    return u_btn->state;
}
/**
 * btn_cout()
 *
 * @return the press @c count of @c u_btn
 */
inline uint16_t btn_count(Usr_Btns *u_btn)
{
    return u_btn->count;
}

/**
 * onRelease()
 * The on-release handler for @c btn
 *
 * @return  False if buton is not pressed
 * @return  True when the button is pressed and released
 *
 * @var btn The button to handle
 */
inline uint8_t onRelease(Usr_Btns *btn)
{
    if(btn->state == RELEASED)
        return 0;
    if(btn->state == PRESSED)
        while(btn->state == PRESSED)
            delay_us(1);
    
    return 1;
}

//@}
/** defgroup gears Gears
 *  Defines the gear structure and related functions.
 *  @{
 */
typedef struct Gear{
    uint8_t     g_num;     ///< Number of the Gear
    uint16_t    lowerB;    ///< Lower bound of Gear
    uint16_t    upperB;    ///< Upper bound of Gear
    uint16_t    cruise;    ///< Cruise RPM level
    struct Gear *prev;     ///< Previous gear
    struct Gear *next;     ///< Next gear
    uint16_t    increase[MAX_GEARS];///< rpm increase levels
    uint16_t    decrease[MAX_GEARS];///< rpm decrease levels
}Gear;

Gear *gear_;               ///<Current gear
uint8_t throttle_pos;
/**
 * Gear_Const()
 * Constructor for Gear Object
 *
 * @var this    the Gear to construct
 * @var num     Gear number
 * @var lower   Lower bound for Gear
 * @var upper   Upper bound for Gear
 * @var p       Previous Gear
 * @var n       Next Gear
 */
inline void Gear_Construct(Gear *this, uint8_t num, uint16_t lower,
                        uint16_t upper, uint16_t cruise, Gear *p,Gear *n)
{
    this->g_num = num;
    this->lowerB = lower;
    this->upperB = upper;
    this->cruise = cruise;
    this->prev = p;
    this->next = n;
}

inline void Gear_IncreaseLevels(Gear *this, uint16_t levels[])
{
    for(int i=0; i<MAX_GEARS;++i)
        this->increase[i] = levels[i];
}

inline void Gear_DecreaseLevels(Gear *this, uint16_t levels[])
{
    for(int i=1; i<MAX_GEARS;++i)
        this->decrease[i] = levels[i];
}
/**
 * gear_num()
 * 
 * @return the number of the current gear
 */
inline uint8_t gear_num()
{
    return gear_->g_num;
}

/**
 * gear_upper()
 *
 * @return The upper bound of the current gear
 */
inline uint16_t gear_upper()
{
    return gear_->upperB;
}

/**
 * gear_lower()
 *
 * @return The lower bound of the current gear
 */
inline uint16_t gear_lower()
{
    return gear_->lowerB;
}

/**
 * shift()
 * Shift gear in the desired @c direction
 *
 * @var direction   The direction to shift
 */
inline void shift(uint8_t direction)
{
    ECU_PORT |= _BV(IGNITION_INT);
    delay_ms(IGNITION_DLY);
    SOLEN_OP_PORT |= _BV(direction);
    delay_ms(SOLEN_DLY);
    SOLEN_OP_PORT &= ~_BV(direction);
    delay_ms(IGNITION_DLY);
    ECU_PORT &= ~_BV(IGNITION_INT);

    cli();
//#ifdef SIMULATE
    int next_rpms;
    switch(direction)
    {
        case SOLEN_UP:
            switch(gear_->g_num)
            {
                case 1:
                    tach.rpms -= 60;
                    break;
                case 2:
                    tach.rpms -= 60;
                    break;
                case 3:
                    tach.rpms -= 60;
                    break;
                case 4:
                    tach.rpms -= 60;
                    break;
                default:
                    //Shifting up in 5th gear not allowed...
                    break;
            }
        case SOLEN_DN:
            switch(gear_->g_num)
            {
                case 1:
                case 2:
                    tach.rpms += 60;
                    break;
                case 3:
                    tach.rpms += 60;
                    break;
                case 4:
                    tach.rpms += 60;
                    break;
                case 5:
                    tach.rpms += 60;
                    break;
                default:
                    break;
            }
    }
    sei();
//#endif  /* SIMULATE */
}
//@}

/** 
 *  @brief  Update the button states.
 *
 *  Fires at #TIMER0_FREQ Hz.
 */
ISR(TIMER0_COMPA_vect);

//#ifdef SIMULATE
/**
 * @brief Converts position of external pedal to the change of rpms.
 *
 * This function converts the position of an external electric pedal using
 * the ADC. It uses the position of the pedal to describe the derrivative of
 * the engine's rpms over time.
 */
ISR(ADC_vect);
//#else
/**
 *  @brief  Count pulses from the tachometer.
 *  Fires at the rising edge of signal
 *  @par Counts the pulses from the tachometer until TIMER1 fires.
 */
ISR(INT0_vect);
//#endif  /* SIMULATE */

/** 
 *  @brief  Tachometer sample time. 
 *
 *  Fires at #TIMER1_FREQ Hz. (1ms) 
 *  @par Converts the number of pulses counted
 *  from the external interrupt into a value that represents the 
 *  instantaneous rpms of the engine. It also places the current rpms into 
 *  the history of rpms.
 */
ISR(TIMER1_COMPA_vect);

#endif  /* SAE_AUTOSHIFTER_H */

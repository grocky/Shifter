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
void update_ave_rpms();

/**
 * average_rpms()
 * Returns the Tach's average rpm variable
 */
uint16_t average_rpms();

uint16_t cur_rpms();

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
uint8_t btn_state(Usr_Btns *u_btn);

/**
 * btn_cout()
 *
 * @return the press @c count of @c u_btn
 */
uint16_t btn_count(Usr_Btns *u_btn);

/**
 * onRelease()
 * The on-release handler for @c btn
 *
 * @return  False if buton is not pressed
 * @return  True when the button is pressed and released
 *
 * @var btn The button to handle
 */
uint8_t onRelease(Usr_Btns *btn);

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
void Gear_Construct(Gear *this, uint8_t num, uint16_t lower,
                        uint16_t upper, uint16_t cruise, Gear *p,Gear *n);


void Gear_IncreaseLevels(Gear *this, uint16_t levels[]);

void Gear_DecreaseLevels(Gear *this, uint16_t levels[]);

/**
 * gear_num()
 * 
 * @return the number of the current gear
 */
uint8_t gear_num();

/**
 * gear_upper()
 *
 * @return The upper bound of the current gear
 */
uint16_t gear_upper();


/**
 * gear_lower()
 *
 * @return The lower bound of the current gear
 */
uint16_t gear_lower();

/**
 * shift()
 * Shift gear in the desired @c direction
 *
 * @var direction   The direction to shift
 */
void shift(uint8_t direction);

//@}
#endif  /* SAE_AUTOSHIFTER_H */

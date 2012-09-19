/**
 *  @file tachometer.h
 *  @brief This file defines the tachometer object and related functions.
 *
 *  @author  Rocky Gray Jr.
 *
 *  @date    4/21/2012
 */
#ifndef TACHOMETER_H
#define TACHOMETER_H
/** @defgroup tacho Tachometer
 *  Holds the tach pulses and rpm conversion.
 *  @c rpms update at #TIMER1_FREQ
 *  @{
 */

typedef struct tachometer Tach; ///<Tachometer object

/**
 * tachometer_singleton()
 * @return the pointer to the tachometer singleton
 */
Tach * tachometer_singleton();

/**
 * set_rpms()
 * Sets the Tach rpm variable
 */
void set_rpms(uint16_t);

/**
 * set_ave_rpms()
 * Sets the Tach ave variable
 */
void set_ave_rpms(uint16_t);

/**
 * update_ave_rpms()
 * Updates the Tach rpm variable
 */
void update_ave_rpms();

/**
 * cur_rpms()
 * @return current rpms of the tachometer
 */
uint16_t get_rpms();

/**
 * ave_rpms()
 * @return current rpms of the tachometer
 */
uint16_t get_ave_rpms();
//@}
#endif

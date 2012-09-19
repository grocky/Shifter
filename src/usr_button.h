/**
 *  @file usr_button.h
 *  @brief This file defines the usr_button object and related functions.
 *
 *  @author  Rocky Gray Jr.
 *
 *  @date    4/21/2012
 */
#ifndef USR_BUTTON_H
#define USR_BUTTON_H

/** @defgroup usrBtns User Buttons
 *  The states of the buttons, debounce values, and related functions.
 *  @{
 */
struct usr_btns
{
    uint8_t state;  ///< Button state. Either #PRESSED or #RELEASED
};

typedef struct usr_btns Usr_Btns;

void btn_init(Usr_Btns *u_btn);
/**
 * btn_state()
 *
 * @param u_btn     a pointer to a Button.
 * @return the @c state of @c u_btn
 */
uint8_t btn_state(Usr_Btns *u_btn);

/**
 * onRelease()
 * The on-release handler for @c btn
 *
 * @return  False if buton is not pressed
 * @return  True when the button is pressed and released
 *
 * @param *btn The button to handle
 */
uint8_t onRelease(Usr_Btns *btn);
//@}
#endif

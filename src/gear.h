/**
 *  @file gear.h
 *  @brief This file defines the gear object and related functions.
 *  The user will be able to creat Gear objects and dine each gear's profile
 *
 *  @author  Rocky Gray Jr.
 *
 *  @date    4/21/2012
 */
#ifndef GEAR_H
#define GEAR_H
/** @defgroup gears Gear
 *  Defines the gear structure and related functions.
 *  @{
 */

struct gear{
    uint8_t     g_num;              ///< Number of the Gear
    uint16_t    lowerB;             ///< Lower bound of Gear
    uint16_t    upperB;             ///< Upper bound of Gear
    uint16_t    cruise;             ///< Cruise RPM level
    struct gear *prev;              ///< Previous gear
    struct gear *next;              ///< Next gear
    uint16_t    increase[MAX_GEARS];///< rpm increase levels
    uint16_t    decrease[MAX_GEARS];///< rpm decrease levels
};

typedef struct gear Gear;

/**
 * Gear_Const()
 * Constructor for Gear Object
 *
 * @param this    the Gear to construct
 * @param num     Gear number
 * @param lower   Lower bound for Gear
 * @param upper   Upper bound for Gear
 * @param cruise  The rate to which the rpms converge
 * @param p       Previous Gear
 * @param n       Next Gear
 */
void Gear_Construct(Gear *this, uint8_t num, uint16_t lower,
                        uint16_t upper, uint16_t cruise, Gear *p,Gear *n);

/**
 * gear_num()
 * 
 * @return the number of the current gear
 */
uint8_t gear_num(Gear *gear_);

/**
 * gear_upper()
 *
 * @return The upper bound of the current gear
 */
uint16_t gear_upper(Gear *gear_);
/**
 * gear_lower()
 *
 * @return The lower bound of the current gear
 */
uint16_t gear_lower(Gear *gear_);

///@ingroup simu 
//@{
/**
 * Gear_IncreaseLevels()
 * Initializes the @c increase array
 *
 * @param this      a pointer to a Gear
 * @param levels[]  a pointer to an array
 */
void Gear_IncreaseLevels(Gear *this, uint16_t levels[]);

/**
 * Gear_DecreaseLevels()
 * Initializes the @c decrease array
 * 
 * @param this      a pointer to a Gear
 * @param levels[]  a pointer to an array 
 */
void Gear_DecreaseLevels(Gear *this, uint16_t levels[]);

Gear *prev(Gear *gear_);
Gear *next(Gear *gear_);
//@}
//@}
#endif

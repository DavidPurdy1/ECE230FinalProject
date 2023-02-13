/*! \file */
/*!
 * stepperMotor.h
 *
 * Description: Stepper motor ULN2003 driver for MSP432P4111 Launchpad.
 *              Assumes SMCLK configured with 48MHz HFXT as source.
 *              Uses Timer_A3 and P2.7, P2.6, P2.5, P2.4
 *
 *  Created on:
 *      Author:
 */

#ifndef STEPPERMOTOR_H_
#define STEPPERMOTOR_H_

//*****************************************************************************
//
// If building with a C++ compiler, make all of the definitions in this header
// have a C binding.
//
//*****************************************************************************
#ifdef __cplusplus
extern "C"
{
#endif

#include "msp.h"
#include <stdbool.h>

#define STEPPER_PORT                    P2
#define STEPPER_MASK                    (0x00F0)
#define STEPPER_IN1                     (0x0080)
#define STEPPER_IN2                     (0x0040)
#define STEPPER_IN3                     (0x0020)
#define STEPPER_IN4                     (0x0010)

// set initial step timer period for 10 RPM (based on 4MHz clock rate)
#define INIT_PERIOD                     0x2DC7
#define STEP_SEQ_CNT                    8
#define MAX_STEPPER_PERIOD              0x2DC7 / 2
#define MIN_STEPPER_PERIOD              MAX_STEPPER_PERIOD * 0xA

/*!
 * \brief This function configures pins and timer for stepper motor driver
 *
 * This function configures P2.4, P2.5, P2.6, and P2.6 as output pins
 *  for the ULN2003 stepper driver IN port, and initializes Timer_A3 to
 *  increment step position with compare interrupt
 *
 * Modified bits 4 to 7 of \b P2DIR register and \b P2SEL registers.
 * Modified \b TA3CTL register and \b TA3CCTL0 registers.
 *
 * \return None
 */
extern void initStepperMotor(void);


/*!
 * \brief This starts stepper motor rotation by turning on Timer_A3
 *
 * This function starts stepper motor rotation by turning on Timer_A3.
 * Assumes stepper motor has already been configured by initStepperMotor().
 *
 * Modified \b TA3CTL register.
 *
 * \return None
 */
void enableStepperMotor(void);


/*!
 * \brief This stops stepper motor rotation by turning off timer
 *
 * This function stops stepper motor rotation by turning off Timer_A3.
 * Stepper motor is still configured after calling this function.
 *
 * Modified \b TA3CTL register.
 *
 * \return None
 */
void disableStepperMotor(void);


/*!
 * \brief This increments step clockwise
 *
 * This function increments to next clockwise step position
 *
 * Modified bit 4, 5, 6, and 7 of \b P2OUT register.
 *
 * \return None
 */
extern void stepClockwise(void);


/*!
 * \brief This increments step counter-clockwise
 *
 * This function increments to next counter-clockwise step position
 *
 * Modified bit 4, 5, 6, and 7 of \b P2OUT register.
 *
 * \return None
 */
extern void stepCounterClockwise(void);


/*!
 * \brief Sets the period for the stepper motor
 *
 * This function sets the period for the stepper motor to rotate
 *
 * Modified TACCR[0]
 *
 * \return None
 */
extern void setStepperPeriod(uint16_t period);


extern void setDirection(bool isClockwise);


//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif /* STEPPERMOTOR_H_ */
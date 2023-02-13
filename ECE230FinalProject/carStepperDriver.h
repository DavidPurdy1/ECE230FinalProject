/*
 * carStepperDriver.h
 *
 *  Created on: Feb 13, 2023
 *      Author: purdydj
 */

#ifndef CARSTEPPERDRIVER_H_
#define CARSTEPPERDRIVER_H_

#include "stepperMotor.h"

// Start the stepper motor definitions here

// stepper motor 1
#define STEPPER_PORT                    P2
#define STEPPER_MASK                    (0x00F0)
#define STEPPER_IN1                     (0x0080)
#define STEPPER_IN2                     (0x0040)
#define STEPPER_IN3                     (0x0020)
#define STEPPER_IN4                     (0x0010)

// stepper motor 2
#define STEPPER2_PORT                    P2
#define STEPPER2_MASK                    (0x00F0)
#define STEPPER2_IN1                     (0x0080)
#define STEPPER2_IN2                     (0x0040)
#define STEPPER2_IN3                     (0x0020)
#define STEPPER2_IN4                     (0x0010)

// TODO: I think the best way right now to handle this is to have the car driver to have structs 
// and then we pass in the structs to init, disable etc..

void initCarSteppers(); 

// Moves the steppers by the number of steps and the direction
void moveStepper1(int steps, int direction);
void moveStepper2(int steps, int direction);

// Handle the conversion of the gyro data to the stepper motor data, 
// it then calls both the moveStepper1 and moveStepper2 functions to move the steppers
// the steppers where they need and sets the direction of the steppers or the
// period of the steppers.
void handleConvertGyroToStepper(int gyroData);

#endif /* CARSTEPPERDRIVER_H_ */

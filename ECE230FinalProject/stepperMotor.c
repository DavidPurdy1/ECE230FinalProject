
#include "stepperMotor.h"
#include "msp.h"

/* Global Variables  */
// fill in array with 8-bit binary sequence for half step
const uint8_t stepperSequence[STEP_SEQ_CNT] = {0b0001, 0b0011, 0b0010, 0b0110, 0b0100, 0b1100, 0b1000, 0b1001};
uint16_t stepPeriod = MAX_STEPPER_PERIOD;
uint8_t currentStep = 0;
static bool clockwise = true;
static bool doStep = true;

void initStepperMotor(void) {
    // set stepper port pins as GPIO outputs
    STEPPER_PORT->SEL0 = (STEPPER_PORT->SEL0) & ~STEPPER_MASK;
    STEPPER_PORT->SEL1 = (STEPPER_PORT->SEL1) & ~STEPPER_MASK;
    STEPPER_PORT->DIR = (STEPPER_PORT->DIR) | STEPPER_MASK;

    // initialize stepper outputs to LOW
    STEPPER_PORT->OUT = (STEPPER_PORT->OUT) & ~STEPPER_MASK;

    /* Configure Timer_A3 and CCR0 */
    // Set period of Timer_A3 in CCR0 register for Up Mode
    TIMER_A3->CCR[0] = stepPeriod;
    // configure CCR0 for Compare mode with interrupt enabled (no output mode - 0)

    TIMER_A3->CCTL[0] |= TIMER_A_CCTLN_CCIE; // interrupt enabled
    TIMER_A3->CTL |= TIMER_A_CCTLN_OUTMOD_0; // no output mode

    // Configure Timer_A3 in Stop Mode, with source SMCLK, prescale 12:1, and
    //  interrupt disabled  -  tick rate will be 4MHz (for SMCLK = 48MHz)
    // configure Timer_A3 (requires setting control AND expansion register)
    TIMER_A3->CTL |= TIMER_A_CTL_MC_0; // stop mode

    TIMER_A3->CTL &= ~TIMER_A_CTL_IE;

    TIMER_A3->CTL |= TIMER_A_CTL_SSEL__SMCLK;

    TIMER_A3->EX0 |= TIMER_A_EX0_TAIDEX_2; // divide by 3

    TIMER_A3->CTL |= TIMER_A_CTL_ID_2;

    TIMER_A3->CTL |= TIMER_A_CTL_CLR;
    /* Configure global interrupts and NVIC */
    // Enable TA3CCR0 compare interrupt by setting IRQ bit in NVIC ISER0 register
    // enable interrupt by setting IRQ bit in NVIC ISER0 register
    NVIC->ISER[0] = 1 << TA3_0_IRQn;

    __enable_irq();                             // Enable global interrupt
}

void setStepperPeriod(uint16_t period){
    stepPeriod = period;
    TIMER_A3->CCR[0] = stepPeriod;
}

void setDirection(bool isClockwise){
    clockwise = isClockwise;
}

void enableStepperMotor(void) {
    // configure Timer_A3 in Up Mode (leaving remaining configuration unchanged)
    TIMER_A3->CTL |=TIMER_A_CTL_MC__UP;
    doStep = true;
}

void disableStepperMotor(void) {
    //  Configure Timer_A3 in Stop Mode (leaving remaining configuration unchanged)
    // TODO: Check that this works
    TIMER_A3->CTL |= TIMER_A_CTL_MC__STOP;
    doStep = false;
}

void stepClockwise(void) {
    currentStep = (currentStep + 1) % STEP_SEQ_CNT;  // increment to next step position
    //  update output port for current step pattern
    //  do this as a single assignment to avoid transient changes on driver signals
    STEPPER_PORT->OUT = (STEPPER_PORT->OUT & 0x0F) + (stepperSequence[currentStep] << 4);
}

void stepCounterClockwise(void) {
    //  update output port for current step pattern
    //  do this as a single assignment to avoid transient changes on driver signals
    // TODO: Check that this works
    currentStep = ((uint8_t)(currentStep - 1)) % STEP_SEQ_CNT;  // decrement to previous step position (counter-clockwise)
    STEPPER_PORT->OUT = (STEPPER_PORT->OUT & 0x0F) + (stepperSequence[currentStep] << 4);
}

// Timer A3 CCR0 interrupt service routine
void TA3_0_IRQHandler(void)
{
    // check which direction to set it to
    if(doStep){
        if(clockwise)
            stepClockwise();
        else
            stepCounterClockwise();
    }

    // clear timer compare flag in TA3CCTL0
    TIMER_A3->CCTL[0] &= ~TIMER_A_CTL_IFG;
}

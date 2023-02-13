/*
 * uart_routines2023.c
 *
 *  Created on: Jan 27, 2023
 *      Author: song
 */

#include "uart_routines2023.h"

//UART A0 IO pins
// P1.3/UCA0TXD |----> RX PC (echo)
//  P1.2/UCA0RXD |<---- TX PC
#define UARTA0port P1
#define UARTA0pins   BIT2 | BIT3

#define SYSTEMCLOCK 12000000    //Hz
#define BAUDRATE    38400   //bits per seconds (Hz)
#define ClockPrescalerValue SYSTEMCLOCK/(16*BAUDRATE)
#define FirstModulationStage 0  //12MHz, 57600 Baud
#define SecondModulationStage 73 //12MHz, 57600 Baud

//configure UART EUSCI_A0
void ConfigureUART_A0(void)
{
    /* Configure MCLK/SMCLK source to DCO, with DCO = 12MHz */
    CS->KEY = CS_KEY_VAL;                // Unlock CS module for register access
    CS->CTL0 = 0;                           // Reset tuning parameters
    CS->CTL0 = CS_CTL0_DCORSEL_3; // Set DCO to 12MHz (nominal, center of 8-16MHz range)
    CS->CTL1 = CS_CTL1_SELA_2 |             // Select ACLK = REFO
            CS_CTL1_SELS_3 |                // SMCLK = DCO
            CS_CTL1_SELM_3;                 // MCLK = DCO
    CS->KEY = 0;                      // Lock CS module from unintended accesses

    /* Configure UART pins */
    UARTA0port->SEL0 |= UARTA0pins; // set 2-UART pins as secondary function
    UARTA0port->SEL1 &= ~(UARTA0pins);

    /* Configure UART
     *  Asynchronous UART mode, 8O1 (8-bit data, even parity, 1 stop bit),
     *  LSB first by default, SMCLK clock source
     */
    EUSCI_A0->CTLW0 |= EUSCI_A_CTLW0_SWRST; // Put eUSCI in reset
    EUSCI_A0->CTLW0 = EUSCI_A_CTLW0_SWRST | // Remain eUSCI in reset
            EUSCI_A_CTLW0_UCSSEL_2 |
            EUSCI_A_CTLW0_MODE_0 |
            EUSCI_A_CTLW0_PEN;
    EUSCI_A0->CTLW0 &= ~EUSCI_A_CTLW0_MODE0;
    EUSCI_A0->CTLW0 &= ~EUSCI_A_CTLW0_SYNC;
    EUSCI_A0->CTLW0 &= ~EUSCI_A_CTLW0_SPB;
    EUSCI_A0->CTLW0 &= ~EUSCI_A_CTLW0_SEVENBIT;
    EUSCI_A0->CTLW0 &= ~EUSCI_A_CTLW0_MSB;
    EUSCI_A0->CTLW0 |= EUSCI_A_CTLW0_PAR;

    EUSCI_A0->BRW = ClockPrescalerValue;

    // TODO set clock prescaler in eUSCI_A0 baud rate control register
    EUSCI_A0->MCTLW = (SecondModulationStage << 8) + (FirstModulationStage << 4)
            + 1;    //enalble oversampling
    // TODO configure baud clock modulation in eUSCI_A0 modulation control register

    EUSCI_A0->CTLW0 &= ~EUSCI_A_CTLW0_SWRST;    // Initialize eUSCI
    EUSCI_A0->IFG &= ~EUSCI_A_IFG_RXIFG;        // Clear eUSCI RX interrupt flag
    EUSCI_A0->IE |= EUSCI_A_IE_RXIE;            // Enable USCI_A0 RX interrupt

    NVIC->ISER[0] |= (1) << EUSCIA0_IRQn;
} //end ConfigureUART_A0(void)

void SendCharArray_A0(char *Buffer)
{
    unsigned int count;
    for (count = 0; count < strlen(Buffer); count++)
    {
        // Check if the TX buffer is empty first
        while (!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG))
            ;
        EUSCI_A0->TXBUF = Buffer[count];
    }   //end for()
} // end SendCharArray(char *Buffer)

// UART interrupt service routine
void EUSCIA0_IRQHandler(void)
{
    if (EUSCI_A0->IFG & EUSCI_A_IFG_RXIFG)
    {
        // Check if the TX buffer is empty first
        while (!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));

        // Echo the received character back
        //  Note that reading RX buffer clears the flag and removes value from buffer
        EUSCI_A0->TXBUF = EUSCI_A0->RXBUF;
    }

    ReceivedChar = EUSCI_A0->RXBUF;
}

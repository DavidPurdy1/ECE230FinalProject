/*
 * uart_routines2023.h
 *
 *  Created on: Jan 27, 2023
 *      Author: song
 */
#include <msp.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "csHFXT.h"

uint16_t ReceivedChar;

void ConfigureUART_A0(void);
void SendCharArray_A0(char *Buffer);

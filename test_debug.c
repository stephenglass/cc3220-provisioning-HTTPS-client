/*
 * test_debug.c
 *
 *  Created on: Feb 28, 2019
 *      Author: sglas
 */

#include <uart_term.h>

/* Enable UART Log */
#define LOG_MESSAGE_ENABLE
#define LOG_MESSAGE UART_PRINT

void testDebugFunction(void) {
    LOG_MESSAGE("Testing this debug function!");
    return;
}


void* testDebugFunctionOS(void* pvParameters) {
    LOG_MESSAGE("Testing this OS debug function!");
    return;
}

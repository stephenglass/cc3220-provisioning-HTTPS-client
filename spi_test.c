/*
 * spi_test.c
 *
 *  Created on: Feb 28, 2019
 *      Author: Stephen Glass
 *
 *  Description: Testing SPI master device
 */

#include <Board.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <ti/drivers/SPI.h>
#include <ti/drivers/GPIO.h>
#include <uart_term.h>

/* Enable UART Log */
#define LOG_MESSAGE_ENABLE
#define LOG_MESSAGE UART_PRINT

#define MSGSIZE 64

#define AFE_CS_HIGH (1)
#define AFE_CS_LOW  (0)

SPI_Handle      spi;
SPI_Params      spiParams;
SPI_Transaction spiTransaction;
uint8_t         transmitBuffer[MSGSIZE];
uint8_t         receiveBuffer[MSGSIZE];
bool            transferOK;

void assertChipSelect(unsigned int transferState) {
    LOG_MESSAGE("Asserting CS state: %d", transferState);
    GPIO_write(Board_GPIO_AFE_CS, transferState);
    return;
}

/*
Configures SPI and tests
*/
void* testSPITask(void *pvParameters) {

    assertChipSelect(AFE_CS_LOW); // Set AFE Chip select active

    SPI_Params_init(&spiParams);  // Initialize SPI parameters
    spiParams.dataSize = 8;       // 8-bit data size
    spi = SPI_open(Board_SPI0, &spiParams);
    if (spi == NULL) {
        while (1);  // SPI_open() failed
    }
    // Fill in transmitBuffer
    spiTransaction.count = MSGSIZE;
    spiTransaction.txBuf = (void *)transmitBuffer;
    spiTransaction.rxBuf = (void *)receiveBuffer;
    transferOK = SPI_transfer(spi, &spiTransaction);
    if (!transferOK) {
        // Error in SPI or transfer already in progress.
        while (1);
    }

    assertChipSelect(AFE_CS_HIGH); // Disable AFE Chip select
    return(0);
}

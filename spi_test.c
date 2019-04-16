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

#define MSGSIZE 32

#define AFE_CS_HIGH (1)
#define AFE_CS_LOW  (0)

/* AFE Write codes */
#define AFE_MOD_REG     0x4C000000
#define AFE_PHASE_REG   0x4E000000
#define AFE_GAIN_REG    0x50000000
#define AFE_STATUS_REG  0x52000000
#define AFE_CONTROL_REG 0x54000000
#define AFE_LENGTH      32
#define AFE_DATA_LENGTH 76

/* PIN NOTES
MOSI = PIN_07
MISO = PIN_06
CLK  = PIN_05
CS   = PIN_01
CLKI = PIN_02
RESET= PIN_62*/

//SPI_Handle      spi;
//SPI_Params      spiParams;
//SPI_Transaction spiTransaction;
//uint32_t        transmitBuffer[MSGSIZE];
//uint32_t        receiveBuffer[MSGSIZE];
//bool            transferOK;

SPI_TransferMode transferMode;

typedef union {
    uint32_t word;
    struct {
        uint32_t data:24;
        uint32_t rw:1;
        uint32_t address:5;
        uint32_t a2:2;
    } bits;
} afe_word_t;

void assertChipSelect(unsigned int transferState) {
    LOG_MESSAGE("Asserting CS state: %d\r\n", transferState);
    GPIO_write(Board_GPIO_AFE_CS, transferState);
    return;
}

/*
Configures SPI and tests
*/
void* testSPITask(void *pvParameters) {
    volatile uint16_t delay = 0;
    GPIO_write(Board_GPIO_AFE_RESET, 1); // Set AFE reset HIGH (Active low)
    for(delay = 0; delay < 10000; delay++);
    assertChipSelect(AFE_CS_LOW); // Set AFE Chip select active

    SPI_Handle      spi;
    SPI_Params      spiParams;
    bool transferOK;

    /* Initialize SPI params */
    SPI_Params_init(&spiParams); // Initialize SPI parameters
    spiParams.dataSize = 32;     // 32-bit data size
    transferMode = SPI_MODE_BLOCKING; // Blocking-mode
    spiParams.transferMode = transferMode; // Transfer mode
    spi = SPI_open(Board_SPI0, &spiParams);
    if (spi == NULL) {
        while (1);  // SPI_open() failed
    }

    // Configure Registers ----------------------
    // -------------------------------------------------------------
    afe_word_t config[1];
    SPI_Transaction spiTransactionConfig;
    config[0].word = 0x5A0000 | 0x086050; //AFE_CONTROL_REG | 0x000031;
    config[0].bits.a2 = 0b01;
    spiTransactionConfig.count = 1;
    spiTransactionConfig.txBuf = config;
    spiTransactionConfig.rxBuf = 0; // no buffer because we aren't reading
    transferOK = SPI_transfer(spi, &spiTransactionConfig);
    if (!transferOK) {
        // Error in SPI or transfer already in progress.
        while (1);
    }

    assertChipSelect(AFE_CS_HIGH);
    for(delay = 0; delay < 10000; delay++);
    assertChipSelect(AFE_CS_LOW);

    config[0].word = 0x580000 | 0xB90000; //AFE_STATUS_REG | 0x1FE100;
    config[0].bits.a2 = 0b01;
    spiTransactionConfig.count = 1;
    spiTransactionConfig.txBuf = config;
    spiTransactionConfig.rxBuf = 0; // no buffer because we aren't reading
    transferOK = SPI_transfer(spi, &spiTransactionConfig);
    if (!transferOK) {
        // Error in SPI or transfer already in progress.
        while (1);
    }
    while(1) {
        assertChipSelect(AFE_CS_HIGH);
        for(delay = 0; delay < 10000; delay++);
        assertChipSelect(AFE_CS_LOW);

        // End Configure Registers ------------
        //------------------------------------------------------------

        SPI_Transaction spiTransactionVoltage;
        afe_word_t readVoltage[6];
        uint32_t readBufferVoltage[6];
        readVoltage[0].word = 0x41FFFFFF;
        readVoltage[0].bits.a2 = 0b01;
        readVoltage[0].bits.address = 0;
        readVoltage[0].bits.rw = 1;


        spiTransactionVoltage.count = 6; //6;
        spiTransactionVoltage.txBuf = readVoltage;
        spiTransactionVoltage.rxBuf = (void *)readBufferVoltage;
        transferOK = SPI_transfer(spi, &spiTransactionVoltage);
        if (!transferOK) {
            // Error in SPI or transfer already in progress.
            while (1);
        }
    }


    assertChipSelect(AFE_CS_HIGH); // Disable AFE Chip select
    return(0);
}

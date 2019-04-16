/*
 * afe_pwm.c
 *
 *  Created on: Mar 31, 2019
 *      Author: sglas
 */

#include <Board.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <ti/drivers/PWM.h>
#include <ti/drivers/GPIO.h>
#include <uart_term.h>

/* Enable UART Log */
#define LOG_MESSAGE_ENABLE
#define LOG_MESSAGE UART_PRINT

/*
Configures PWM for AFE
*/
void* configurePWM(void *pvParameters) {
    LOG_MESSAGE("Configuring PWM for AFE..\r\n");
    PWM_Handle pwm;
    PWM_Params pwmParams;
    uint32_t   dutyValue;
    // Initialize the PWM driver.
    PWM_init();
    // Initialize the PWM parameters
    PWM_Params_init(&pwmParams);
    pwmParams.idleLevel = PWM_IDLE_LOW;      // Output low when PWM is not running
    pwmParams.periodUnits = PWM_PERIOD_HZ;   // Period is in Hz
    pwmParams.periodValue = 4e6;             // 4MHz
    pwmParams.dutyUnits = PWM_DUTY_FRACTION; // Duty is in fractional percentage
    pwmParams.dutyValue = 0;                 // 0% initial duty cycle
    // Open the PWM instance
    pwm = PWM_open(Board_PWM1, &pwmParams);
    if (pwm == NULL) {
        // PWM_open() failed
        while (1);
    }
    PWM_start(pwm);                          // start PWM with 0% duty cycle
    dutyValue = (uint32_t) (((uint64_t) PWM_DUTY_FRACTION_MAX * 50) / 100);
    PWM_setDuty(pwm, dutyValue);  // set duty cycle to 50%
    LOG_MESSAGE("PWM configured 4MHz!\r\n");
    return(0);
}

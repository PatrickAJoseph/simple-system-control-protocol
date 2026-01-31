/*
 * Copyright (c) 2020, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *  ======== uart2callback.c ========
 */
#include <stdint.h>
#include <stddef.h>

/* POSIX Header files */
#include <semaphore.h>

/* Driver Header files */
#include <ti/drivers/GPIO.h>
#include <ti/drivers/UART2.h>

/* Driver configuration */
#include "ti_drivers_config.h"

#include "sscp.h"

static sem_t sem;

UART2_Handle uart;
UART2_Params uartParams;

/* Define register structures. */

struct led_register
{
    uint32_t state              : 1;         //  State of LED.
    uint32_t on                 : 1;         //  LED on command bit.
    uint32_t off                : 1;         //  LED off command bit.
    uint32_t toggle             : 1;         //  LED toggle command.
    uint32_t blink_start        : 1;         //  LED blink start command.
    uint32_t blink_stop         : 1;         //  LED blink stop command.
    uint32_t blink_count        : 8;         //  Number of times to blink the LED.
    uint32_t blink_interval_ms  : 16;        //  LED blink interval in milli-seconds.
    uint32_t reserved           : 2;         //  Reserved bits.  
};

struct led_register led0_register;
struct led_register led1_register;

/* Define callbacks for registers. */

void led0_register_callback(void* reg, int operation)
{
    struct led_register* led0;

    if( operation == SSCP_REGISTER_OPERATION_READ )
    {

    }
    else if( operation == SSCP_REGISTER_OPERATION_WRITE )
    {
        
    }
}

void led1_register_callback(void* reg, int operation)
{
    struct led_register* led0;

    if( operation == SSCP_REGISTER_OPERATION_READ )
    {

    }
    else if( operation == SSCP_REGISTER_OPERATION_WRITE )
    {
        
    }
}

/* Create a list of register defines. */

static SSCP_REGISTER_HANDLE_LIST(sscpSerialHandleList)
{
    SSCP_REGISTER_HANDLE( 0, led0_register, led0_register_callback ),
    SSCP_REGISTER_HANDLE( 0, led1_register, led1_register_callback ),
};

/* Function prototype for UART transmitter. */

int UART_send(uint8_t* data, size_t size)
{
    int ret = 0;

    ret = UART2_write(uart, data, size, NULL);

    return ( ( ret == UART2_STATUS_SUCCESS ) ? 1 : 0 );
}

/* Create SSCP handle. */

SSCP_HANDLE(sscpSerialHandle, SSCP_REGISTER_HANDLE_LIST_NAME(sscpSerialHandleList), UART_send, 0);

/* UART2 callback function. */

void callbackFxn(UART2_Handle handle, void *buffer, size_t count, void *userArg, int_fast16_t status)
{
    if (status != UART2_STATUS_SUCCESS)
    {
        /* RX error occured in UART2_read() */
        while (1) {}
    }

    int index;

    /* Update SSCP from producer process. */

    for( index = 0 ; index < count ; index++ )
    {
        SSCP_handleRxByte(&sscpSerialHandle, ((uint8_t*)buffer)[index] );
    }

    sem_post(&sem);
}


/*
 *  ======== mainThread ========
 */
void *mainThread(void *arg0)
{
    char input;
    const char echoPrompt[] = "Echoing characters:\r\n";
    int32_t semStatus;
    uint32_t status = UART2_STATUS_SUCCESS;

    /* Call driver init functions */
    GPIO_init();

    /* Configure the LED pin */
    GPIO_setConfig(CONFIG_GPIO_LED_0, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW);

    /* Create semaphore */
    semStatus = sem_init(&sem, 0, 0);

    if (semStatus != 0)
    {
        /* Error creating semaphore */
        while (1) {}
    }

    /* Create a UART in CALLBACK read mode */
    UART2_Params_init(&uartParams);
    uartParams.readMode     = UART2_Mode_CALLBACK;
    uartParams.readCallback = callbackFxn;
    uartParams.baudRate     = 115200;

    uart = UART2_open(CONFIG_UART2_0, &uartParams);

    if (uart == NULL)
    {
        /* UART2_open() failed */
        while (1) {}
    }

    /* Turn on user LED to indicate successful initialization */
    GPIO_write(CONFIG_GPIO_LED_0, CONFIG_GPIO_LED_ON);

    /* Pass NULL for bytesWritten since it's not used in this example */
    UART2_write(uart, echoPrompt, sizeof(echoPrompt), NULL);

    /* Loop forever echoing */
    while (1)
    {
        /* Pass NULL for bytesRead since it's not used in this example */
        status = UART2_read(uart, &input, 1, NULL);

        if (status != UART2_STATUS_SUCCESS)
        {
            /* UART2_read() failed */
            while (1) {}
        }

        sem_wait(&sem);

        SSCP_process(&sscpSerialHandle);
    }
}

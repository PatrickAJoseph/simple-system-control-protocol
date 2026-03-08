/******************************************************************************

 @file  main_freertos.c

 @brief main entry of the BLE stack sample application.

 Group: WCS, BTS
 Target Device: cc23xx

 ******************************************************************************
 
 Copyright (c) 2013-2025, Texas Instruments Incorporated
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:

 *  Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.

 *  Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.

 *  Neither the name of Texas Instruments Incorporated nor the names of
    its contributors may be used to endorse or promote products derived
    from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 ******************************************************************************
 
 
 *****************************************************************************/

/*******************************************************************************
 * INCLUDES
 */

/* RTOS header files */
#include <FreeRTOS.h>
#include <stdint.h>
#include <task.h>
#ifdef __ICCARM__
    #include <DLib_Threads.h>
#endif
#include <ti/drivers/Power.h>
#include <ti/drivers/GPIO.h>
#include <ti/devices/DeviceFamily.h>

#include "ti/ble/stack_util/icall/app/icall.h"
#include "ti/ble/stack_util/health_toolkit/assert.h"
#include "ti/ble/stack_util/bcomdef.h"

#include "sscp.h"

#include <pthread.h>

#ifndef USE_DEFAULT_USER_CFG
#include "ti/ble/app_util/config/ble_user_config.h"
// BLE user defined configuration
icall_userCfg_t user0Cfg = BLE_USER_CFG;
#endif // USE_DEFAULT_USER_CFG


/*******************************************************************************
 * MACROS
 */

/*******************************************************************************
 * CONSTANTS
 */

/*******************************************************************************
 * TYPEDEFS
 */

/*******************************************************************************
 * LOCAL VARIABLES
 */

/*******************************************************************************
 * GLOBAL VARIABLES
 */

/*******************************************************************************
 * EXTERNS
 */
extern void appMain(void);
extern void AssertHandler(uint8 assertCause, uint8 assertSubcause);


/* Define register structures. */

struct led_register
{
    uint32_t state              : 1;         //  State of LED.
    uint32_t on                 : 1;         //  LED on command bit.
    uint32_t off                : 1;         //  LED off command bit.
    uint32_t toggle             : 1;         //  LED toggle command.
    uint32_t blink              : 1;         //  LED blink command.
    uint32_t blink_count        : 8;         //  Number of times to blink the LED.
    uint32_t blink_interval_ms  : 16;        //  LED blink interval in milli-seconds.
    uint32_t reserved           : 3;         //  Reserved bits.  
};

struct button_register
{
    uint32_t state              : 1;         // Current state of button.
    uint32_t press_count        : 31;        // Number of times the button was pressed.
};

struct led_register led0_register;
struct led_register led1_register;
struct button_register button0_register;
struct button_register button1_register;

/* Define callbacks for registers. */

void button0_register_callback(void* reg, int operation)
{
    struct button_register* button0 = reg;

    if( operation == SSCP_REGISTER_OPERATION_READ )
    {
        button0->state = GPIO_read(CONFIG_GPIO_BUTTON_0);       
    }
    else if( operation == SSCP_REGISTER_OPERATION_WRITE )
    {
        
    }
}

void button1_register_callback(void* reg, int operation)
{
    struct button_register* button0 = reg;

    if( operation == SSCP_REGISTER_OPERATION_READ )
    {
        button0->state = GPIO_read(CONFIG_GPIO_BUTTON_1);       
    }
    else if( operation == SSCP_REGISTER_OPERATION_WRITE )
    {
        
    }
}

void led0_register_callback(void* reg, int operation)
{
    struct led_register* led0 = reg;

    int index;

    if( operation == SSCP_REGISTER_OPERATION_READ )
    {
        led0->state = GPIO_read(CONFIG_GPIO_LED_0);
    }
    else if( operation == SSCP_REGISTER_OPERATION_WRITE )
    {
        if( led0->on )
        {
            GPIO_write(CONFIG_GPIO_LED_0, CONFIG_GPIO_LED_ON);
            led0->state = 1;
        }
        else if( led0->off )
        {
            GPIO_write(CONFIG_GPIO_LED_0, CONFIG_GPIO_LED_OFF);
            led0->state = 0;
        }
        else if( led0->toggle )
        {
            GPIO_toggle(CONFIG_GPIO_LED_0);
            led0->state ^= 1;
        }
        else if( led0->blink )
        {
            for( index = 0 ; index < 2 * led0->blink_count ; index++ )
            {
                GPIO_toggle(CONFIG_GPIO_LED_0);
                usleep( 1000 * (uint32_t)led0->blink_interval_ms );
            }
        }
    }
}

void led1_register_callback(void* reg, int operation)
{
    struct led_register* led1 = reg;

    int index;

    if( operation == SSCP_REGISTER_OPERATION_READ )
    {
        led1->state = GPIO_read(CONFIG_GPIO_LED_1);
    }
    else if( operation == SSCP_REGISTER_OPERATION_WRITE )
    {
        if( led1->on )
        {
            GPIO_write(CONFIG_GPIO_LED_1, CONFIG_GPIO_LED_ON);
            led1->state = 1;
        }
        else if( led1->off )
        {
            GPIO_write(CONFIG_GPIO_LED_1, CONFIG_GPIO_LED_OFF);
            led1->state = 0;
        }
        else if( led1->toggle )
        {
            GPIO_toggle(CONFIG_GPIO_LED_1);
            led1->state ^= 1;
        }
        else if( led1->blink )
        {
            for( index = 0 ; index < 2 * led1->blink_count ; index++ )
            {
                GPIO_toggle(CONFIG_GPIO_LED_1);
                usleep( 1000 * (uint32_t)led1->blink_interval_ms );
            }
        }
    }
}


extern uint8_t SSCP_bleResponsePacket[];

int BLE_send(uint8_t* data, size_t size)
{
    memcpy(SSCP_bleResponsePacket, data, size);
    return 0;
}

/* Create a list of register defines. */

static SSCP_REGISTER_HANDLE_LIST(sscpBleHandleList)
{
    SSCP_REGISTER_HANDLE( 0, led0_register, led0_register_callback ),
    SSCP_REGISTER_HANDLE( 1, led1_register, led1_register_callback ),
    SSCP_REGISTER_HANDLE( 2, button0_register, button0_register_callback ),
    SSCP_REGISTER_HANDLE( 3, button1_register, button1_register_callback ),
};

SSCP_HANDLE(sscpBleHandle, SSCP_REGISTER_HANDLE_LIST_NAME(sscpBleHandleList), BLE_send, 0);

/*******************************************************************************
 * @fn          Main
 *
 * @brief       Application Main
 *
 * input parameters
 *
 * @param       None.
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */

#define SSCP_PROCESS_THREAD_STACK_SIZE      512

void* sscpProcessingTask(void* args)
{
    while(1)
    {
        SSCP_process(&sscpBleHandle);
        usleep(10);
    }    
}


void gpioButtonCallback(uint_least8_t index)
{
    if( index == CONFIG_GPIO_BUTTON_0 )
    {
        button0_register.press_count++;
    }
    else if( index == CONFIG_GPIO_BUTTON_1 )
    {
        button1_register.press_count++;
    }
}


int main()
{
  /* Register Application callback to trap asserts raised in the Stack */
  halAssertCback = AssertHandler;
  RegisterAssertCback(AssertHandler);

  Board_init();

    /* Configure buttons. */
    GPIO_setConfig(CONFIG_GPIO_BUTTON_0, GPIO_CFG_IN_PU | GPIO_CFG_INT_FALLING_INTERNAL );
    GPIO_setConfig(CONFIG_GPIO_BUTTON_1, GPIO_CFG_IN_PU | GPIO_CFG_INT_FALLING_INTERNAL );

    /* Set interrupt callback */
    GPIO_setCallback(CONFIG_GPIO_BUTTON_0, gpioButtonCallback);
    GPIO_setCallback(CONFIG_GPIO_BUTTON_1, gpioButtonCallback);

    GPIO_enableInt(CONFIG_GPIO_BUTTON_0);
    GPIO_enableInt(CONFIG_GPIO_BUTTON_1);


  /* Update User Configuration of the stack */
  user0Cfg.appServiceInfo->timerTickPeriod = ICall_getTickPeriod();
  user0Cfg.appServiceInfo->timerMaxMillisecond  = ICall_getMaxMSecs();

  /* Initialize all applications tasks */
  appMain();

    pthread_t thread;
    pthread_attr_t attrs;
    struct sched_param priParam;
    int retc;

    /* Initialize the attributes structure with default values */
    pthread_attr_init(&attrs);

    /* Set priority, detach state, and stack size attributes */
    priParam.sched_priority = 1;
    retc                    = pthread_attr_setschedparam(&attrs, &priParam);
    retc |= pthread_attr_setdetachstate(&attrs, PTHREAD_CREATE_DETACHED);
    retc |= pthread_attr_setstacksize(&attrs, SSCP_PROCESS_THREAD_STACK_SIZE);
    if (retc != 0)
    {
        /* failed to set attributes */
        while (1) {}
    }

    retc = pthread_create(&thread, &attrs, sscpProcessingTask, NULL);
    if (retc != 0)
    {
        /* pthread_create() failed */
        while (1) {}
    }


  /* Start the FreeRTOS scheduler */
  vTaskStartScheduler();

  return 0;

}

//*****************************************************************************
//
//! \brief Application defined stack overflow hook
//!
//! \param  none
//!
//! \return none
//!
//*****************************************************************************
void vApplicationStackOverflowHook(TaskHandle_t pxTask, char *pcTaskName)
{
    //Handle FreeRTOS Stack Overflow
    AssertHandler(HAL_ASSERT_CAUSE_STACK_OVERFLOW_ERROR, 0);
}

/*******************************************************************************
 * @fn          AssertHandler
 *
 * @brief       This is the Application's callback handler for asserts raised
 *              in the stack.  When EXT_HAL_ASSERT is defined in the Stack Wrapper
 *              project this function will be called when an assert is raised,
 *              and can be used to observe or trap a violation from expected
 *              behavior.
 *
 *              As an example, for Heap allocation failures the Stack will raise
 *              HAL_ASSERT_CAUSE_OUT_OF_MEMORY as the assertCause and
 *              HAL_ASSERT_SUBCAUSE_NONE as the assertSubcause.  An application
 *              developer could trap any malloc failure on the stack by calling
 *              HAL_ASSERT_SPINLOCK under the matching case.
 *
 *              An application developer is encouraged to extend this function
 *              for use by their own application.  To do this, add assert.c
 *              to your project workspace, the path to assert.h (this can
 *              be found on the stack side). Asserts are raised by including
 *              assert.h and using macro HAL_ASSERT(cause) to raise an
 *              assert with argument assertCause.  the assertSubcause may be
 *              optionally set by macro HAL_ASSERT_SET_SUBCAUSE(subCause) prior
 *              to asserting the cause it describes. More information is
 *              available in assert.h.
 *
 * input parameters
 *
 * @param       assertCause    - Assert cause as defined in assert.h.
 * @param       assertSubcause - Optional assert subcause (see assert.h).
 *
 * output parameters
 *
 * @param       None.
 *
 * @return      None.
 */
void AssertHandler(uint8_t assertCause, uint8_t assertSubcause)
{
    // check the assert cause
    switch(assertCause)
    {
        case HAL_ASSERT_CAUSE_OUT_OF_MEMORY:
        {
            // ERROR: OUT OF MEMORY
            HAL_ASSERT_SPINLOCK;
            break;
        }

        case HAL_ASSERT_CAUSE_INTERNAL_ERROR:
        {
            // check the subcause
            if(assertSubcause == HAL_ASSERT_SUBCAUSE_FW_INERNAL_ERROR)
            {
                // ERROR: INTERNAL FW ERROR
                HAL_ASSERT_SPINLOCK;
            }
            else
            {
                // ERROR: INTERNAL ERROR
                HAL_ASSERT_SPINLOCK;
            }
            break;
        }

        case HAL_ASSERT_CAUSE_ICALL_ABORT:
        {
            HAL_ASSERT_SPINLOCK;
            break;
        }

        case HAL_ASSERT_CAUSE_ICALL_TIMEOUT:
        {
            HAL_ASSERT_SPINLOCK;
            break;
        }

        case HAL_ASSERT_CAUSE_WRONG_API_CALL:
        {
            HAL_ASSERT_SPINLOCK;
            break;
        }

        case HAL_ASSERT_CAUSE_STACK_OVERFLOW_ERROR:
        {
            HAL_ASSERT_SPINLOCK;
            break;
        }

        case HAL_ASSERT_CAUSE_LL_INIT_RNG_NOISE_FAILURE:
        {
            /*
             * Device must be reset to recover from this case.
             *
             * The HAL_ASSERT_SPINLOCK with is replacable with custom handling,
             * at the end of which Power_reset(); MUST be called.
             *
             * BLE5-stack functionality will be compromised when LL_initRNGNoise
             * fails.
             */
            HAL_ASSERT_SPINLOCK;
            break;
        }

        default:
        {
            HAL_ASSERT_SPINLOCK;
            break;
        }
    }

    return;
}

/*******************************************************************************
 */

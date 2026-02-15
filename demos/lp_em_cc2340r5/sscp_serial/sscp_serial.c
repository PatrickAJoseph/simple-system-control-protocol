
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

/* Create a list of register defines. */

static SSCP_REGISTER_HANDLE_LIST(sscpSerialHandleList)
{
    SSCP_REGISTER_HANDLE( 0, led0_register, led0_register_callback ),
    SSCP_REGISTER_HANDLE( 1, led1_register, led1_register_callback ),
    SSCP_REGISTER_HANDLE( 2, button0_register, button0_register_callback ),
    SSCP_REGISTER_HANDLE( 3, button1_register, button1_register_callback ),
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


/*
 *  ======== mainThread ========
 */
void *mainThread(void *arg0)
{
    char input;
    int32_t semStatus;
    uint32_t status = UART2_STATUS_SUCCESS;

    /* Call driver init functions */
    GPIO_init();

    /* Configure the LED pins */
    GPIO_setConfig(CONFIG_GPIO_LED_0, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW);
    GPIO_setConfig(CONFIG_GPIO_LED_1, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW);

    /* Configure buttons. */
    GPIO_setConfig(CONFIG_GPIO_BUTTON_0, GPIO_CFG_IN_PU | GPIO_CFG_INT_FALLING_INTERNAL );
    GPIO_setConfig(CONFIG_GPIO_BUTTON_1, GPIO_CFG_IN_PU | GPIO_CFG_INT_FALLING_INTERNAL );

    /* Set interrupt callback */
    GPIO_setCallback(CONFIG_GPIO_BUTTON_0, gpioButtonCallback);
    GPIO_setCallback(CONFIG_GPIO_BUTTON_1, gpioButtonCallback);

    GPIO_enableInt(CONFIG_GPIO_BUTTON_0);
    GPIO_enableInt(CONFIG_GPIO_BUTTON_1);

    /* Create semaphore */
    semStatus = sem_init(&sem, 0, 0);

    if (semStatus != 0)
    {
        __asm volatile("bkpt 0");
    }

    /* Create a UART in CALLBACK read mode */
    UART2_Params_init(&uartParams);
    uartParams.readMode     = UART2_Mode_CALLBACK;
    uartParams.readCallback = callbackFxn;
    uartParams.baudRate     = 115200;

    uart = UART2_open(CONFIG_UART2_0, &uartParams);

    if (uart == NULL)
    {
        __asm volatile("bkpt 0");
    }

    while (1)
    {
        status = UART2_read(uart, &input, 1, NULL);

        if (status != UART2_STATUS_SUCCESS)
        {
            __asm volatile("bkpt 0");
        }

        sem_wait(&sem);

        (void)SSCP_process(&sscpSerialHandle);
    }
}

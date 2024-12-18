/***************************************************************************//**
 * @file main.c
 * @brief main() function.
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/
#include "sl_component_catalog.h"
#include "sl_system_init.h"
#include "app.h"
#if defined(SL_CATALOG_POWER_MANAGER_PRESENT)
#include "sl_power_manager.h"
#endif
#if defined(SL_CATALOG_KERNEL_PRESENT)
#include "sl_system_kernel.h"
#else // SL_CATALOG_KERNEL_PRESENT
#include "sl_system_process_action.h"
#endif // SL_CATALOG_KERNEL_PRESENT


#include "em_device.h"
#include "em_chip.h"
#include "em_emu.h"
#include "em_cmu.h"
#include "em_gpio.h"
#include "em_usart.h"

#define BSP_TXPORT gpioPortA
#define BSP_RXPORT gpioPortA
#define BSP_TXPIN 5
#define BSP_RXPIN 6
#define BSP_ENABLE_PORT gpioPortD
#define BSP_ENABLE_PIN 4

typedef enum {
    STATE_WORKING,
    STATE_SAMPLE_DHT,
    STATE_ADV_BLE
} uart_state_t;

extern uart_state_t CHECK_STATE;
uint32_t bufferUART;

void initGPIO(void)
{
  // Configure the USART TX pin to the board controller as an output
  GPIO_PinModeSet(BSP_TXPORT, BSP_TXPIN, gpioModePushPull, 1);

  // Configure the USART RX pin to the board controller as an input
  GPIO_PinModeSet(BSP_RXPORT, BSP_RXPIN, gpioModeInput, 0);

  /*
   * Configure the BCC_ENABLE pin as output and set high.  This enables
   * the virtual COM port (VCOM) connection to the board controller and
   * permits serial port traffic over the debug connection to the host
   * PC.
   *
   * To disable the VCOM connection and use the pins on the kit
   * expansion (EXP) header, comment out the following line.
   */
  GPIO_PinModeSet(BSP_ENABLE_PORT, BSP_ENABLE_PIN, gpioModePushPull, 1);
}

void initUSART0(void)
{
  CMU_ClockEnable(cmuClock_USART0, true);
  CMU_ClockEnable(cmuClock_GPIO, true);


  // Default asynchronous initializer (115.2 Kbps, 8N1, no flow control)
  USART_InitAsync_TypeDef init = USART_INITASYNC_DEFAULT;

  // Route USART0 TX and RX to the board controller TX and RX pins
  GPIO->USARTROUTE[0].TXROUTE = (BSP_TXPORT << _GPIO_USART_TXROUTE_PORT_SHIFT)
      | (BSP_TXPIN << _GPIO_USART_TXROUTE_PIN_SHIFT);
  GPIO->USARTROUTE[0].RXROUTE = (BSP_RXPORT << _GPIO_USART_RXROUTE_PORT_SHIFT)
      | (BSP_RXPIN << _GPIO_USART_RXROUTE_PIN_SHIFT);

  // Enable RX and TX signals now that they have been routed
  GPIO->USARTROUTE[0].ROUTEEN = GPIO_USART_ROUTEEN_RXPEN | GPIO_USART_ROUTEEN_TXPEN;

  // Configure and enable USART0
  USART_InitAsync(USART0, &init);

  // Enable NVIC USART sources
  NVIC_ClearPendingIRQ(USART0_RX_IRQn);
  NVIC_EnableIRQ(USART0_RX_IRQn);
  NVIC_ClearPendingIRQ(USART0_TX_IRQn);
  NVIC_EnableIRQ(USART0_TX_IRQn);

//  USART_IntEnable(USART0, USART_IEN_RXDATAV);
}

extern uint16_t ADV_BLE, SAMPLE_DHT;
uint8_t bufferUART_in[10], leng = 0;

void USART0_RX_IRQHandler(void)
{
  // Get the character just received
//  bufferUART = USART0->RXDATA;
//
//
//  if (bufferUART == '5')
//  {
//    USART_IntDisable(USART0, USART_IEN_RXDATAV);
//
//    UART_log_info("Input choice: ");
//    while (bufferUART != '1' && bufferUART != '2')
//      bufferUART = USART_Rx(USART0);
//    CHECK_STATE = bufferUART - '0';
//
//    switch (CHECK_STATE)
//    {
//      case STATE_SAMPLE_DHT:
//        leng = 0;
//        UART_log_info("Sample DHT Interval time (01 - 99) seconds: ");
//        bufferUART_in[1] = USART_Rx(USART0);
//        bufferUART_in[0] = USART_Rx(USART0);
//        SAMPLE_DHT = (bufferUART_in[1] - '0') * 10 + (bufferUART_in[0] - '0');
//        break;
//      case STATE_ADV_BLE:
//        leng = 0;
//        UART_log_info("ADV BLE Interval time (01 - 99) seconds: ");
//        bufferUART_in[1] = USART_Rx(USART0);
//        bufferUART_in[0] = USART_Rx(USART0);
//        ADV_BLE = (bufferUART_in[1] - '0') * 10 + (bufferUART_in[0] - '0');
//        break;
//      default:
//        break;
//    }
//    bufferUART = '0';
//    USART_IntEnable(USART0, USART_IEN_RXDATAV);
//    CHECK_STATE = 0;
//  }
}


int main(void)
{

  // Initialize Silicon Labs device, system, service(s) and protocol stack(s).
  // Note that if the kernel is present, processing task(s) will be created by
  // this call.
  sl_system_init();

  // Initialize the application. For example, create periodic timer(s) or
  // task(s) if the kernel is present.
  app_init();
  initGPIO();
  initUSART0();

  memlcd_app_init();
  UART_log_info("Test\n");


#if defined(SL_CATALOG_KERNEL_PRESENT)
  // Start the kernel. Task(s) created in app_init() will start running.
  sl_system_kernel_start();
#else // SL_CATALOG_KERNEL_PRESENT
  while (1) {
    // Do not remove this call: Silicon Labs components process action routine
    // must be called from the super loop.
    sl_system_process_action();

    // Application process.
    app_process_action();

    if (USART0->RXDATA == 'i')
    {
      UART_log_info("Input choice: ");
      while (bufferUART != '1' && bufferUART != '2')
        bufferUART = USART_Rx(USART0);
      CHECK_STATE = bufferUART - '0';

      switch (CHECK_STATE)
      {
        case STATE_SAMPLE_DHT:
          leng = 0;
          UART_log_info("Sample DHT Interval time (01 - 99) seconds: \r\n");
          bufferUART_in[1] = USART_Rx(USART0);
          bufferUART_in[0] = USART_Rx(USART0);
          SAMPLE_DHT = (bufferUART_in[1] - '0') * 10 + (bufferUART_in[0] - '0');
          updateAppTimer();
          break;
        case STATE_ADV_BLE:
          leng = 0;
          UART_log_info("ADV BLE Interval time (01 - 99) seconds: \r\n");
          bufferUART_in[1] = USART_Rx(USART0);
          bufferUART_in[0] = USART_Rx(USART0);
          ADV_BLE = ((bufferUART_in[1] - '0') * 10 + (bufferUART_in[0] - '0'));
          updateADVBLE();
          break;
        default:
          break;
      }
      bufferUART = '0';
      CHECK_STATE = 0;
    }

#if defined(SL_CATALOG_POWER_MANAGER_PRESENT)
    // Let the CPU go to sleep if the system allows it.
    sl_power_manager_sleep();
#endif
  }
#endif // SL_CATALOG_KERNEL_PRESENT
}

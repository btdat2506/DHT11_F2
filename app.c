/***************************************************************************//**
 * @file
 * @brief Core application logic.
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/


#include "em_common.h"
#include "app_assert.h"
#include "sl_bluetooth.h"
#include "gatt_db.h"
#include "app.h"
//#include "app_log.h"
#include "custom_adv.h"
#include "app_timer.h"

#include "em_chip.h"
#include "em_cmu.h"
#include "em_gpio.h"
#include "em_usart.h"


#include "sl_sleeptimer.h"
#include <DHT.h>
#include <stdarg.h>


#define BSP_TXPORT gpioPortA
#define BSP_RXPORT gpioPortA
#define BSP_TXPIN 5
#define BSP_RXPIN 6
#define BSP_ENABLE_PORT gpioPortD
#define BSP_ENABLE_PIN 4

#define BSP_GPIO_LEDS
#define BSP_GPIO_LED0_PORT gpioPortD
#define BSP_GPIO_LED0_PIN 2
#define BSP_GPIO_LED1_PORT gpioPortD
#define BSP_GPIO_LED1_PIN 3
#define BSP_GPIO_PB0_PORT gpioPortB
#define BSP_GPIO_PB0_PIN 0
#define BSP_GPIO_PB1_PORT gpioPortB
#define BSP_GPIO_PB1_PIN 1

uint32_t flag=0;

void initLED_BUTTON(){
  // Enable GPIO clock
  CMU_ClockEnable(cmuClock_GPIO, true);

  // Configure PB0 and PB1 as input with glitch filter enabled
  GPIO_PinModeSet(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, gpioModeInputPullFilter, 1);
  GPIO_PinModeSet(BSP_GPIO_PB1_PORT, BSP_GPIO_PB1_PIN, gpioModeInputPullFilter, 1);

  // Configure LED0 and LED1 as output
  GPIO_PinModeSet(BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN, gpioModePushPull, 0);
  GPIO_PinModeSet(BSP_GPIO_LED1_PORT, BSP_GPIO_LED1_PIN, gpioModePushPull, 0);

  // Enable IRQ for even numbered GPIO pins
  NVIC_EnableIRQ(GPIO_EVEN_IRQn);

  // Enable IRQ for odd numbered GPIO pins
  NVIC_EnableIRQ(GPIO_ODD_IRQn);

  // Enable falling-edge interrupts for PB pins
  GPIO_ExtIntConfig(BSP_GPIO_PB0_PORT, BSP_GPIO_PB0_PIN, BSP_GPIO_PB0_PIN, 0, 1, true);
  GPIO_ExtIntConfig(BSP_GPIO_PB1_PORT, BSP_GPIO_PB1_PIN, BSP_GPIO_PB1_PIN, 0, 1, true);
}


void GPIO_EVEN_IRQHandler(void)
{
  GPIO_IntClear(0x5555);

}

void GPIO_ODD_IRQHandler(void)
{
  GPIO_IntClear(0xAAAA);

}

CustomAdv_t sData; // Our custom advertising data stored here

// This action creates a memory area for our "timer variable".
static app_timer_t update_timer;

// The advertising set handle allocated from Bluetooth stack.
static uint8_t advertising_set_handle = 0xff;

// ============== USER DATA ================ //
DHT_DataTypedef DHT11_Data;
uint32_t Temperature, Humidity;
uint32_t Student_ID = 0x21207130; // Student ID
char buffer_disp[20];

uint16_t ADV_BLE = 1, SAMPLE_DHT = 1;
//uint8_t bufferUART_in[10], leng = 0;

extern uint32_t bufferUART;

typedef enum {
    STATE_WORKING,
    STATE_SAMPLE_DHT,
    STATE_ADV_BLE
} uart_state_t;

uart_state_t CHECK_STATE;

void UART_log_info(const char *format, ...) {
  va_list args;
  va_start(args, format);

  char buffer[256]; // Create a buffer to store the formatted string
  int len = vsnprintf(buffer, sizeof(buffer), format, args);
  va_end(args);

  if (len > 0 && len < sizeof(buffer)) {
    // Send the formatted string to the USART
    for (int i = 0; i < len; i++) {
       USART_Tx(USART0, buffer[i]); // Use the USART_Tx function
    }
  } else {
    // Handle the case where the format string is too long or invalid
     const char *errorMsg = "Error: UART_log format string too long or invalid!\r\n";
    for (int i = 0; errorMsg[i] != '\0'; i++) {
       USART_Tx(USART0, errorMsg[i]); //Use USART_Tx here to send error message
    }
  }
}

/****************************************************************************
 * Application Init.
 *****************************************************************************/
static void update_timer_cb(app_timer_t *timer, void *data)
{
  (void)data;
  (void)timer;



  DHT_GetData(&DHT11_Data);
  Temperature = DHT11_Data.Temperature;
  Humidity = DHT11_Data.Humidity;

  UART_log_info("Nhom DDCH\r\n");
  UART_log_info("Temp: %d C\r\n", Temperature);
  UART_log_info("Humd: %d %%\r\n", Humidity);

  UART_log_info("Sample DHT Interval time: %d\r\n", SAMPLE_DHT);
  UART_log_info("ADV BLE Interval time: %d\r\n", ADV_BLE);


  snprintf(buffer_disp, sizeof(buffer_disp), "Nhiet do: %d C", Temperature );
  memlcd_display(2, buffer_disp);
  snprintf(buffer_disp, sizeof(buffer_disp), "Do am: %d %%", Humidity);
  memlcd_display(3, buffer_disp);
  snprintf(buffer_disp, sizeof(buffer_disp), "Chu Ki Sensor: %d s", SAMPLE_DHT);
  memlcd_display(4, buffer_disp);


  if(Humidity >= 75 && flag == 0)
  {
      GPIO_PinOutSet(BSP_GPIO_LED1_PORT, BSP_GPIO_LED1_PIN);
  }
  else
    GPIO_PinOutClear(BSP_GPIO_LED1_PORT, BSP_GPIO_LED1_PIN);

  update_adv_data(&sData, advertising_set_handle, (uint8_t) Temperature, (uint8_t) Humidity);
}


void updateAppTimer()
{
  sl_status_t sc;
  //initLED_BUTTON();

  sc = app_timer_stop(&update_timer);

  app_assert_status(sc);

  sc = app_timer_start(&update_timer,
                       SAMPLE_DHT*1000,              //ms
                       update_timer_cb,
                       NULL,
                       true);

  app_assert_status(sc);
}

void updateADVBLE()
{
  sl_status_t sc;

  sc = sl_bt_advertiser_stop(advertising_set_handle);
  app_assert_status(sc);

  sc = sl_bt_advertiser_set_timing(
    advertising_set_handle,
    ADV_BLE*1600, // min. adv. interval (milliseconds * 1.6)
    ADV_BLE*1600, // max. adv. interval (milliseconds * 1.6)
    0,   // adv. duration
    0);  // max. num. adv. events
  app_assert_status(sc);

  // Restart advertising after client has disconnected.
  sc = sl_bt_legacy_advertiser_start(advertising_set_handle,
                                     sl_bt_advertiser_connectable_scannable);
  app_assert_status(sc);
}


SL_WEAK void app_init(void)
{
  sl_status_t sc;
  //initLED_BUTTON();

  sc = app_timer_start(&update_timer,
                       SAMPLE_DHT*1000,              //ms
                       update_timer_cb,
                       NULL,
                       true);

  app_assert_status(sc);
}


/**************************************************************************//**
 * Application Process Action.
 *****************************************************************************/
SL_WEAK void app_process_action(void)
{
  /////////////////////////////////////////////////////////////////////////////
  // Put your additional application code here!                              //
  // This is called infinitely.                                              //
  // Do not call blocking functions from here!                               //
  /////////////////////////////////////////////////////////////////////////////
}

/**************************************************************************//**
 * Bluetooth stack event handler.
 * This overrides the dummy weak implementation.
 *
 * @param[in] evt Event coming from the Bluetooth stack.
 *****************************************************************************/
void sl_bt_on_event(sl_bt_msg_t *evt)
{
  sl_status_t sc;
  bd_addr address;
  uint8_t address_type;
  uint8_t system_id[8];
  uint16_t led0_state = GPIO_PinOutGet(BSP_GPIO_LED0_PORT, BSP_GPIO_LED0_PIN);
  uint16_t led1_state = GPIO_PinOutGet(BSP_GPIO_LED1_PORT, BSP_GPIO_LED1_PIN);

  switch (SL_BT_MSG_ID(evt->header)) {
    // -------------------------------
    // This event indicates the device has started and the radio is ready.
    // Do not call any stack command before receiving this boot event!
    case sl_bt_evt_system_boot_id:

      // Extract unique ID from BT Address.
      sc = sl_bt_system_get_identity_address(&address, &address_type);
      app_assert_status(sc);

      // Pad and reverse unique ID to get System ID.
      system_id[0] = address.addr[5];
      system_id[1] = address.addr[4];
      system_id[2] = address.addr[3];
      system_id[3] = 0xFF;
      system_id[4] = 0xFE;
      system_id[5] = address.addr[2];
      system_id[6] = address.addr[1];
      system_id[7] = address.addr[0];

      sc = sl_bt_gatt_server_write_attribute_value(gattdb_system_id,
                                                   0,
                                                   sizeof(system_id),
                                                   system_id);
      app_assert_status(sc);

      // Create an advertising set.
      sc = sl_bt_advertiser_create_set(&advertising_set_handle);
      app_assert_status(sc);

      // Set advertising interval to 1s.
      sc = sl_bt_advertiser_set_timing(
        advertising_set_handle,
        ADV_BLE*1600, // min. adv. interval (milliseconds * 1.6)
        ADV_BLE*1600, // max. adv. interval (milliseconds * 1.6)
        0,   // adv. duration
        0);  // max. num. adv. events
      app_assert_status(sc);

      // Setting channel
      sl_bt_advertiser_set_channel_map(advertising_set_handle, 7);
      app_assert_status(sc);



      // Add data to Adv packet
      fill_adv_packet(&sData, FLAG, COMPANY_ID, led0_state, led1_state, "DANGDATCHIHOANG");
      UART_log_info("fill_adv_packet completed\r\n");

      // Start advertise
      start_adv(&sData, advertising_set_handle);
      UART_log_info("Started advertising\r\n");

      break;

    // -------------------------------
    // This event indicates that a new connection was opened.
    case sl_bt_evt_connection_opened_id:
      break;

    // -------------------------------
    // This event indicates that a connection was closed.
    case sl_bt_evt_connection_closed_id:
      // Generate data for advertising
      sc = sl_bt_legacy_advertiser_generate_data(advertising_set_handle,
                                                 sl_bt_advertiser_general_discoverable);
      app_assert_status(sc);

      // Restart advertising after client has disconnected.
      sc = sl_bt_legacy_advertiser_start(advertising_set_handle,
                                         sl_bt_advertiser_connectable_scannable);
      app_assert_status(sc);
      break;

    ///////////////////////////////////////////////////////////////////////////
    // Add additional event handlers here as your application requires!      //
    ///////////////////////////////////////////////////////////////////////////

    // -------------------------------
    // Default event handler.
    default:
      break;
  }
}

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

#include <sl_udelay.h>
#include <app_lcd.h>
#include "em_cmu.h"
#include "em_gpio.h"
#include "sl_sleeptimer.h"
#include <DHT.h>
#include "app_log.h"

int main(void)
{
  // Initialize Silicon Labs device, system, service(s) and protocol stack(s).
  // Note that if the kernel is present, processing task(s) will be created by
  // this call.
  sl_system_init();

  // Initialize the application. For example, create periodic timer(s) or
  // task(s) if the kernel is present.
  app_init();
  memlcd_app_init();
  app_log_info("Test\n");
  DHT_DataTypedef DHT11_Data;
  uint32_t Temperature, Humidity;

  CMU_ClockEnable(cmuClock_GPIO, true);


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

    char buffer_temper[16];
    char buffer_humi[16];

    DHT_GetData(&DHT11_Data);
    Temperature = DHT11_Data.Temperature;
    Humidity = DHT11_Data.Humidity;
    set_temper_humi(Temperature, Humidity);
        app_log_info("Nhom DDCH\r\n");
        app_log_info("Temp: %d\r\n", DHT11_Data.Temperature);
        app_log_info("Humd: %d\r\n", DHT11_Data.Humidity);
    sl_sleeptimer_delay_millisecond(1000); //1s
    snprintf(buffer_temper, sizeof(buffer_temper), "Nhiet do: %d",Temperature );
    snprintf(buffer_humi, sizeof(buffer_humi), "Do am: %d", Humidity);

    memlcd_display_temperature(buffer_temper);
    memlcd_display_humidity(buffer_humi);
#if defined(SL_CATALOG_POWER_MANAGER_PRESENT)
    // Let the CPU go to sleep if the system allows it.
    sl_power_manager_sleep();
#endif
  }
#endif // SL_CATALOG_KERNEL_PRESENT
}

#include <string.h>
#include "custom_adv.h"
#include "stdio.h"
#include <stdint.h>

#define NAME_MAX_LENGTH 16


void fill_adv_packet(CustomAdv_t *pData, uint8_t flags, uint16_t companyID, uint16_t led0, uint16_t led1, char *name)
{
  int n;

  pData->len_flags = 0x02;
  pData->type_flags = 0x01;
  pData->val_flags = flags;

  pData->len_manuf = 7;
  pData->type_manuf = 0xFF;
  pData->company_LO = companyID & 0xFF;
  pData->company_HI = (companyID >> 8) & 0xFF;

  pData->led1_high = (led1 >> 8) & 0xFF;
  pData->led1_low = led1 & 0xFF;
  pData->led0_high = (led0 >> 8) & 0xFF;
  pData->led0_low = led0 & 0xFF;

  n = strlen(name);
  if (n > NAME_MAX_LENGTH - 1) {
    pData->type_name = 0x08;
  } else {
    pData->type_name = 0x09;
  }

  strncpy(pData->name, name, NAME_MAX_LENGTH - 1);
  pData->name[NAME_MAX_LENGTH - 1] = '\0';  // Đảm bảo chuỗi có null terminator

  if (n > NAME_MAX_LENGTH - 1) {
    n = NAME_MAX_LENGTH - 1;
  }

  pData->len_name = 1 + n;

  pData->data_size = 3 + (1 + pData->len_manuf) + (1 + pData->len_name);
}

void start_adv(CustomAdv_t *pData, uint8_t advertising_set_handle)
{
  sl_status_t sc;
  sc = sl_bt_legacy_advertiser_set_data(advertising_set_handle, 0, pData->data_size, (const uint8_t *)pData);
  app_assert(sc == SL_STATUS_OK,
                "[E: 0x%04x] Failed to set advertising data\n",
                (int)sc);

  sc = sl_bt_legacy_advertiser_start(advertising_set_handle, sl_bt_legacy_advertiser_connectable);
  app_assert(sc == SL_STATUS_OK,
                  "[E: 0x%04x] Failed to start advertising\n",
                  (int)sc);
}

void update_adv_data(CustomAdv_t *pData, uint8_t advertising_set_handle, uint16_t led0, uint16_t led1)
{
  sl_status_t sc;

  pData->led1_high = (led1 >> 8) & 0xFF;
  pData->led1_low = led1 & 0xFF;
  pData->led0_high = (led0 >> 8) & 0xFF;
  pData->led0_low = led0 & 0xFF;

  sc = sl_bt_legacy_advertiser_set_data(advertising_set_handle, 0, pData->data_size, (const uint8_t *)pData);
  app_assert(sc == SL_STATUS_OK,
                  "[E: 0x%04x] Failed to set advertising data\n",
                  (int)sc);
}

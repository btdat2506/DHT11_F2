

/************** MAKE CHANGES HERE ********************/
#include <DHT.h>

#define TYPE_DHT11    // define according to your sensor
//#define TYPE_DHT22

#define DHT_PORT gpioPortB
#define DHT_PIN 0


/*******************************************     NO CHANGES AFTER THIS LINE      ****************************************************/

uint8_t Rh_byte1, Rh_byte2, Temp_byte1, Temp_byte2;
uint16_t SUM; uint8_t Presence = 0;

#include "DHT.h"

// uint32_t DWT_Delay_Init(void)
// {
//   /* Disable TRC */
//   CoreDebug->DEMCR &= ~CoreDebug_DEMCR_TRCENA_Msk; // ~0x01000000;
//   /* Enable TRC */
//   CoreDebug->DEMCR |=  CoreDebug_DEMCR_TRCENA_Msk; // 0x01000000;

//   /* Disable clock cycle counter */
//   DWT->CTRL &= ~DWT_CTRL_CYCCNTENA_Msk; //~0x00000001;
//   /* Enable  clock cycle counter */
//   DWT->CTRL |=  DWT_CTRL_CYCCNTENA_Msk; //0x00000001;

//   /* Reset the clock cycle counter value */
//   DWT->CYCCNT = 0;

//      /* 3 NO OPERATION instructions */
//      __ASM volatile ("NOP");
//      __ASM volatile ("NOP");
//   __ASM volatile ("NOP");

//   /* Check if clock cycle counter has started */
//      if(DWT->CYCCNT)
//      {
//        return 0; /*clock cycle counter started*/
//      }
//      else
//   {
//     return 1; /*clock cycle counter not started*/
//   }
// }

// __STATIC_INLINE void delay(volatile uint32_t microseconds)
// {
//   uint32_t clk_cycle_start = DWT->CYCCNT;

//   /* Go to number of cycles for system */
//   microseconds *= (HAL_RCC_GetHCLKFreq() / 1000000);

//   /* Delay till end */
//   while ((DWT->CYCCNT - clk_cycle_start) < microseconds);
// }

void Set_Pin_Output (GPIO_Port_TypeDef *GPIOx, unsigned int GPIO_Pin)
{
	// GPIO_InitTypeDef GPIO_InitStruct = {0};
	// GPIO_InitStruct.Pin = GPIO_Pin;
	// GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	// GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	// HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);
	GPIO_PinModeSet(GPIOx, GPIO_Pin, gpioModePushPull, 0);

}

void Set_Pin_Input (GPIO_Port_TypeDef *GPIOx, unsigned int GPIO_Pin)
{
	// GPIO_InitTypeDef GPIO_InitStruct = {0};
	// GPIO_InitStruct.Pin = GPIO_Pin;
	// GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	// GPIO_InitStruct.Pull = GPIO_NOPULL;
	// HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);
	GPIO_PinModeSet(GPIOx, GPIO_Pin, gpioModeInputPullFilter, 0);
}


void DHT_Start (void)
{
	// DWT_Delay_Init();
	Set_Pin_Output (DHT_PORT, DHT_PIN);  // set the pin as output
	GPIO_PinOutClear (DHT_PORT, DHT_PIN);   // pull the pin low
	sl_udelay_wait (18000);   // wait for 18ms

    GPIO_PinOutSet (DHT_PORT, DHT_PIN);   // pull the pin high
    sl_udelay_wait (20);   // wait for 30us
	Set_Pin_Input(DHT_PORT, DHT_PIN);    // set as input
}

uint8_t DHT_Check_Response (void)
{
	uint8_t Response = 0;
	sl_udelay_wait(40);
	if (!(GPIO_PinInGet (DHT_PORT, DHT_PIN)))
	{
		sl_udelay_wait(80);
		if ((GPIO_PinInGet (DHT_PORT, DHT_PIN))) Response = 1;
		else Response = -1;
	}
	while ((GPIO_PinInGet (DHT_PORT, DHT_PIN)));   // wait for the pin to go low

	return Response;
}

uint8_t DHT_Read (void)
{
	uint8_t i,j;
	for (j=0;j<8;j++)
	{
		while (!(GPIO_PinInGet (DHT_PORT, DHT_PIN)));   // wait for the pin to go high
		sl_udelay_wait(40);   // wait for 40 us
		if (!(GPIO_PinInGet (DHT_PORT, DHT_PIN)))   // if the pin is low
		{
			i&= ~(1<<(7-j));   // write 0
		}
		else i|= (1<<(7-j));  // if the pin is high, write 1
		while ((GPIO_PinInGet (DHT_PORT, DHT_PIN)));  // wait for the pin to go low
	}
	return i;
}



void DHT_GetData (DHT_DataTypedef *DHT_Data)
{
    DHT_Start ();
	Presence = DHT_Check_Response ();
	Rh_byte1 = DHT_Read ();
	Rh_byte2 = DHT_Read ();
	Temp_byte1 = DHT_Read ();
	Temp_byte2 = DHT_Read ();
	SUM = DHT_Read();

	if (SUM == (Rh_byte1+Rh_byte2+Temp_byte1+Temp_byte2))
	{
		DHT_Data->Temperature = Temp_byte1;
		DHT_Data->Humidity = Rh_byte1;
	}
}



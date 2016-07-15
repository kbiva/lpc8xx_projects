/*
 *  main.c
 *
 *  LPC812 microboard, DS18B20 thermomether and 2.5 digits indicator.
 *  Displays current temperature on a 2.5 digits indicator.
 *  LED blinks at constant speed (period is 1s).
 *
 *  Author: Kestutis Bivainis
 */

#include "chip.h"
#include "2_5_digits_indicator.h"
#include "DS18B20.h"
#include "1-Wire.h"

#define LED_PIN 17

extern uint32_t SystemCoreClock;

const uint32_t OscRateIn = 12000000;
const uint32_t ExtRateIn = 0;

unsigned char ROM_SN[DS18B20_SERIAL_NUM_SIZE];

void WKT_IRQHandler(void) {

  // Clear WKT interrupt request
  Chip_WKT_ClearIntStatus(LPC_WKT);

  // LED will toggle state on wakeup event
  Chip_GPIO_SetPinToggle(LPC_GPIO_PORT, 0, LED_PIN);
}

volatile uint32_t counter=0;

void MRT_IRQHandler(void) {

  if (Chip_MRT_IntPending(LPC_MRT_CH0)) {
    Chip_MRT_IntClear(LPC_MRT_CH0);
    counter++;
  }
}

void delay_ms(uint32_t msec) {

  counter=0;
  while(counter<msec){};
}

int main(void) {

  uint32_t i,temp1;

  // Read clock settings and update SystemCoreClock variable
  SystemCoreClockUpdate();

  // init GPIO
  Chip_GPIO_Init(LPC_GPIO_PORT);
  Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT, 0, LED_PIN);
  Chip_GPIO_SetPinState(LPC_GPIO_PORT, 0, LED_PIN, false);

  Init_2_5_Digits_Indicator();

  // init MRT
  Chip_MRT_Init();
// Channel0 for miliseconds delay
  Chip_MRT_SetMode(LPC_MRT_CH0,MRT_MODE_REPEAT);
  Chip_MRT_SetInterval(LPC_MRT_CH0, SystemCoreClock/1000 | MRT_INTVAL_LOAD);
  Chip_MRT_SetEnabled(LPC_MRT_CH0);
  NVIC_EnableIRQ(MRT_IRQn);
// Channel1
  Chip_MRT_SetMode(LPC_MRT_CH1,MRT_MODE_ONESHOT_BUS_STALL);
  Chip_MRT_SetEnabled(LPC_MRT_CH1);

  IndicatorDisplayMinus();
  delay_ms(2000);

  // reset 1wire bus
  while(One_Wire_Reset()){
    delay_ms(10);
  }

  // search for thermometer
  if(DS18B20_Search_Rom_One_Device(&ROM_SN)!=One_Wire_Success) {
    Chip_GPIO_SetPinState(LPC_GPIO_PORT, 0, LED_PIN, true);
    while(1){};
  }

  // display ROM
  for(i=0;i<DS18B20_SERIAL_NUM_SIZE;i++){
    IndicatorDisplayHexLZ(ROM_SN[i]);
    delay_ms(1000);
    IndicatorClear();
    delay_ms(1000);
  }

  IndicatorDisplayMinus();

  // Alarm/wake timer as chip wakeup source
  Chip_SYSCTL_EnablePeriphWakeup(SYSCTL_WAKEUP_WKTINT);

  // Enable and reset WKT clock
  Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_WKT);
  Chip_SYSCTL_PeriphReset(RESET_WKT);

  // Enable 10KHz oscillator for power down mode
  Chip_PMU_SetPowerDownControl(LPC_PMU, PMU_DPDCTRL_LPOSCEN);

  // Enable WKT interrupt
  NVIC_EnableIRQ(WKT_IRQn);

  // 10KHz clock source for WKT
  Chip_WKT_SetClockSource(LPC_WKT, WKT_CLKSRC_10KHZ);

  // set sleep options
  Chip_SYSCTL_SetDeepSleepPD(SYSCTL_DEEPSLP_BOD_PD | SYSCTL_DEEPSLP_WDTOSC_PD);
  Chip_SYSCTL_SetWakeup(~(SYSCTL_SLPWAKE_IRCOUT_PD | SYSCTL_SLPWAKE_IRC_PD | SYSCTL_SLPWAKE_FLASH_PD));

  while (1) {

    //display temperature
    if(DS18B20_Start_Conversion_by_ROM(&ROM_SN)==One_Wire_Success) {

      // Wait 1s for conversion complete
      Chip_WKT_LoadCount(LPC_WKT, Chip_WKT_GetClockRate(LPC_WKT) * 1);
      // Put chip in power-down
      Chip_PMU_PowerDownState(LPC_PMU);
      // Will return here after wakeup and WKT IRQ
      Chip_WKT_Stop(LPC_WKT);

      if(DS18B20_Get_Conversion_Result_by_ROM_CRC(&ROM_SN,&temp1)==One_Wire_Success) {
        IndicatorDisplayDec(temp1>>4);
      }
      else {
        IndicatorDisplayMinus();
      }
    }
    else {
      IndicatorDisplayMinus();
    }
  }
}

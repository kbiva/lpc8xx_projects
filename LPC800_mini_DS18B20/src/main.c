/*
 * main.c
 *
 *  Created on: 2014.01.26
 *      Author: Kestutis Bivainis
 */


#include "chip.h"
#include "spi.h"
#include "nokia6100.h"
#include "DS18B20.h"
#include "1-Wire.h"

#define LED_PIN 2

const char ascii[] = "0123456789ABCDEF";

const uint32_t OscRateIn = 12000000;
const uint32_t ExtRateIn = 0;

// up to 10 thermometers on 1-Wire bus
unsigned char ROM_SN[10][DS18B20_SERIAL_NUM_SIZE];

void WKT_IRQHandler(void) {
  // Clear WKT interrupt request
  Chip_WKT_ClearIntStatus(LPC_WKT);

  // LED will toggle state on wakeup event
  Chip_GPIO_SetPinToggle(LPC_GPIO_PORT, 0, LED_PIN);
}

int32_t main(void) {

  volatile uint32_t dd;
  uint8_t cnt,i,j,res;
  uint32_t temp1[One_Wire_Device_Number_MAX];

  SystemCoreClockUpdate();

  // init GPIO
  Chip_GPIO_Init(LPC_GPIO_PORT);
  Chip_SYSCTL_PeriphReset(RESET_GPIO);

  // init SPI0 at SystemCoreClock speed
  Chip_SPI_Init(LPC_SPI0);
  Chip_SPI_ConfigureSPI(LPC_SPI0,SPI_MODE_MASTER|
                                 SPI_CLOCK_CPHA0_CPOL0|
                                 SPI_DATA_MSB_FIRST|
                                 SPI_SSEL_ACTIVE_LO);
  //Chip_SPIM_SetClockRate(LPC_SPI0,SystemCoreClock);
  LPC_SPI0->DIV=0;
  Chip_SPI_Enable(LPC_SPI0);

  // init MRT
  Chip_MRT_Init();
  Chip_MRT_SetMode(LPC_MRT_CH1,MRT_MODE_ONESHOT_BUS_STALL);

  // init SWM
  Chip_SWM_Init();
  Chip_SWM_DisableFixedPin(SWM_FIXED_SWCLK);//PIO0_3
  Chip_SWM_DisableFixedPin(SWM_FIXED_SWDIO);//PIO0_2
  Chip_SWM_DisableFixedPin(SWM_FIXED_ACMP_I2);//PIO0_1
  Chip_SWM_DisableFixedPin(SWM_FIXED_ACMP_I1);//PIO0_0
  Chip_SWM_MovablePinAssign(SWM_SPI0_SCK_IO,CLK_PIN);
  Chip_SWM_MovablePinAssign(SWM_SPI0_MOSI_IO,MOSI_PIN);
  Chip_SWM_Deinit();

  // init onboard LED
  Chip_GPIO_SetPinState(LPC_GPIO_PORT,0,LED_PIN,true);
  Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT,0,LED_PIN);

  // init LCD reset pin
  Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT,0,RESET_PIN);

  // init LCD
  LCDInit();
  LCDClearScreenBlack();

  LCDPutStr("kbiva.wordpress.com", 116, 10, WHITE, BLACK);

  // reset 1wire bus
  while(One_Wire_Reset()){
    for(dd=0;dd<1000000;dd++){};
  }

  LCDPutStr("Nokia 6020 LCD and", 101, 4, WHITE, BLACK);
  LCDPutStr("DS18B20 thermometers:", 91, 4, WHITE, BLACK);
  LCDPutStr("Nr       ID         C", 78, 1, WHITE, BLACK);

  // search for thermometers
  DS18B20_Search_Rom2(&cnt,&ROM_SN);

  // display found IDs
  for(j=0;j<cnt;j++) {
    LCDPutChar(j+0x31,66-j*9,2,WHITE, BLACK);
    for(i=0;i<DS18B20_SERIAL_NUM_SIZE;i++) {
      LCDPutChar(ascii[(ROM_SN[j][i] >> 4) & 0x0F],66-j*9,14+(i*12),WHITE, BLACK);
      LCDPutChar(ascii[(ROM_SN[j][i] ) & 0x0F],66-j*9,14+(i*12+6),WHITE, BLACK);
    }
  }

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

    // Setup for wakeup in 1s
    Chip_WKT_LoadCount(LPC_WKT, Chip_WKT_GetClockRate(LPC_WKT) * 1);

    // Put chip in power-down
    Chip_PMU_PowerDownState(LPC_PMU);

    // Will return here after wakeup and WKT IRQ
    Chip_WKT_Stop(LPC_WKT);

    //display temperatures
    for(j=0;j<cnt;j++) {
      DS18B20_Start_Conversion_by_ROM(&ROM_SN[j]);
      res=DS18B20_Get_Conversion_Result_by_ROM_CRC(&ROM_SN[j],&temp1[j]);
      if(res==One_Wire_Success && temp1[j] != 0x550) {
        LCDPutChar(ascii[((temp1[j]>>4)/10) & 0x0F],66-j*9,118,WHITE, BLACK);
        LCDPutChar(ascii[((temp1[j]>>4)%10) & 0x0F],66-j*9,124,WHITE, BLACK);
      }
      else {
        LCDPutChar('E',66-j*9,118,WHITE, BLACK);
        LCDPutChar('r',66-j*9,124,WHITE, BLACK);
      }
    }
  }
}

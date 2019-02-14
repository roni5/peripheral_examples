/**************************************************************************//**
 * @main_gg11_tg11.c
 * @brief The LDMA is configured to transfer a single word to the CCV register
 * once the first compare event occurs. The values loaded into the CCV are such
 * that a 1ms pulse is generated after a 3 second delay
 * @version 0.0.1
 ******************************************************************************
 * @section License
 * <b>Copyright 2018 Silicon Labs, Inc. http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/

#include "em_device.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_chip.h"
#include "em_gpio.h"
#include "em_timer.h"
#include "em_ldma.h"

// Note: change this to change the number of seconds to delay the pulse going high
#define NUM_SEC_DELAY 1

// Note: change this to change the timer prescale
#define TIMER_PRESCALE timerPrescale1024

// Note: change this to change the pulse width (in units of ms)
#define PULSE_WIDTH 1

// Compare values for outputting the rising and falling edge
static uint32_t compareValue1;
static uint32_t compareValue2;

/**************************************************************************//**
 * @brief
 *    Configure and start DMA
 *****************************************************************************/
void initLdma(void)
{
  // Channel descriptor configuration
  static LDMA_Descriptor_t descriptor =
    LDMA_DESCRIPTOR_SINGLE_M2P_BYTE(&compareValue2,     // Memory source address
                                    &TIMER1->CC[0].CCV, // Peripheral destination address
                                    1);                 // Number of bytes per transfer
  descriptor.xfer.size     = ldmaCtrlSizeWord; // Unit transfer size
  descriptor.xfer.doneIfs  = 0;                // Don't trigger interrupt when done

  // Transfer configuration and trigger selection
  LDMA_TransferCfg_t transferConfig =
    LDMA_TRANSFER_CFG_PERIPHERAL(ldmaPeripheralSignal_TIMER1_CC0);

  // LDMA initialization
  LDMA_Init_t init = LDMA_INIT_DEFAULT;
  LDMA_Init(&init);

  // Start the transfer
  uint32_t channelNum = 0;
  LDMA_StartTransfer(channelNum, &transferConfig, &descriptor);
}

/**************************************************************************//**
 * @brief
 *    GPIO initialization
 *****************************************************************************/
void initGpio(void)
{
  CMU_ClockEnable(cmuClock_GPIO, true);

  // Set PC13 as output
  GPIO_PinModeSet(gpioPortC, 13, gpioModePushPull, 0);
}

/**************************************************************************//**
 * @brief
 *    TIMER initialization
 *****************************************************************************/
void initTimer(void)
{
  // Enable clock for TIMER1 module
  CMU_ClockEnable(cmuClock_TIMER1, true);

  // Configure TIMER1 Compare/Capture for output compare
  TIMER_InitCC_TypeDef timerCCInit = TIMER_INITCC_DEFAULT;
  timerCCInit.mode = timerCCModeCompare;
  timerCCInit.cmoa = timerOutputActionToggle; // Toggle output on compare match
  TIMER_InitCC(TIMER1, 0, &timerCCInit);

  // Set route to Location 0 and enable
  // TIM1_CC0 #0 is PC13
  TIMER1->ROUTELOC0 |= TIMER_ROUTELOC0_CC0LOC_LOC0;
  TIMER1->ROUTEPEN |= TIMER_ROUTEPEN_CC0PEN;

  // Initialize and start timer with highest prescale
  TIMER_Init_TypeDef timerInit = TIMER_INIT_DEFAULT;
  timerInit.enable = false;
  timerInit.prescale = TIMER_PRESCALE;
  timerInit.oneShot = true; // Generate only one pulse
  TIMER_Init(TIMER1, &timerInit);

  // Set the first compare value
  compareValue1 = CMU_ClockFreqGet(cmuClock_TIMER1)
                    * NUM_SEC_DELAY
                    / (1 << TIMER_PRESCALE);
  TIMER_CompareSet(TIMER1, 0, compareValue1);

  // Set the second compare value (don't actually use it, just set the global so
  // that it can be used by the handler later)
  compareValue2 = (CMU_ClockFreqGet(cmuClock_TIMER1)
                    * PULSE_WIDTH
                    / 1000
                    / (1 << TIMER_PRESCALE))
                    + compareValue1;

  // Enable the TIMER
  TIMER_Enable(TIMER1, true);
}

/**************************************************************************//**
 * @brief
 *    Main function
 *****************************************************************************/
int main(void)
{
  // Chip errata
  CHIP_Init();

  // Init DCDC regulator with kit specific parameters
  EMU_DCDCInit_TypeDef dcdcInit = EMU_DCDCINIT_DEFAULT;
  EMU_DCDCInit(&dcdcInit);

  // Initializations
  initGpio();
  initLdma();
  initTimer();

  while (1) {
    EMU_EnterEM1(); // Enter EM1 (won't exit)
  }
}


/*
 * Copyright (c) 2022 Arm Limited. All rights reserved.
 */

#include <stddef.h>

#include "sensor_drv.h"
#include "arm_vsi.h"
#include "platform_irq.h"
#ifdef _RTE_
#include "RTE_Components.h"
#endif
#include CMSIS_device_header

/* Sensor Peripheral definitions */
#define SensorO          ARM_VSI1_NS                /* Sensor Output access struct */
#define SensorO_IRQn     ARM_VSI1_IRQn           /* Sensor Output Interrupt number */
#define SensorO_Handler  ARM_VSI1_Handler        /* Sensor Output Interrupt handler */
#define SensorI          ARM_VSI0_NS                /* Sensor Input access struct */
#define SensorI_IRQn     ARM_VSI0_IRQn           /* Sensor Input Interrupt number */
#define SensorI_Handler  ARM_VSI0_Handler        /* Sensor Input Interrupt handler */

/* Sensor Peripheral registers */
#define CONTROL         Regs[0] /* Control receiver */
#define CHANNELS        Regs[1] /* Number of channels */
#define SAMPLE_BITS     Regs[2] /* Sample number of bits (8..32) */
#define SAMPLE_RATE     Regs[3] /* Sample rate (samples per second) */

/* Sensor Control register definitions */
#define CONTROL_ENABLE_Pos      0U                              /* CONTROL: ENABLE Position */
#define CONTROL_ENABLE_Msk      (1UL << CONTROL_ENABLE_Pos)     /* CONTROL: ENABLE Mask */

/* Driver State */
static uint8_t Initialized = 0U;

/* Event Callback */
static SensorDrv_Event_t CB_Event = NULL;

/* Sensor Output Interrupt Handler */
void SensorO_Handler (void) {

  SensorO->IRQ.Clear = 0x00000001U;
  __DSB();
  __ISB();
  if (CB_Event != NULL) {
    CB_Event(SENSOR_DRV_EVENT_TX_DATA);
  }
}

/* Sensor Input Interrupt Handler */
void SensorI_Handler (void) {

  SensorI->IRQ.Clear = 0x00000001U;
  __DSB();
  __ISB();
  if (CB_Event != NULL) {
    CB_Event(SENSOR_DRV_EVENT_RX_DATA);
  }
}

/* Initialize Sensor Interface */
int32_t SensorDrv_Initialize (SensorDrv_Event_t cb_event) {

  CB_Event = cb_event;

  /* Initialize Sensor Output peripheral */
  SensorO->Timer.Control = 0U;
  SensorO->DMA.Control   = 0U;
  SensorO->IRQ.Clear     = 0x00000001U;
  SensorO->IRQ.Enable    = 0x00000001U;
  SensorO->CONTROL       = 0U;

  /* Initialize Sensor Input peripheral */
  SensorI->Timer.Control = 0U;
  SensorI->DMA.Control   = 0U;
  SensorI->IRQ.Clear     = 0x00000001U;
  SensorI->IRQ.Enable    = 0x00000001U;
  SensorI->CONTROL       = 0U;

  /* Enable peripheral interrupts */
  NVIC_EnableIRQ(SensorO_IRQn);
  //NVIC->ISER[(((uint32_t)SensorO_IRQn) >> 5UL)] = (uint32_t)(1UL << (((uint32_t)SensorO_IRQn) & 0x1FUL));

  NVIC_EnableIRQ(SensorI_IRQn);
//  NVIC->ISER[(((uint32_t)SensorI_IRQn) >> 5UL)] = (uint32_t)(1UL << (((uint32_t)SensorI_IRQn) & 0x1FUL));

  __DSB();
  __ISB();

  Initialized = 1U;

  return SENSOR_DRV_OK;
}

/* De-initialize Sensor Interface */
int32_t SensorDrv_Uninitialize (void) {

  /* Disable peripheral interrupts */
NVIC_DisableIRQ(SensorO_IRQn);
//  NVIC->ICER[(((uint32_t)SensorO_IRQn) >> 5UL)] = (uint32_t)(1UL << (((uint32_t)SensorO_IRQn) & 0x1FUL));

NVIC_DisableIRQ(SensorI_IRQn);
//  NVIC->ICER[(((uint32_t)SensorI_IRQn) >> 5UL)] = (uint32_t)(1UL << (((uint32_t)SensorI_IRQn) & 0x1FUL));
  __DSB();
  __ISB();

  /* De-initialize Sensor Output peripheral */
  SensorO->Timer.Control = 0U;
  SensorO->DMA.Control   = 0U;
  SensorO->IRQ.Clear     = 0x00000001U;
  SensorO->IRQ.Enable    = 0x00000000U;
  SensorO->CONTROL       = 0U;

  /* De-initialize Sensor Input peripheral */
  SensorI->Timer.Control = 0U;
  SensorI->DMA.Control   = 0U;
  SensorI->IRQ.Clear     = 0x00000001U;
  SensorI->IRQ.Enable    = 0x00000000U;
  SensorI->CONTROL       = 0U;

  Initialized = 0U;

  return SENSOR_DRV_OK;
}

/* Configure Sensor Interface */
int32_t SensorDrv_Configure (uint32_t interface, uint32_t channels, uint32_t sample_bits, uint32_t sample_rate) {
  uint32_t format;

  if (Initialized == 0U) {
    return SENSOR_DRV_ERROR;
  }

  if ((channels <  1U) ||
      (channels > 32U) ||
      (sample_bits <  8U) ||
      (sample_bits > 32U) ||
      (sample_rate == 0U)) {
    return SENSOR_DRV_ERROR_PARAMETER;
  }

  switch (interface) {
    case SENSOR_DRV_INTERFACE_TX:
      if ((SensorO->CONTROL & CONTROL_ENABLE_Msk) != 0U) {
        return SENSOR_DRV_ERROR;
      }
      SensorO->CHANNELS    = channels;
      SensorO->SAMPLE_BITS = sample_bits;
      SensorO->SAMPLE_RATE = sample_rate;
      break;
    case SENSOR_DRV_INTERFACE_RX:
      if ((SensorI->CONTROL & CONTROL_ENABLE_Msk) != 0U) {
        return SENSOR_DRV_ERROR;
      }
      SensorI->CHANNELS    = channels;
      SensorI->SAMPLE_BITS = sample_bits;
      SensorI->SAMPLE_RATE = sample_rate;
      break;
    default:
      return SENSOR_DRV_ERROR_PARAMETER;
  }

  return SENSOR_DRV_OK;
}

/* Set Sensor Interface buffer */
int32_t SensorDrv_SetBuf (uint32_t interface, void *buf, uint32_t block_num, uint32_t block_size) {

  if (Initialized == 0U) {
    return SENSOR_DRV_ERROR;
  }

  switch (interface) {
    case SENSOR_DRV_INTERFACE_TX:
      if ((SensorO->DMA.Control & ARM_VSI_DMA_Enable_Msk) != 0U) {
        return SENSOR_DRV_ERROR;
      }
      SensorO->DMA.Address   = (uint32_t)buf;
      SensorO->DMA.BlockNum  = block_num;
      SensorO->DMA.BlockSize = block_size;
      break;
    case SENSOR_DRV_INTERFACE_RX:
      if ((SensorI->DMA.Control & ARM_VSI_DMA_Enable_Msk) != 0U) {
        return SENSOR_DRV_ERROR;
      }
      SensorI->DMA.Address   = (uint32_t)buf;
      SensorI->DMA.BlockNum  = block_num;
      SensorI->DMA.BlockSize = block_size;
      break;
    default:
      return SENSOR_DRV_ERROR_PARAMETER;
  }

  return SENSOR_DRV_OK;
}

uint32_t irq, tm_ctrl, dma_ctrl;


/* Control Sensor Interface */
int32_t SensorDrv_Control (uint32_t control) {
  uint32_t sample_size;
  uint32_t sample_rate;
  uint32_t block_size;

  if (Initialized == 0U) {
    return SENSOR_DRV_ERROR;
  }

  if ((control & SENSOR_DRV_CONTROL_TX_DISABLE) != 0U) {
    SensorO->Timer.Control = 0U;
    SensorO->DMA.Control   = 0U;
    SensorO->CONTROL       = 0U;
  } else if ((control & SENSOR_DRV_CONTROL_TX_ENABLE) != 0U) {
    SensorO->CONTROL       = CONTROL_ENABLE_Msk;
    SensorO->DMA.Control   = ARM_VSI_DMA_Direction_M2P |
                            ARM_VSI_DMA_Enable_Msk;
    sample_size = SensorO->CHANNELS * ((SensorO->SAMPLE_BITS + 7U) / 8U);
    sample_rate = SensorO->SAMPLE_RATE;
    if ((sample_size == 0U) || (sample_rate == 0U)) {
      SensorO->Timer.Interval = 0xFFFFFFFFU;
    } else {
      block_size = SensorO->DMA.BlockSize;
      SensorO->Timer.Interval = (1000000U * (block_size / sample_size)) / sample_rate;
    }
    SensorO->Timer.Control = ARM_VSI_Timer_Trig_DMA_Msk |
                             ARM_VSI_Timer_Trig_IRQ_Msk |
                             ARM_VSI_Timer_Periodic_Msk |
                             ARM_VSI_Timer_Run_Msk;
  }

  if ((control & SENSOR_DRV_CONTROL_RX_DISABLE) != 0U) {
    SensorI->Timer.Control = 0U;
    SensorI->DMA.Control   = 0U;
    SensorI->CONTROL       = 0U;
  } else if ((control & SENSOR_DRV_CONTROL_RX_ENABLE) != 0U) {
    SensorI->CONTROL       = CONTROL_ENABLE_Msk;
    SensorI->DMA.Control   = ARM_VSI_DMA_Direction_P2M |
                            ARM_VSI_DMA_Enable_Msk;
    sample_size = SensorI->CHANNELS * ((SensorI->SAMPLE_BITS + 7U) / 8U);
    sample_rate = SensorI->SAMPLE_RATE;
    if ((sample_size == 0U) || (sample_rate == 0U)) {
      SensorI->Timer.Interval = 0xFFFFFFFFU;
    } else {
      block_size = SensorI->DMA.BlockSize;
      SensorI->Timer.Interval = (1000000U * (block_size / sample_size)) / sample_rate;
    }
    SensorI->Timer.Control = ARM_VSI_Timer_Trig_DMA_Msk |
                             ARM_VSI_Timer_Trig_IRQ_Msk |
                             ARM_VSI_Timer_Periodic_Msk |
                             ARM_VSI_Timer_Run_Msk;
  }

  if((control & SENSOR_DRV_CONTROL_RX_PAUSE) != 0U) {
    SensorI->IRQ.Enable    = 0x00000000U;
    SensorI->Timer.Control = 0U;
    SensorI->DMA.Control   = 0U;
  }
  else if((control & SENSOR_DRV_CONTROL_RX_RESUME) != 0U) {
    SensorI->IRQ.Enable    = 0x00000001U;
    SensorI->Timer.Control = ARM_VSI_Timer_Trig_DMA_Msk |
                             ARM_VSI_Timer_Trig_IRQ_Msk |
                             ARM_VSI_Timer_Periodic_Msk |
                             ARM_VSI_Timer_Run_Msk;
    SensorI->DMA.Control   = ARM_VSI_DMA_Direction_P2M |
                             ARM_VSI_DMA_Enable_Msk;
  }

  return SENSOR_DRV_OK;
}

/* Get transmitted block count */
uint32_t SensorDrv_GetTxCount (void) {
  return (SensorO->Timer.Count);
}

/* Get received block count */
uint32_t SensorDrv_GetRxCount (void) {
  return (SensorI->Timer.Count);
}

/* Get Sensor Interface status */
SensorDrv_Status_t SensorDrv_GetStatus (void) {
  SensorDrv_Status_t status;
  uint32_t sr;

  if ((SensorO->CONTROL & CONTROL_ENABLE_Msk) != 0U) {
    status.tx_active = 1U;
  } else {
    status.tx_active = 0U;
  }

  if ((SensorI->CONTROL & CONTROL_ENABLE_Msk) != 0U) {
    status.rx_active = 1U;
  } else {
    status.rx_active = 0U;
  }

  return (status);
}

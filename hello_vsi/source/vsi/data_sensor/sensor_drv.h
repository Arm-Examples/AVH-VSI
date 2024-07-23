/*
 * Copyright (c) 2022 Arm Limited. All rights reserved.
 */

#ifndef __SENSOR_DRV_H
#define __SENSOR_DRV_H

#ifdef  __cplusplus
extern "C"
{
#endif

#include <stdint.h>

/* Sensor Interface */
#define SENSOR_DRV_INTERFACE_TX              (1U)  ///< Transmitter
#define SENSOR_DRV_INTERFACE_RX              (2U)  ///< Receiver

/* Sensor Control */
#define SENSOR_DRV_CONTROL_TX_ENABLE         (1UL << 0)  ///< Enable Transmitter
#define SENSOR_DRV_CONTROL_RX_ENABLE         (1UL << 1)  ///< Enable Receiver
#define SENSOR_DRV_CONTROL_TX_DISABLE        (1UL << 2)  ///< Disable Transmitter
#define SENSOR_DRV_CONTROL_RX_DISABLE        (1UL << 3)  ///< Disable Receiver

#define SENSOR_DRV_CONTROL_TX_PAUSE         (1UL << 4)  ///< Enable Transmitter
#define SENSOR_DRV_CONTROL_RX_PAUSE         (1UL << 5)  ///< Enable Receiver
#define SENSOR_DRV_CONTROL_TX_RESUME        (1UL << 6)  ///< Disable Transmitter
#define SENSOR_DRV_CONTROL_RX_RESUME        (1UL << 7)  ///< Disable Receiver

/* Sensor Event */
#define SENSOR_DRV_EVENT_TX_DATA             (1UL << 0)  ///< Data block transmitted
#define SENSOR_DRV_EVENT_RX_DATA             (1UL << 1)  ///< Data block received

/* Return code */
#define SENSOR_DRV_OK                        (0)  ///< Operation succeeded
#define SENSOR_DRV_ERROR                     (-1) ///< Unspecified error
#define SENSOR_DRV_ERROR_BUSY                (-2) ///< Driver is busy
#define SENSOR_DRV_ERROR_TIMEOUT             (-3) ///< Timeout occurred
#define SENSOR_DRV_ERROR_UNSUPPORTED         (-4) ///< Operation not supported
#define SENSOR_DRV_ERROR_PARAMETER           (-5) ///< Parameter error

/**
\brief Sensor Status
*/
typedef struct {
  uint32_t tx_active        :  1;       ///< Transmitter active
  uint32_t rx_active        :  1;       ///< Receiver active
  uint32_t reserved         : 30;
} SensorDrv_Status_t;

/**
  \fn          SensorDrv_Event_t
  \brief       Sensor Events callback function type: void (*SensorDrv_Event_t) (uint32_t event
  \param[in]   event events notification mask
  \return      none
*/
typedef void (*SensorDrv_Event_t) (uint32_t event);

/**
  \fn          int32_t SensorDrv_Initialize (SensorDrv_Event_t cb_event)
  \brief       Initialize Sensor Interface.
  \param[in]   cb_event pointer to \ref SensorDrv_Event_t
  \return      return code
*/
int32_t SensorDrv_Initialize (SensorDrv_Event_t cb_event);

/**
  \fn          int32_t SensorDrv_Uninitialize (void)
  \brief       De-initialize Sensor Interface.
  \return      return code
*/
int32_t SensorDrv_Uninitialize (void);

/**
  \fn          int32_t SensorDrv_Configure (uint32_t interface, uint32_t channels, uint32_t sample_bits, uint32_t sample_rate)
  \brief       Configure Sensor Interface.
  \param[in]   interface   sensor interface
  \param[in]   channels    number of channels
  \param[in]   sample_bits sample number of bits (8..32)
  \param[in]   sample_rate sample rate (samples per second)
  \return      return code
*/
int32_t SensorDrv_Configure (uint32_t interface, uint32_t channels, uint32_t sample_bits, uint32_t sample_rate);

/**
  \fn          int32_t SensorDrv_SetBuf (uint32_t interface, void *buf, uint32_t block_num, uint32_t block_size)
  \brief       Set Sensor Interface buffer.
  \param[in]   interface   sensor interface
  \param[in]   buf         pointer to buffer for sensor data
  \param[in]   block_num   number of blocks in buffer (must be 2^n)
  \param[in]   block_size  block size in number of samples
  \return      return code
*/
int32_t SensorDrv_SetBuf (uint32_t interface, void *buf, uint32_t block_num, uint32_t block_size);

/**
  \fn          int32_t SensorDrv_Control (uint32_t control)
  \brief       Control Sensor Interface.
  \param[in]   control operation
  \return      return code
*/
int32_t SensorDrv_Control (uint32_t control);

/**
  \fn          uint32_t SensorDrv_GetTxCount (void)
  \brief       Get transmitted block count.
  \return      number of transmitted blocks
*/
uint32_t SensorDrv_GetTxCount (void);

/**
  \fn          uint32_t SensorDrv_GetRxCount (void)
  \brief       Get received block count.
  \return      number of received blocks
*/
uint32_t SensorDrv_GetRxCount (void);

/**
  \fn          SensorDrv_Status_t SensorDrv_GetStatus (void)
  \brief       Get Sensor Interface status.
  \return      \ref SensorDrv_Status_t
*/
SensorDrv_Status_t SensorDrv_GetStatus (void);

#ifdef  __cplusplus
}
#endif

#endif /* __SENSOR_DRV_H */

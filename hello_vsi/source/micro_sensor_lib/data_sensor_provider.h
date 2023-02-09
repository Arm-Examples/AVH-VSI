/* Copyright 2018 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/

#ifndef __DATA_SENSOR_PROVIDER_H__
#define __DATA_SENSOR_PROVIDER_H__

#include <stdint.h>
#include "app_cfg.h"

// This function will read from the sensor buffer to retrive a small amount
// of data. In this example, the data should be used as soon as possible since 
// it might be overwritten. In a real application, you should allow the application
// enough time to use the data before it being overwritten..
uint32_t get_sensor_data(uint32_t num_data, uint32_t *sensor_data_size, DATA_TYPE **sensor_data);

// Returns the amount of data that has been fetched from the Sensor
uint32_t get_total_fetched_sensor_data();

#endif  // __DATA_SENSOR_PROVIDER_H__

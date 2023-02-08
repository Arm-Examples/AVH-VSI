/* Copyright 2022 Tobias Andersson @Arm Ltd. All Rights Reserved.

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

#ifdef _RTE_
#include "RTE_Components.h"
#ifdef RTE_Compiler_EventRecorder
#include "EventRecorder.h"
#else
#define EventStartCv(slot, v1, v2)
#define EventStopCv(slot, v1, v2)
#endif
#endif

#include "app_cfg.h"
#include "main_functions.h"
#include "micro_logger.h"
#include "data_sensor_provider.h"

// Globals, used for compatibility with Arduino-style sketches.
#if DATA_BITSIZE == 8U
uint8_t sensor_samples[DATA_NUM_ELEMENTS];
#elif DATA_BITSIZE == 16U
uint16_t sensor_samples[DATA_NUM_ELEMENTS];
#elif DATA_BITSIZE == 32U
uint32_t sensor_samples[DATA_NUM_ELEMENTS];
#endif
int32_t previous_sample = 0;

// The name of this function is important for Arduino compatibility.
void setup()
{
    previous_sample = 0;
}

// The name of this function is important for Arduino compatibility.
void loop()
{
    // Fetch the spectrogram for the current time.
    const int32_t current_sample = get_total_fetched_sensor_data();
    log_debug("current_sample: %d", current_sample);

    int how_many_new_slices = 0;
    EventStartCv(0, current_sample, previous_sample);

    int sensor_samples_size = 0;

    int sensor_status = get_sensor_samples(DATA_NUM_ELEMENTS, &sensor_samples_size, &sensor_samples);
    log_debug("sensor_status: %d", sensor_status);

    how_many_new_slices = sensor_samples_size;
    log_debug("sensor_samples_size: %d", how_many_new_slices);

    EventStopCv(0, sensor_status, how_many_new_slices);

    if (sensor_status != 0)
    {
        log_error("Something wrong with the sensor. Sensor status: %d", sensor_status);
        return;
    }

    // If no new sensor samples have been received since last time, don't bother
    // printing
    if (how_many_new_slices == 0)
    {
        return;
    }
    previous_sample = current_sample;
    
}

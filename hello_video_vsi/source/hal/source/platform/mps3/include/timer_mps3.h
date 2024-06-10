/*
 * SPDX-FileCopyrightText: Copyright 2021-2022 Arm Limited and/or its affiliates <open-source-office@arm.com>
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef TIMER_MPS3_H
#define TIMER_MPS3_H

#include "platform_pmu.h"

#include <stdint.h>
#include <stdbool.h>

#if defined (ARM_NPU)
    #include "ethosu_profiler.h"    /* Arm Ethos-U NPU profiling functions. */
#endif /* defined (ARM_NPU) */

/* Container for timestamp up-counters. */
typedef struct mps3_pmu_counters_ {
    uint32_t    counter_1Hz;
    uint32_t    counter_100Hz;

    /* Running at FPGA clock rate. See get_mps3_core_clock(). */
    uint32_t    counter_fpga;

    /* Running at processor core's internal clock rate, triggered by SysTick. */
    uint64_t    counter_systick;
} mps3_pmu_counters;

/**
 * @brief   Resets the counters.
 */
void platform_reset_counters(void);

/**
 * @brief       Gets the current counter values.
 * @param[out]  Pointer to a pmu_counters object.
 **/
void platform_get_counters(pmu_counters* counters);

/**
 * @brief  Gets the MPS3 core clock
 * @return Clock rate in Hz expressed as 32 bit unsigned integer.
 */
uint32_t get_mps3_core_clock(void);

/**
 * @brief   System tick interrupt handler.
 **/
void SysTick_Handler(void);

#endif /* TIMER_MPS3_H */

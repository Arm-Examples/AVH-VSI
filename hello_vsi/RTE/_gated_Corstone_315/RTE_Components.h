/*
 * CSOLUTION generated file: DO NOT EDIT!
 * Generated by: csolution version 2.5.0
 *
 * Project: 'hello_vsi.gated+Corstone_315' 
 * Target:  'gated+Corstone_315' 
 */

#ifndef RTE_COMPONENTS_H
#define RTE_COMPONENTS_H


/*
 * Define the Device Header File: 
 */
#define CMSIS_device_header "SSE315.h"

/* ARM::CMSIS Driver:USART@1.1.0 */
#define RTE_USART0      1

/* ARM::CMSIS-Compiler:STDOUT:Custom@1.1.0 */
#define RTE_CMSIS_Compiler_STDOUT                /* CMSIS-Compiler STDOUT */
        #define RTE_CMSIS_Compiler_STDOUT_Custom         /* CMSIS-Compiler STDOUT: Custom */
/* ARM::CMSIS-View:Event Recorder&DAP@1.6.0 */
#define RTE_CMSIS_View_EventRecorder
        #define RTE_CMSIS_View_EventRecorder_DAP
/* ARM::CMSIS:RTOS2:Keil RTX5&Source@5.9.0 */
#define RTE_CMSIS_RTOS2                 /* CMSIS-RTOS2 */
        #define RTE_CMSIS_RTOS2_RTX5            /* CMSIS-RTOS2 Keil RTX5 */
        #define RTE_CMSIS_RTOS2_RTX5_SOURCE     /* CMSIS-RTOS2 Keil RTX5 Source */
/* ARM::Device:Native Driver:IO@1.0.1 */
#define RTE_MPS3_IO      1
/* ARM::Device:Native Driver:PPC@1.2.0 */
#define RTE_MAIN0_PPC_CORSTONE315            1
        #define RTE_MAIN_EXP0_PPC_CORSTONE315        1
        #define RTE_MAIN_EXP1_PPC_CORSTONE315        1
        #define RTE_MAIN_EXP2_PPC_CORSTONE315        1
        #define RTE_MAIN_EXP3_PPC_CORSTONE315        1
        #define RTE_PERIPH0_PPC_CORSTONE315          1
        #define RTE_PERIPH1_PPC_CORSTONE315          1
        #define RTE_PERIPH_EXP0_PPC_CORSTONE315      1
        #define RTE_PERIPH_EXP1_PPC_CORSTONE315      1
        #define RTE_PERIPH_EXP2_PPC_CORSTONE315      1
        #define RTE_PERIPH_EXP3_PPC_CORSTONE315      1
/* ARM::Device:Native Driver:SysCounter@1.0.1 */
#define RTE_SYSCOUNTER      1
/* ARM::Device:Native Driver:Timeout@1.0.1 */
#define RTE_TIMEOUT      1


#endif /* RTE_COMPONENTS_H */

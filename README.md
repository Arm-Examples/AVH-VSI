# Basic Virtual Interface Applications

This repository contains a simple Hello World example for Arm Virtual Hardware.

## Examples Description

| Example name                              | Description   |
|---                                            |---            |
| [Hello VSI](./hello_vsi)                     | A sample of using the Virtual Streaming Interface on AVH FVPs. Sensor data is streamed to the embedded application via the python interface from a text file. |


## Build a project

The project are provided in [csolution format](https://github.com/Open-CMSIS-Pack/cmsis-toolbox/blob/main/docs/YML-Input-Format.md) and can be open for editing and build in **Keil MDK for VsCode** or **Keil uVision** IDEs. Corresponding IDE documentation pages explain this in more details:
 - [Keil MDK VsCode Get started with an example project](https://developer.arm.com/documentation/108029/latest/Get-started-with-an-example-project)
 - [uVision Open Project menu](https://developer.arm.com/documentation/101407/latest/User-Interface/Project-Menu-and-Commands)

You can also use *cbuild* utility from [CMSIS-Toolbox](https://github.com/Open-CMSIS-Pack/cmsis-toolbox/blob/main/docs/build-operation.md) to build the project in command line. For example for hello_vsi project, Corstone-315 target with build type event:

      ```bash
      cbuild hello_vsi.csolution.yml --context .event+Corstone_315 --update-rte --packs --toolchain GCC
      ```

## Arm Virtual Hardware (AVH) Setup

Follow the [Infrastructure documentation](https://arm-software.github.io/AVH/main/infrastructure/html/index.html) on how to setup AVH FVPs on the cloud or locally.

## Version History

| Version | Date | Release notes |
|---      |---   |---            |
| [v0.0.1c](https://github.com/Arm-Examples/Hello_AVH/releases/tag/v0.0.1c) | 2023.4.12 | Minor fix for v0.0.1b |
| [v0.0.1b](https://github.com/Arm-Examples/Hello_AVH/releases/tag/v0.0.1b) | 2023.3.29 | Minor fix for v0.0.1 |
| [v0.0.1](https://github.com/Arm-Examples/Hello_AVH/releases/tag/v0.0.1) | 2023.3.13 | Arm VSI demo for Arm Virtual Hardware. |
# Basic Virtual Interface Applications
[![CMSIS Compliance](https://img.shields.io/github/actions/workflow/status/Arm-Examples/Hello_AVH/verify.yml?logo=arm&logoColor=0091bd&label=CMSIS%20Compliance)](https://www.keil.arm.com/cmsis) 

This repository contains a simple examples demonstrating use of virtual peripherals on AVH FVPs.

## Examples Description

| Example name                              | Description   |
|---                                        |---            |
| [Hello VSI](./hello_vsi)                  | A sample of using the Virtual Streaming Interface on AVH FVPs. Sensor data is streamed to the embedded application via the python interface from a text file. |
| [Hello Video VSI](./hello_video_vsi)      | A sample of using the Virtual Streaming Interface on AVH FVPs for video streaming input. Content of a data file/web ca, is streamed to the embedded application via the python interface and is displayed on the model's LCD. |

## Build a project

The projects are provided in [csolution format](https://github.com/Open-CMSIS-Pack/cmsis-toolbox/blob/main/docs/YML-Input-Format.md) and can be open for editing and building in **Keil MDK for VS Code** or **Keil uVision** IDEs. Corresponding IDE documentation pages explain this in more details:
 - [Keil Studio VS Code Get started with an example project](https://developer.arm.com/documentation/108029/latest/Get-started-with-an-example-project)
 - [uVision Open Project menu](https://developer.arm.com/documentation/101407/latest/User-Interface/Project-Menu-and-Commands)

You can also use *cbuild* utility from [CMSIS-Toolbox](https://github.com/Open-CMSIS-Pack/cmsis-toolbox/blob/main/docs/build-operation.md) to build the project in command line. For example for hello_vsi project, Corstone-315 target with build type event:

```bash
cbuild hello_vsi.csolution.yml --context .event+Corstone_315 --update-rte --packs --toolchain GCC
```

README files in the project folder provide additional details.

## Arm Virtual Hardware (AVH) Setup

See the [AVH FVP Infrastructure](https://arm-software.github.io/AVH/main/infrastructure/html/index.html) on how to setup AVH FVPs for use in local and cloud-based environments.

# Basic Virtual Interface Applications
[![License](https://img.shields.io/github/license/Arm-Examples/AVH-VSI?label)](https://github.com/Arm-Examples/AVH-VSI/blob/main/LICENSE)
[![CMSIS Compliance](https://img.shields.io/github/actions/workflow/status/Arm-Examples/Hello_AVH/cmsis_verify.yml?logo=arm&logoColor=0091bd&label=CMSIS%20Compliance)](https://www.keil.arm.com/cmsis) 
[![hello_vsi Build and Run](https://img.shields.io/github/actions/workflow/status/Arm-Examples/AVH-VSI/hello_vsi.yml?logo=arm&logoColor=0091bd&label=hello_vsi%20Build%20and%20Run)](https://github.com/Arm-Examples/AVH-VSI/tree/main/.github/workflows/hello_vsi.yml)
[![hello_video_vsi Build](https://img.shields.io/github/actions/workflow/status/Arm-Examples/AVH-VSI/hello_video_vsi.yml?logo=arm&logoColor=0091bd&label=hello_video_vsi%20Build)](https://github.com/Arm-Examples/AVH-VSI/tree/main/.github/workflows/hello_video_vsi.yml)

This repository contains simple examples demonstrating the use of virtual peripherals on AVH FVPs.

## Quick Start

1. Install [Keil Studio for VS Code](https://marketplace.visualstudio.com/items?itemName=Arm.keil-studio-pack) from the VS Code marketplace.
2. Clone this repository (for example using [Git in VS Code](https://code.visualstudio.com/docs/sourcecontrol/intro-to-git)) or download the ZIP file. Then open the base folder in VS Code.
3. Open the [CMSIS View](https://mdk-packs.github.io/vscode-cmsis-solution-docs/userinterface.html#2-main-area-of-the-cmsis-view) in VS Code and use the ... menu to choose an example via *Select Active Solution from workspace*.
4. The related tools and software packs are downloaded and installed. Review progress with *View - Output - CMSIS Solution*.
5. In the CMSIS view, use the [Action buttons](https://github.com/ARM-software/vscode-cmsis-csolution?tab=readme-ov-file#action-buttons) to build, load and debug the example on the hardware.

## Examples Description

| Example name                              | Description   |
|---                                        |---            |
| [Hello VSI](./hello_vsi)                  | A simple example of using the Virtual Streaming Interface (VSI) on AVH FVPs. Sensor data is streamed to the embedded application via the python interface from a text file. More details in [hello_vsi/README.MD](./hello_vsi/README.md). |
| [Hello Video VSI](./hello_video_vsi)      | An example of using the VSI on AVH FVPs for video streaming. The content of an image/video file is streamed to the embedded application via the python interface and is displayed on the model's LCD. More details in [hello_video_vsi/README.MD](./hello_video_vsi/README.md). |

## Build a project

The projects are provided in [csolution format](https://github.com/Open-CMSIS-Pack/cmsis-toolbox/blob/main/docs/YML-Input-Format.md) and can be opened for editing and building in **Keil MDK for VS Code** or **Keil uVision** IDEs. Corresponding IDE documentation pages explain this in more detail:

- [Keil Studio VS Code Get started with an example project](https://developer.arm.com/documentation/108029/latest/Get-started-with-an-example-project)
- [uVision Open Project menu](https://developer.arm.com/documentation/101407/latest/User-Interface/Project-Menu-and-Commands)

You can also use the *cbuild* utility from [CMSIS-Toolbox](https://github.com/Open-CMSIS-Pack/cmsis-toolbox/blob/main/docs/build-operation.md) to build the project on the command line.

README files in the project folders provide additional details.

## Arm Virtual Hardware (AVH) Setup

See the [AVH FVP Infrastructure](https://arm-software.github.io/AVH/main/infrastructure/html/index.html) on how to setup AVH FVPs for use in local and cloud-based environments.


## Continuous Integration (CI)

The underlying build system of [Keil Studio](https://www.keil.arm.com/) uses the [CMSIS-Toolbox](https://open-cmsis-pack.github.io/cmsis-toolbox/) and CMake. [CI](https://en.wikipedia.org/wiki/Continuous_integration) is effectively supported with:

- Tool installation based on a single [`vcpkg-configuration.json`](./vcpkg-configuration.json) file for desktop and CI environments.
- CMSIS solution files (`*.csolution.yml`) that enable seamless builds in CI, for example using GitHub actions.

| <div style="width:150px"> CI Workflow </div>    | Description |
|---                                              |--- |
| [hello_vsi.yml](/.github/workflows/hello_vsi.yml) | Build the application for a specific compiler, target, and build type configuration. Save the build output as artifact, execute the generated binary on model and save the FVP UART log as artifact.|
| [hello_video_vsi.yml](/.github/workflows/hello_video_vsi.yml) | Build the application for a specific compiler and target pair. Save the build output as artifact. |


## Files and Directories

This is a list of the relevant files and directories.

| <div style="width:150px"> File/Directory </div>      | Content |
|---                                                   |--- |
| [vcpkg-configuration.json](./vcpkg-configuration.json) | List of tools installed (by [Arm Tools Environment Manager](https://marketplace.visualstudio.com/items?itemName=Arm.environment-manager)) in VS Code and the CI Build system. |
| [.github/workflows](./.github/workflows)               | Contains setup for the CI Build and Run workflows. |
| [hello_vsi](./hello_vsi)                               | Contains the example to use Virtual Streaming Interface (VSI) on AVH FVPs. |
| [hello_video_vsi](./hello_video_vsi)                   | Contains the example to use VSI on AVH FVPs for video streaming. |

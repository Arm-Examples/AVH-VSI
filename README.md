# Basic Virtual Interface Applications

This repository contains a simple Hello World example for Arm Virtual Hardware.

## Examples Description

| Example name                              | Description   |
|---                                            |---            |
| [Hello VSI](./hello_vsi)                     | A sample of using the Virtual Streaming Interface on AVH. Data is streamed to the application via the python interface.There are different versions such as GUI version and gated fetch for various use cases. |

## Arm Virtual Hardware (AVH) Setup

Follow the [Infrasructure documentation](https://arm-software.github.io/AVH/main/infrastructure/html/index.html) on how to setup AVH on the cloud or locally.

### Change the Arm Tool Variant

In order to build applications for [Arm Corstone-310](https://developer.arm.com/Processors/Corstone-310), you need to use the `ult` tool variant. This is not necessary for Corstone-300 applications.

Change the tool variant by setting the following environment variable on the cloud or local build environment:

```bash
export ARM_TOOL_VARIANT='ult'
```

To make this permanent, you can add the line to your `~/.bashrc` file.

## Build a project

1. Clone this project using git.

    ```bash
    git clone https://github.com/Arm-Examples/Hello_AVH.git
    ```

1. Enter the application dir

    ```bash
    cd Hello_AVH/<example>
    ```

1. Update the keil packs index

    ```bash
    cpackget update-index
    ```

1. Build the applicaiton

    ```bash
    cbuild --packs target/<platform>/<application>.cprj
    ```

## Run the Applicaiton using Virtual Hardware

```bash
./run_example.sh
```

## Common Issues

### Missing pack(s)

If you experience the following error:

```bash
M654: Package '<vendor>::<package>@<version>' was added to the list of missing packages.
error cbuild: missing packs must be installed, rerun cbuild with the --packs option
```

Use the following command to manually install the missing pack(s).\
Note: replace `<vendor>`, `<package>` and `<version>` with the corresponding information from the error message.

```bash
cpackget pack add https://www.keil.com/pack/<vendor>.<package>.<version>.pack -a
```

## Version History

| Version | Date | Release notes |
|---      |---   |---            |
|  |  | Arm VSI demo for Arm Virtual Hardware. |

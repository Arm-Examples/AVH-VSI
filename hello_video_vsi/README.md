# Hello Video VSI Example

This example uses the [Virtual Streaming Interface (VSI)](https://arm-software.github.io/AVH/main/simulation/html/group__arm__vsi.html) to simulate an input from a peripheral video sensor.

Application firmware defines video input configuration in app.c file. The video vsi python scripts read the specified image/video file and send it to the application running on an [AVH FVP Model](https://arm-software.github.io/AVH/main/overview/html/index.html) where it gets displayed on the model LCD screen.

See [Video over VSI documentation](https://arm-software.github.io/AVH/main/simulation/html/group__arm__vsi__video.html) for more information about the implementation of video driver for AVH FVPs.

This is a minimal example of how to use Video over VSI, and you can modify the python script and the video driver in order to match your use case.

## Build and run

The example is provided in [csolution format](https://github.com/Open-CMSIS-Pack/cmsis-toolbox/blob/main/docs/YML-Input-Format.md) and can be build in an IDE, or in command line interface using *cbuild* utility from [CMSIS-Toolbox](https://github.com/Open-CMSIS-Pack/cmsis-toolbox/blob/main/docs/build-operation.md).

All applications have been ported for Corstone-310, Corstone-300 and can be compiled with Arm Compiler 6 (AC6) or GCC.

The path to the input image/video file is specified in the `app.c` file as `INPUT_IMAGE` and by default is `./samples/couple.bmp`. The path is relative to the current location and assumes the execution from the `./hello_video_vsi/` folder.

To display the content of the input file on the model's lcd, use `.lcd_out` as the build type, followed by the target platform. For example, to build and run on Corstone-310 with GCC toolchaing use:

Compile:

```bash
cbuild hello_video_vsi.csolution.yml --packs --rebuild --toolchain GCC --context .lcd_out+Corstone_310
```

Run the program in .elf format when compiled in GCC:

```bash
FVP_Corstone_SSE-310 -a ./out/hello_video_vsi/Corstone_310/lcd_out/GCC/hello_video_vsi.elf -C mps3_board.v_path=./source/vsi/video_vsi_py/
```

To compile with Arm Compiler use `--toolchain AC6` option.  With Arm Compiler an .axf file is generated and should be loaded in the same way as shown above.
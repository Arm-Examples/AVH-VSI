# Hello Video VSI Example

This example uses the VSI ([Virtual Streaming Interface](https://arm-software.github.io/AVH/main/simulation/html/group__arm__vsi.html)) to simulate an input from a peripheral video sensor. The video sensor will read the specified image/video file and send it to the application running on [AVH FVP Models](https://arm-software.github.io/AVH/main/overview/html/index.html) where it gets displayed on the mode LCD screen. The video peripheral is controlled by python scripts that reads the data from a local file. This is a minimal example of how to use Arm VSI, and you can modify the python script and the sensor driver in order to match your use case.

## Build and run

Use the cbuild tool or an IDE to build the application project in csolution format (See the project [README](../README.md)).

All applications have been ported for Corstone-300, Corstone-310 and can be compiled with Arm Compiler 6.

The path to the input image/video file is specified in the `app.c` file as `INPUT_IMAGE` and by default is `./samples/couple.bmp`. The path is relative to the current lcoation and assumes the execution from the `./hello_video_vsi` folder.

To display the content of the input file on the model's lcd, use `.lcd_out` as the build type, followed by the target platform. For example, to build and run on Corstone-300, use:

Compile: 

```bash
cbuild hello_video_vsi.csolution.yml -c .lcd_out+Corstone_300 --packs --update-rte
```

To run the program:

```bash
FVP_Corstone_SSE-300 -a ./out/hello_video_vsi/Corstone_300/lcd_out/hello_video_vsi.axf -C mps3_board.v_path=./source/vsi/video_vsi_py/
```

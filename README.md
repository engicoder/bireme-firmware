# Bireme Keyboard Firmware
This repository contains firwmare for the bireme keyboard and associate receiver. This firmware resides in three sub-directories:

1. qmk      | qmk portion of firmware for the ATmega32u4 the receiver
1. keyboard | wireless firmware for nRF51822 on the left and right halves of bireme keyboard
1. receiver | wireless firmware for the nRF51822 on the receiver

Precompile hex files are included for all if the above in the `precompiled-hex` directory. The files are:
* bireme_default.hex        | QMK firmware. Flash to ATmega32u4 on receiver
* bireme_keyboard_left.hex  | Flash to nRF51822 on left keyboard half.
* bireme_keyboard_right.hex | Flash to nRF51822 on right keyboard half.
* bireme_receiver.hex       | Flash to nRF51822 on the receiver.

This firmware is based on the mitosis firmware. The source code has been extensively modified and restructured.
https://github.com/reversebias/mitosis

Note: The instructions below assume a linux local machine

## Visual Studio Code support
Visual Studio Code workspace, task and debugging configuration is included in this repository for the wireless portion of the firmware. 
> Code should be launched using the `vscode.sh` script in the root directory. This script sets up the environment variables using the `setenv.sh` script and starts Code in that environment. 
> Debugging support uses the Cortex-Debug extension. https://marketplace.visualstudio.com/items?itemName=marus25.cortex-debug

## Building QMK firmware

This repository only includes the keyboard specific code for the QMK build of Bireme. This needs to be added to a full 
QMK source tree inorder to build. 

* Clone or download QMK repository from https://github.com/qmk/qmk_firmware.
* Copy the qmk/bireme folder to qmk_firmware/keyboards directory in the QML source tree.
* From the qmk_firmware root directory, build the Bireme firmware with the following command
    'make bireme:default'

Bireme used the default ATmega32u4 bootloader for flashing firmware. 'dfu-programmer` can be used to flash the device with the following commands:
'''
dfu-programmer.exe atmega32u4 erase
dfu-programmer.exe atmega32u4 flash bireme_default.hex
dfu-programmer.exe atmega32u4 reset
'''

## Building wireless firmware

### Dependencies

* ARM Embedded Toolchain
* Nordic nRF5 SDK 12.3.0
* Nordic nRF5 SDK 15.3.0
* Nordic command line tools
* SWD programmer software

#### Download Arm Embedded Toolchain

https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm/downloads

Extract to the local machine: (suggested location: /opt)

Note: This code was developed and tested using the 8-2019-q3-update version

#### Download Nordic SDK 

https://www.nordicsemi.com/Software-and-Tools/Software/nRF5-SDK/Download#infotabs

Download versions 12.3.0 and 15.3.0
Extract both to the same base directory (in the home directory, i.e. ~/nrf-sdk)

#### Download the Nordic command line tools

https://www.nordicsemi.com/Software-and-Tools/Development-Tools/nRF-Command-Line-Tools/Download#infotabs\

Extract to the local machine: (suggested location: /opt)
Note: Nordic has a Debian install package but it does not seem to update paths correctly.

#### SWD programmer software

The nRF modules must be programmed using the ARM SWD (Single wire debugging) interface. 
There are several devices available that can be used to flash the firmware via SWD including:

* Segger J-Link
* STmicroelextronics ST-Link
* Chinese clones of J-Link and ST-Link

### Path configuration

A few files need to be updated to change configuration paths to coincide with the locations on the machine.

#### Nordic SDK makefile.posix
This file is located in each of the Nordic SDK directories at 
```
/components/toolchain/gcc/Makefile.posix
```
Edit this file and change the `GNU_INSTALL_ROOT` define to match the directory where you extracted the ARM Embedded toolchain. For example

```
GNU_INSTALL_ROOT := opt/gcc-arm-none-eabi-8-2019-q3-update
```

#### setevn.sh script
This script sets up all environment variables needed by the Bireme makefile. It is located in the bireme root directory. Edit this file and update the variables to the paths on the local machine.

* `GNU_GCC`      | The path to the ARM embedded toolchain
* `NRF_SDK_KBD`  | path to the Nordic nRF5 SKD version 15.3.0 
* `NRF_SDK_RCVR` | path to the Nordic nRF5 SKD version 12.3.0 
* `NRF_TOOLS`    | path to the Nordic command line tools
* `SEGGER`       | path to Segger tools (for J-Link)


### Clone Bireme repository

Clone the bireme source repository to the local machine
```
git clone https://github.com/engicoder/bireme
```

### Command line build
From the bireme root directory:

```
. ./setenv.sh
make
```

### Visual Studio Code build
Launch Code using the `vscode.sh` script
```
./vscode.sh
```

To build everything, run the `build all` task by 
* Press `Ctrl+P` and type `task build all` followed by `Enter`
or 
* Select `Run Task` from the `Terminal` menu and the select `build all`




























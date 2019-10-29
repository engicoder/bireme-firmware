# Bireme Keyboard Firmware
This repository contains firwmare for the bireme keyboard and associate receiver. This firmware resides in three sub-directories:
<table>
    <tr>
        <td><code>keyboard</code></td>
        <td>Wireless firmware for nRF51822 on the left and right halves of bireme keyboard</td>
    </tr>
    <tr>
        <td><code>qmk</code></td>
        <td>QMK portion of firmware for the ATmega32u4 on the receiver</td>
    </tr>
    <tr>
        <td><code>receiver</code></td>
        <td>wireless firmware for the nRF51822 on the receiver</td>
    </tr>
</table>   

### Precompiled binaries
Precompile hex files for all firmware parts are included in the `precompiled-hex` directory. The files are:
<table>
    <tr>
        <td><code>bireme_default.hex</code></td>
        <td>QMK firmware. Flash to ATmega32u4 on receiver
    </tr>
    <tr>
        <td><code>bireme_keyboard_left.hex</code></td>
        <td>Flash to nRF51822 on left keyboard half.
    </tr>
    <tr>
        <td><code>bireme_keyboard_right.hex</code></td>
        <td>Flash to nRF51822 on right keyboard half.
    </tr>
    <tr>
        <td><code>bireme_receiver.hex</code></td>
        <td>Flash to nRF51822 on the receiver.
    </tr>
</table>   


This firmware is based on the mitosis firmware. https://github.com/reversebias/mitosis
The source code has been extensively modified and restructured.

> Note: The instructions give here assume a linux local machine.

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
### Flashing QMK firmware
Bireme uses the factory ATmega32u4 bootloader for flashing firmware. 'dfu-programmer` can be used to flash the device as follows:

Press the reset button on Bireme receiver. The receiver will reset and enter update mode. From command line, execute the following commands:
'''
dfu-programmer atmega32u4 erase
dfu-programmer atmega32u4 flash bireme_default.hex
dfu-programmer atmega32u4 reset
'''

## Building wireless firmware

### Dependencies

* ARM Embedded Toolchain
* Nordic nRF5 SDK 12.3.0
* Nordic nRF5 SDK 15.3.0

#### Download Arm Embedded Toolchain
https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm/downloads

Extract to the local machine: (suggested location: /opt)
Note: This code was developed and tested using the 8-2019-q3-update version

#### Download Nordic SDK 
https://www.nordicsemi.com/Software-and-Tools/Software/nRF5-SDK/Download#infotabs

Download SDK versions 12.3.0 and 15.3.0
Extract both to the same base directory (in the home directory, i.e. ~/nrf-sdk)

### Path configuration
A few files need to be updated to change configuration paths to coincide with the locations on the machine.

#### Nordic SDK makefile.posix
This file is located in each of the Nordic SDK directories at 
```
<nrf-sdk-path>/components/toolchain/gcc/Makefile.posix
```
Edit this file and change the `GNU_INSTALL_ROOT` define to match the directory where you extracted the ARM Embedded toolchain. For example

```
GNU_INSTALL_ROOT := opt/gcc-arm-none-eabi-8-2019-q3-update
```

#### Environment script `setevn.sh`
The `setenv.sh` script creates the environment variables needed by the Bireme build. The script is located in the bireme root directory. Edit this file and update the variables to the appropriate paths on the local machine.

 Variable|Usage
--------------|-------------------------------------------
`GCC_ARM`|path to the ARM embedded toolchain
`NRF_SDK_KBD`|path to Nordic nRF5 SDK version 15.3.0 
`NRF_SDK_RCVR`|path to Nordic nRF5 SDK version 12.3.0 
`NRF_TOOLS`|path to Nordic command line tools

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
Launch Code using the `vscode.sh` script from the root directory of the bireme repository
```
./vscode.sh
```

To build everything, run the `build all` task. There are two ways to do this in Visual Studio Code
1. Press `Ctrl+P` and type `task build all` followed by `Enter`
1. Select `Run Task` from the `Terminal` menu and the select `build all`


## Flashing and Debugging firmware
The nRF modules can be programmed using the ARM SWD (Single wire debugging) interface. OpenOCD is detailed here but other tools are available such as those from SEGGER.

### OpenOCD installation
OpenOCD supports flashing/programming and debugging of both the receiver and keyboard using either J-Link or ST-Link. The current release version of OpenOCD (0.10.0) does not include support for the nRF52840 device used by the keyboard halves, therefore a version using the latest updates must be built/installed. 

There are a few dependencies needed to ensure that OpenOCD can function properly. Install these packages

```
sudo apt-get install make libtool pkg-config autoconf automake texinfo libusb-1.0-0-dev
```

Clone the OpenOCD source code repository from the Github mirror
```
git clone --recursive https://github.com/ntfreak/openocd
```

From the OpenOCD repository root directory, execute the build
```    
./bootstrap
./configure
make
```

Install the build version of OpenOCD. (will be installed to `/usr/local/bin`)
```
sudo make install
```


### udev device rules configuration
The latest udev device rules for OpenOCD compatible devices need to be installed for OpenOCD to access the device. 
The rules file can be found in the Bireme root directory and can be installed as follows:
Copy the rules file:
```
sudo cp utils/60-openocd.rules /etc/udev/rule.d/
```
Add the current user to the plugdev group so that OpenOCD does not have to run as root. Note: The rules file gives access the OpenOCD compatible device to the 'plugdev' group.
```
sudo useradd -G plugdev $(whoami)
```
Reload the rules:
```
sudo udevadm control --reload-rules
```
Restart udev
```
sudo udevadm trigger
```

### Hardware programmer/debuggers
There are several devices available that can be used to program and debug the firmware via SWD including:

* Segger J-Link
* STmicroelextronics ST-Link
* Chinese clones of J-Link or ST-Link
Note: I have not tested any of the FTDI based devices

## Flashing wireless firmware
The firmware can be flashed using the OpenOCD server with either J-Link or ST-Link, or with the Nordic `nrfjprog` tool on J-link. The Nordic tools do not support ST-Link.

### Flashing firmware with OpenOCD

OpenOCD runs a server that exposes a telnet port that can be used for commands.

Start the OpenOCD server by specifying the configuration file with the '-f' flag.
```
openocd -f <config-file>
```
where `<config-file>` is:
* `nrf51-stlink.cfg` for receiver 
* `nrf52-stlink.cfg` for keyboard

Once the OpenOCD server is running, you can send commands via a telnet session:
```
telnet localhost 4444
```
To program the device, issue the following commands via the telnet session:
```
> reset halt
> program `<firmware-file>` verify
> reset
```
where `<firmware-file>` is the full path to the firmware .hex file. See [Precompiled binaries](#precompiled-binaries) for list of hex files and usage. Locally built .hex files can be found in:
* `receiver/_build` for the receiver
* `keyboard/_build` for the keyboard


### Optional tools and software

#### ST-Link
ST-Link utilities to query information on ST-Link devices can be found at:
https://github.com/texane/stlink
Note: For Debian based linux distros such as Ubuntu you will need to build the utilities from source.

#### J-Link 
The latest J-Link software is recommended and can be downloaded from:
https://www.segger.com/downloads/jlink/#J-LinkSoftwareAndDocumentationPack

#### Nordic command line tools
The Nordic command line tools can be used to program the devices if you are using J-Link. This is optional.
The tools can be download from:
https://www.nordicsemi.com/Software-and-Tools/Development-Tools/nRF-Command-Line-Tools/Download#infotabs\

Extract to the local machine: (suggested location: /opt)
Note: Nordic has a Debian install package but it does not seem to update paths correctly.








# Wi-SUN AT command firmware
AT command set firmware for Texas Instruments CC1352P7

## Features
* Runs on CC1352P7 LaunchPad™ development kit
* Wi-SUN via AT commands over UART
* TCP and UDP IPv6 sockets with flow control
* Supports off-chip over-the-air updates using TI's CoAP OAD protocol
* Based on SimpleLink™ ns_coap_node example
* Based on [cAT](https://github.com/marcinbor85/cAT) for AT command parsing

## Requirements
1. [TI Arm Clang Compiler Tools](https://www.ti.com/tool/download/ARM-CGT-CLANG)
2. [SimpleLink™ CC13xx and CC26xx software development kit (SDK)](https://www.ti.com/tool/download/SIMPLELINK-LOWPOWER-F2-SDK)

## Compile
1. Update `export.sh` with installation paths and run it to set environment variables
2. From the `Release/` directory, run
    > `make sysconfig-[region]` (replace [region] with the actual region to build) \
    > `make -j4 all`

## Install
The default configuration of this repository requires the MCUBoot bootloader from the SimpleLink™ SDK to
be installed on the device and run the application. Flash `wisun_iot_modem.bin` to address 0x0.

To run without a bootloader, change `MCUBOOT_HEAD_SIZE` and `ENTRY_SIZE` to `0x0` in `cc13x2x7_cc26x2x7.cmd`
and flash `wisun_iot_modem-noheader.bin` (or `wisun_iot_modem.out` directly) to address 0x0.

## Use
UART pins are configured in `wisun_*.syscfg`. Default configuration uses pins
DIO16 for TX and DIO17 for RX at a rate of 115200.

Check `static struct cat_command cmds[]` in `application.c` for a list of available AT commands.

## Docker
The `Dockerfile` allows to create a Docker image with a build environment.

Note: For Mac M1 add `--platform linux/x86_64` to the below docker instructions.

- In the project root dir, build the image
>`docker build -t wisun-dev .`
- Run the container. Remeber to change the volume path to your dev dir
>`docker run -it --name wisun -v /path/to/codebase:/data wisun-dev`
- Stop the container
>`docker stop wisun`
- Start the container
>`docker start -i wisun`
- Remove the container
>`docker rm wisun`

## Code formatter
- `clang-format` version >='14.0.0' to support the Macro whitespaces implemented in the included libs from SimpleLink SDK

- You can run to fix format the changed files
> `./format.sh`

Or you can download the 'Clang-Format' VSCode extension which creates a keyboad shortcut to format the current file:

Mac `shift + option + f` Windows `shift + alt + f` Linux `ctl + shift + i`

https://marketplace.visualstudio.com/items?itemName=xaver.clang-format

- To check the format without fixing the format for any files, you can run
> `python3 .clang-format.py -r ./ -e "./Release/syscfg/*"`

## Authors and Contributors ##

The following people have significantly contributed to the design
and development of `Wi-SUN AT command firmware`:
Okra Solar Pty Ltd.

Authors
 *  Georg Lippitsch
 *  Amr Ayoub
 *  Landry Leblanc

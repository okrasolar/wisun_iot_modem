FROM ubuntu:22.04

# Install dependencies
RUN apt-get update && apt-get -y install make wget python3 python3-pip python3-venv python3-dev \
    libc6-i386 libusb-0.1-4 libgconf-2-4 libncurses5 libpython2.7 libtinfo5 clang-format

# Get the build tools
RUN wget https://dr-download.ti.com/software-development/ide-configuration-compiler-or-debugger/MD-ayxs93eZNN/3.2.2.LTS/ti_cgt_armllvm_3.2.2.LTS_linux-x64_installer.bin && \
    chmod +x ti_cgt_armllvm_3.2.2.LTS_linux-x64_installer.bin && \
    ./ti_cgt_armllvm_3.2.2.LTS_linux-x64_installer.bin --mode unattended --prefix . && \
    rm ti_cgt_armllvm_3.2.2.LTS_linux-x64_installer.bin

# Get the SDK
RUN wget --no-verbose --show-progress --progress=dot:mega https://dr-download.ti.com/software-development/software-development-kit-sdk/MD-BPlR3djvTV/7.40.00.77/simplelink_cc13xx_cc26xx_sdk_7_40_00_77.run && \
    chmod +x simplelink_cc13xx_cc26xx_sdk_7_40_00_77.run && \
    ./simplelink_cc13xx_cc26xx_sdk_7_40_00_77.run --mode unattended --prefix . && \
    rm simplelink_cc13xx_cc26xx_sdk_7_40_00_77.run && \
    rm -r xdctools_3_62_01_15_core

# Update path
ENV PATH="/ti-cgt-armllvm_3.2.2.LTS/bin:${PATH}"
ENV PATH="/ti-cgt-armllvm_3.2.2.LTS/lib:${PATH}"

WORKDIR /data

ENTRYPOINT bash


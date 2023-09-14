FROM ubuntu:18.04

# Install dependencies
RUN apt-get update && apt-get -y install make wget python3 python3-pip python3-venv python3-dev \
    libc6-i386 libusb-0.1-4 libgconf-2-4 libncurses5 libpython2.7 libtinfo5

# Get the build tools
RUN wget https://dr-download.ti.com/software-development/ide-configuration-compiler-or-debugger/MD-ayxs93eZNN/3.0.0.STS/ti_cgt_armllvm_3.0.0.STS_linux-x64_installer.bin && \
    chmod +x ti_cgt_armllvm_3.0.0.STS_linux-x64_installer.bin && \
    ./ti_cgt_armllvm_3.0.0.STS_linux-x64_installer.bin --mode unattended --prefix . && \
    rm ti_cgt_armllvm_3.0.0.STS_linux-x64_installer.bin

# Get the SDK
RUN wget --no-verbose --show-progress --progress=dot:mega https://dr-download.ti.com/software-development/software-development-kit-sdk/MD-BPlR3djvTV/7.10.00.98/simplelink_cc13xx_cc26xx_sdk_7_10_00_98.run && \
    chmod +x simplelink_cc13xx_cc26xx_sdk_7_10_00_98.run && \
    ./simplelink_cc13xx_cc26xx_sdk_7_10_00_98.run --mode unattended --prefix . && \
    rm simplelink_cc13xx_cc26xx_sdk_7_10_00_98.run && \
    rm -r xdctools_3_62_01_15_core

RUN pip3 install cmake==3.16.6 scikit-build

RUN pip3 install clang-format==16.0.3

# Update path
ENV PATH="/ti-cgt-armllvm_3.0.0.STS/bin:${PATH}"
ENV PATH="/ti-cgt-armllvm_3.0.0.STS/lib:${PATH}"

WORKDIR /data

ENTRYPOINT bash


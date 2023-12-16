#!/bin/bash

if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <region>"
    exit 1
fi

set -e

whoami
pwd

export TI_TOOLS_ROOT="/ti-cgt-armllvm_3.2.0.LTS"
export TI_LIB="/simplelink_cc13xx_cc26xx_sdk_7_10_01_24"
export TI_SYSCONFIG="/sysconfig_1.16.2"
export TI_PROJECT_FOLDER_MODEM=$(pwd)

cd Release/
make clean
make sysconfig-$1
make all

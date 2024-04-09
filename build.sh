#!/bin/bash

if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <region>"
    exit 1
fi

set -e

whoami
pwd

export TI_TOOLS_ROOT="/ti-cgt-armllvm_3.2.2.LTS"
export TI_LIB="/simplelink_cc13xx_cc26xx_sdk_7_40_00_77"
export TI_SYSCONFIG="/sysconfig_1.18.1"
export TI_PROJECT_FOLDER_MODEM=$(pwd)
export WISUN_REGION=$1

cd Release/
make clean
make

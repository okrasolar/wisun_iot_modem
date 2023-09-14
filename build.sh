#!/bin/bash

set -e

whoami
pwd

export TI_TOOLS_ROOT="/ti-cgt-armllvm_3.0.0.STS"
export TI_LIB="/simplelink_cc13xx_cc26xx_sdk_7_10_00_98"
export TI_SYSCONFIG="/sysconfig_1_15_0"
export TI_PROJECT_FOLDER_MODEM=$(pwd)

# Replace vendor specific config
sed -i \
    -e s/0xABCD/$PAN_ID/g \
    -e s/"Wi-SUN Network"/"$NETWORK_NAME"/g \
    -e s/123456789ABCDEF00000000000000000/$NETWORK_KEY/g \
    ti_wisunfan_coap_node.syscfg

cd Release/
make clean
make all

################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
mbed/mbedtls2tirtos/platform/%.o: ../mbed/mbedtls2tirtos/platform/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"$(CG_TOOL_ROOT)/bin/tiarmclang" -c @"$(CG_PROJECT_FOLDER)/application/defines/router.opts" @"$(CG_PROJECT_FOLDER)/Release/syscfg/ti_wisunfan_config.opts"  -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16 -mlittle-endian -mthumb -Oz -I"$(CG_PROJECT_FOLDER)" -I"$(CG_PROJECT_FOLDER)/Release" -I"$(CG_DRIVER_LIB)/source/ti/ti_wisunfan/wisunfan_mac/low_level/cc13xx/combo" -I"$(CG_DRIVER_LIB)/source/ti/ti_wisunfan/mbed_port/ti-rf-driver/ti-rf-driver" -I"$(CG_DRIVER_LIB)/source/ti/ti_wisunfan/apps/udp_nanostack_mesh_tirf" -I"$(CG_DRIVER_LIB)/source/ti/ti_wisunfan/apps/common/include" -I"$(CG_DRIVER_LIB)/source/ti/common/nv" -I"$(CG_DRIVER_LIB)/source/ti/ti_wisunfan/mbed_config/ws_router" -I"$(CG_DRIVER_LIB)/source/ti/ti_wisunfan/mbed_port/ti-rf-driver/source" -I"$(CG_DRIVER_LIB)/source/ti/ti_wisunfan/mbed_port/mbednanostack2tirtos" -I"$(CG_DRIVER_LIB)/source/ti/ti_wisunfan/mbed_port/mbednanostack2tirtos/platform" -I"$(CG_DRIVER_LIB)/source/ti/ti_wisunfan/mbed_port/mbedtls2tirtos" -I"$(CG_DRIVER_LIB)/source/ti/ti_wisunfan/mbed_port/mbedtls2tirtos/platform" -I"$(CG_DRIVER_LIB)/source/ti/ti_wisunfan/mbed_port/mbedtls2tirtos/mbedtls/mbed-crypto" -I"$(CG_PROJECT_FOLDER)/mbed/frameworks/nanostack-libservice/mbed-client-libservice" -I"$(CG_PROJECT_FOLDER)/mbed/frameworks/nanostack-libservice" -I"$(CG_PROJECT_FOLDER)/mbed/frameworks/nanostack-libservice/mbed-client-libservice/platform" -I"$(CG_PROJECT_FOLDER)/mbed/frameworks/mbed-client-randlib/mbed-client-randlib/platform" -I"$(CG_PROJECT_FOLDER)/mbed/frameworks/mbed-client-randlib/mbed-client-randlib" -I"$(CG_PROJECT_FOLDER)/mbed/frameworks/mbed-client-randlib" -I"$(CG_PROJECT_FOLDER)/mbed/frameworks/mbed-trace" -I"$(CG_PROJECT_FOLDER)/mbed/frameworks/mbed-coap" -I"$(CG_PROJECT_FOLDER)/mbed/frameworks/mbed-coap/source/include" -I"$(CG_PROJECT_FOLDER)/mbed/nanostack/sal-stack-nanostack" -I"$(CG_PROJECT_FOLDER)/mbed/nanostack/sal-stack-nanostack/source" -I"$(CG_PROJECT_FOLDER)/mbed/nanostack/sal-stack-nanostack/nanostack" -I"$(CG_PROJECT_FOLDER)/mbed/nanostack/sal-stack-nanostack/source/Service_Libs/mdns" -I"$(CG_PROJECT_FOLDER)/mbed/nanostack/sal-stack-nanostack/source/Service_Libs/mdns/fnet" -I"$(CG_PROJECT_FOLDER)/mbed/nanostack/sal-stack-nanostack/source/Service_Libs/mdns/fnet/fnet_stack" -I"$(CG_PROJECT_FOLDER)/mbed/nanostack/sal-stack-nanostack/source/Service_Libs/mdns/fnet/fnet_stack/stack" -I"$(CG_PROJECT_FOLDER)/mbed/nanostack/sal-stack-nanostack/source/Service_Libs/mdns/fnet/fnet_stack/services/serial" -I"$(CG_PROJECT_FOLDER)/mbed/nanostack/sal-stack-nanostack/source/Core/include" -I"$(CG_PROJECT_FOLDER)/mbed/nanostack/sal-stack-nanostack/source/6LowPAN" -I"$(CG_PROJECT_FOLDER)/mbed/nanostack/sal-stack-nanostack-eventloop" -I"$(CG_PROJECT_FOLDER)/mbed/nanostack/sal-stack-nanostack-eventloop/nanostack-event-loop" -I"$(CG_PROJECT_FOLDER)/mbed/nanostack/sal-stack-nanostack-eventloop/nanostack-event-loop/platform" -I"$(CG_PROJECT_FOLDER)/mbed/nanostack/nanostack-hal-mbed-cmsis-rtos" -I"$(CG_PROJECT_FOLDER)/mbed/nanostack/mbed-mesh-api/source/include" -I"$(CG_PROJECT_FOLDER)/mbed/nanostack/mbed-mesh-api/mbed-mesh-api" -I"$(CG_PROJECT_FOLDER)/mbed/nanostack/mbed-mesh-api" -I"$(CG_PROJECT_FOLDER)/mbed/nanostack/coap-service/coap-service" -I"$(CG_PROJECT_FOLDER)/mbed/nanostack/coap-service/source/include" -I"$(CG_DRIVER_LIB)/source/third_party/ti_wisunfan/mbedtls" -I"$(CG_DRIVER_LIB)/source/third_party/ti_wisunfan/mbedtls/platform/inc" -I"$(CG_DRIVER_LIB)/source/third_party/ti_wisunfan/mbedtls/inc" -I"$(CG_DRIVER_LIB)/source/third_party/ti_wisunfan/mbedtls/platform/COMPONENT_PSA_SRV_IMPL/COMPONENT_NSPE" -I"$(CG_DRIVER_LIB)/source/ti/ti_wisunfan" -I"$(CG_DRIVER_LIB)/source/ti/ti_wisunfan/shell" -I"$(CG_DRIVER_LIB)/source/ti/ti_wisunfan/config" -I"$(CG_DRIVER_LIB)/source/ti/ti_wisunfan/uip" -I"$(CG_DRIVER_LIB)/source/ti/ti_wisunfan/uip/conf" -I"$(CG_DRIVER_LIB)/source/ti/ti_wisunfan/uip/inc" -I"$(CG_DRIVER_LIB)/source/ti/ti_wisunfan/uip/pltfrm/sysbios" -I"$(CG_DRIVER_LIB)/source/ti/ti_wisunfan/uip/net" -I"$(CG_DRIVER_LIB)/source/ti/ti_wisunfan/wisunfan_mac/services" -I"$(CG_DRIVER_LIB)/source/ti/ti_wisunfan/wisunfan_mac/hal/assert" -I"$(CG_DRIVER_LIB)/source/ti/ti_wisunfan/wisunfan_mac/hal/appasrt" -I"$(CG_DRIVER_LIB)/source/ti/ti_wisunfan/wisunfan_mac/hal/crypto" -I"$(CG_DRIVER_LIB)/source/ti/ti_wisunfan/wisunfan_mac/hal/platform" -I"$(CG_DRIVER_LIB)/source/ti/ti_wisunfan/wisunfan_mac/hal/rf" -I"$(CG_DRIVER_LIB)/source/ti/ti_wisunfan/wisunfan_mac/high_level" -I"$(CG_DRIVER_LIB)/source/ti/ti_wisunfan/wisunfan_mac/low_level" -I"$(CG_DRIVER_LIB)/source/ti/ti_wisunfan/wisunfan_mac/common/osal_port" -I"$(CG_DRIVER_LIB)/source/ti/ti_wisunfan/wisunfan_mac/common" -I"$(CG_DRIVER_LIB)/source/ti/ti_wisunfan/wisunfan_mac/common/boards" -I"$(CG_DRIVER_LIB)/source/ti/ti_wisunfan/wisunfan_mac/common/util" -I"$(CG_DRIVER_LIB)/source/ti/ti_wisunfan/wisunfan_mac/common/inc" -I"$(CG_DRIVER_LIB)/source/ti/ti_wisunfan/wisunfan_mac/common/stack/src" -I"$(CG_DRIVER_LIB)/source/ti/ti_wisunfan/wisunfan_mac/common/stack/tirtos/inc" -I"$(CG_DRIVER_LIB)/source/ti/ti_wisunfan/wisunfan_mac/common/heapmgr" -I"$(CG_DRIVER_LIB)/source/ti/ti_wisunfan/wisunfan_mac/services/saddr" -I"$(CG_DRIVER_LIB)/source/ti/ti_wisunfan/wisunfan_mac/services/sdata" -I"$(CG_DRIVER_LIB)/source/ti/ti_wisunfan/wisunfan_mac/fh" -I"$(CG_DRIVER_LIB)/source/ti/ti_wisunfan/wisunfan_mac/inc" -I"$(CG_DRIVER_LIB)/source/ti/ti_wisunfan/wisunfan_mac/rom" -I"$(CG_DRIVER_LIB)/source/ti/ti_wisunfan/wisunfan_mac/inc/cc13xx" -I"$(CG_DRIVER_LIB)/source/ti/ti_wisunfan/wisunfan_mac/low_level/cc13xx" -I"$(CG_DRIVER_LIB)/source/ti/ti_wisunfan/wisunfan_mac/tracer" -I"$(CG_PROJECT_FOLDER)/mbed/nanostack/mbed-mesh-api/source" -I"$(CG_PROJECT_FOLDER)/mbed/nanostack/sal-stack-nanostack/source/6LoWPAN/MAC" -I"$(CG_PROJECT_FOLDER)/mbed/nanostack/sal-stack-nanostack/source/6LoWPAN/Thread" -I"$(CG_PROJECT_FOLDER)/mbed/nanostack/sal-stack-nanostack/source/Security/PANA" -I"$(CG_PROJECT_FOLDER)/mbed/nanostack/sal-stack-nanostack/source/Service_Libs/mdns/fnet/fnet_stack/services/mdns" -I"$(CG_PROJECT_FOLDER)/mbed/nanostack/sal-stack-nanostack/source/Service_Libs/fnv_hash" -I"$(CG_PROJECT_FOLDER)/mbed/nanostack/sal-stack-nanostack/source/Service_Libs/hmac" -I"$(CG_PROJECT_FOLDER)/mbed/nanostack/sal-stack-nanostack/source/Service_Libs/load_balance" -I"$(CG_PROJECT_FOLDER)/mbed/nanostack/sal-stack-nanostack/source/libNET/src" -I"$(CG_PROJECT_FOLDER)/mbed/nanostack/sal-stack-nanostack-eventloop/source" -I"$(CG_DRIVER_LIB)/source/third_party/ti_wisunfan/mbedtls/platform/COMPONENT_PSA_SRV_IMPL" -I"$(CG_PROJECT_FOLDER)/mbed/nanostack/sal-stack-nanostack/source/Service_Libs/whiteboard" -I"$(CG_PROJECT_FOLDER)/mbed/nanostack/sal-stack-nanostack/source/6LoWPAN/ws" -I"$(CG_PROJECT_FOLDER)/mbed/nanostack/sal-stack-nanostack/source/DHCPv6_client" -I"$(CG_PROJECT_FOLDER)/mbed/nanostack/sal-stack-nanostack/source/libDHCPv6" -I"$(CG_PROJECT_FOLDER)/mbed/nanostack/sal-stack-nanostack/source/Service_Libs/utils" -I"$(CG_PROJECT_FOLDER)/mbed/nanostack/sal-stack-nanostack/source/Service_Libs/pan_blacklist" -I"$(CG_PROJECT_FOLDER)/mbed/nanostack/sal-stack-nanostack/source/Common_Protocols" -I"$(CG_DRIVER_LIB)/source/ti/posix/ticlang" -I"$(CG_DRIVER_LIB)/source/ti/ti_wisunfan/ncp_interface/config" -I"$(CG_DRIVER_LIB)/source/ti/ti_wisunfan/ncp_interface/include" -I"$(CG_DRIVER_LIB)/source/ti/ti_wisunfan/ncp_interface/src" -I"$(CG_DRIVER_LIB)/source/ti/ti_wisunfan/ncp_interface/src/core" -I"$(CG_DRIVER_LIB)/source/ti/ti_wisunfan/ncp_interface/src/core/utils" -I"$(CG_DRIVER_LIB)/source/ti/ti_wisunfan/ncp_interface/examples/platforms" -I"$(CG_DRIVER_LIB)/source/ti/ti_wisunfan/ncp_interface/examples/ncp_ftd" -I"$(CG_DRIVER_LIB)/source/ti/ti_wisunfan/ncp_interface/examples/ncp_ftd/platform" -I"$(CG_DRIVER_LIB)/source/ti/ti_wisunfan/ncp_interface/src/core/thread" -I"$(CG_DRIVER_LIB)/source/ti/ti_wisunfan/ncp_interface/src/ncp" -I"$(CG_DRIVER_LIB)/source/ti/ti_wisunfan/ncp_interface/examples/ncp_ftd/platform/nv" -I"$(CG_DRIVER_LIB)/source" -I"$(CG_DRIVER_LIB)/source/ti/devices/cc13x2x7_cc26x2x7" -I"$(CG_DRIVER_LIB)/source/ti/devices/cc13x2x7_cc26x2x7/inc" -I"$(CG_DRIVER_LIB)/source/ti/devices/cc13x2x7_cc26x2x7/driverlib" -I"$(CG_DRIVER_LIB)/kernel/tirtos7/packages" -DCOAP_SERVICE_ENABLE -DxCOAP_PANID_LIST -DEXCLUDE_TRACE -DxSWITCH_NCP_TO_TRACE -DTLS_SEC_PROT_LIB_USE_MBEDTLS_PLATFORM_MEMORY -DxWISUN_AUTO_START -DTI_WISUN_FAN_DEBUG -DBoard_EXCLUDE_NVS_EXTERNAL_FLASH -DDeviceFamily_CC13X2X7 -gdwarf-3 -fdiagnostics-show-option -fshort-enums -munaligned-access -funsigned-char -fcommon -ffunction-sections -fno-strict-aliasing -include mbed_config.h -march=armv7e-m -MMD -MP -MF"mbed/mbedtls2tirtos/platform/$(basename $(<F)).d_raw" -MT"$(@)" -std=gnu9x $(GEN_OPTS__FLAG) -o"$@" "$(shell echo $<)"
	@echo 'Finished building: "$<"'
	@echo ' '



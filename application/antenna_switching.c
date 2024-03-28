#include <stddef.h>
#include <stdint.h>

#ifndef DeviceFamily_CC13X2X7
#define DeviceFamily_CC13X2X7
#endif

#include <ti/devices/DeviceFamily.h>

#include "ti_drivers_config.h"

#include <ti/drivers/GPIO.h>
#include DeviceFamily_constructPath(driverlib/ioc.h)
#include <ti/drivers/rf/RF.h>

/*
 * ======== Antenna switching ========
 */
/*
 * ======== rfDriverCallbackAntennaSwitching ========
 * Sets up the antenna switch depending on the current PHY configuration.
 */
void rfDriverCallbackAntennaSwitching(RF_Handle client, RF_GlobalEvent events, void* arg)
{

    if (events & RF_GlobalEventRadioSetup) {
        bool sub1GHz = false;
        uint8_t loDivider = 0;

        /* Switch off all paths. */
        GPIO_write(CONFIG_RF_24GHZ, 0);
        GPIO_write(CONFIG_RF_HIGH_PA, 0);
        GPIO_write(CONFIG_RF_SUB1GHZ, 0);

        /* Decode the current PA configuration. */
        RF_TxPowerTable_PAType paType = (RF_TxPowerTable_PAType)RF_getTxPower(client).paType;

        /* Decode the generic argument as a setup command. */
        RF_RadioSetup* setupCommand = (RF_RadioSetup*)arg;

        switch (setupCommand->common.commandNo) {
            case (CMD_RADIO_SETUP):
            case (CMD_BLE5_RADIO_SETUP):
                loDivider = RF_LODIVIDER_MASK & setupCommand->common.loDivider;

                /* Sub-1GHz front-end. */
                if (loDivider != 0) {
                    sub1GHz = true;
                }
                break;
            case (CMD_PROP_RADIO_DIV_SETUP):
                loDivider = RF_LODIVIDER_MASK & setupCommand->prop_div.loDivider;

                /* Sub-1GHz front-end. */
                if (loDivider != 0) {
                    sub1GHz = true;
                }
                break;
            default:
                break;
        }

        if (sub1GHz) {
            /* Sub-1 GHz */
            if (paType == RF_TxPowerTable_HighPA) {
                /* PA enable --> HIGH PA
                 * LNA enable --> Sub-1 GHz
                 */
                GPIO_setConfigAndMux(CONFIG_RF_24GHZ, GPIO_CFG_OUTPUT, IOC_PORT_GPIO);
                /* Note: RFC_GPO3 is a work-around because the RFC_GPO1 (PA enable signal) is sometimes not
                         de-asserted on CC1352 Rev A. */
                GPIO_setConfigAndMux(CONFIG_RF_HIGH_PA, GPIO_CFG_OUTPUT, IOC_PORT_RFC_GPO3);
                GPIO_setConfigAndMux(CONFIG_RF_SUB1GHZ, GPIO_CFG_OUTPUT, IOC_PORT_RFC_GPO0);
            } else {
                /* RF core active --> Sub-1 GHz */
                GPIO_setConfigAndMux(CONFIG_RF_24GHZ, GPIO_CFG_OUTPUT, IOC_PORT_GPIO);
                GPIO_setConfigAndMux(CONFIG_RF_HIGH_PA, GPIO_CFG_OUTPUT, IOC_PORT_GPIO);
                GPIO_setConfigAndMux(CONFIG_RF_SUB1GHZ, GPIO_CFG_OUTPUT | GPIO_CFG_OUT_HIGH, IOC_PORT_GPIO);
            }
        } else {
            /* 2.4 GHz */
            if (paType == RF_TxPowerTable_HighPA) {
                /* PA enable --> HIGH PA
                 * LNA enable --> 2.4 GHz
                 */
                GPIO_setConfigAndMux(CONFIG_RF_24GHZ, GPIO_CFG_OUTPUT, IOC_PORT_RFC_GPO0);
                /* Note: RFC_GPO3 is a work-around because the RFC_GPO1 (PA enable signal) is sometimes not
                         de-asserted on CC1352 Rev A. */
                GPIO_setConfigAndMux(CONFIG_RF_HIGH_PA, GPIO_CFG_OUTPUT, IOC_PORT_RFC_GPO3);
                GPIO_setConfigAndMux(CONFIG_RF_SUB1GHZ, GPIO_CFG_OUTPUT, IOC_PORT_GPIO);
            } else {
                /* RF core active --> 2.4 GHz */
                GPIO_setConfigAndMux(CONFIG_RF_24GHZ, GPIO_CFG_OUTPUT | GPIO_CFG_OUT_HIGH, IOC_PORT_GPIO);
                GPIO_setConfigAndMux(CONFIG_RF_HIGH_PA, GPIO_CFG_OUTPUT, IOC_PORT_GPIO);
                GPIO_setConfigAndMux(CONFIG_RF_SUB1GHZ, GPIO_CFG_OUTPUT, IOC_PORT_GPIO);
            }
        }
    } else if (events & RF_GlobalEventRadioPowerDown) {
        /* Switch off all paths. */
        GPIO_write(CONFIG_RF_24GHZ, 0);
        GPIO_write(CONFIG_RF_HIGH_PA, 0);
        GPIO_write(CONFIG_RF_SUB1GHZ, 0);

        /* Reset the IO multiplexer to GPIO functionality */
        GPIO_setConfigAndMux(CONFIG_RF_24GHZ, GPIO_CFG_OUTPUT, IOC_PORT_GPIO);
        GPIO_setConfigAndMux(CONFIG_RF_HIGH_PA, GPIO_CFG_OUTPUT, IOC_PORT_GPIO);
        GPIO_setConfigAndMux(CONFIG_RF_SUB1GHZ, GPIO_CFG_OUTPUT, IOC_PORT_GPIO);
    }
}

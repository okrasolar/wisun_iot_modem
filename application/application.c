/*
MIT License

Copyright (c) 2023 Okra Solar Pty Ltd.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

/*
 *  ======== application.c ========
 */

#ifndef WISUN_NCP_ENABLE
#undef EXCLUDE_TRACE
#endif
#include "mbed_config_app.h"
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

/* Driver Header files */

#include <ti/drivers/GPIO.h>
#include <ti/drivers/SPI.h>
#include <ti/drivers/UART2.h>
#include <ti/drivers/dpl/ClockP.h>

/* Driver configuration */
#include "syscfg/ti_drivers_config.h"
#include "syscfg/ti_wisunfan_config.h"

#include "lib/driverlib/sys_ctrl.h"

#include "nsconfig.h"
#include "mesh_system.h"
#include "socket_api.h"
#include "ip6string.h"
#include "net_interface.h"
#include "wisun_tasklet.h"
#include "ns_trace.h"
#include "fhss_config.h"
#include "randLIB.h"
#include "ws_management_api.h"

#include "6LoWPAN/ws/ws_common_defines.h"
#include "Common_Protocols/ipv6_constants.h"
#include "6LoWPAN/ws/ws_common.h"

#include "Core/include/ns_address_internal.h"
#include "NWK_INTERFACE/Include/protocol_abstract.h"
#include "NWK_INTERFACE/Include/protocol.h"
#include "ws_bootstrap.h"

#include "nsdynmemLIB.h"

#include "net_rpl.h"
#include "RPL/rpl_protocol.h"
#include "RPL/rpl_policy.h"
#include "RPL/rpl_control.h"
#include "RPL/rpl_objective.h"
#include "RPL/rpl_upward.h"
#include "RPL/rpl_downward.h"
#include "RPL/rpl_structures.h"

#ifdef COAP_SERVICE_ENABLE
#include "coap_service_api.h"
#include "oad.h"
#endif

#include "cat.h"

#include "application.h"

/******************************************************************************
Defines & enums
 *****************************************************************************/
#define TRACE_GROUP "main"

#define NET_BUF_SIZE 1220
#define MASTER_GROUP 0
#define MY_GROUP 1
#define NOT_INITIALIZED -1

typedef enum connection_status {
    CON_STATUS_LOCAL_UP = 0,     /*!< local IP address set */
    CON_STATUS_GLOBAL_UP = 1,    /*!< global IP address set */
    CON_STATUS_DISCONNECTED = 2, /*!< no connection to network */
    CON_STATUS_CONNECTING = 3,   /*!< connecting to network */
    CON_STATUS_ERROR_UNSUPPORTED = 4
} connection_status_t;

/******************************************************************************
 Static & Global Variables
 *****************************************************************************/
static connection_status_t _connect_status;
static volatile bool uartCharReceived = false;
static volatile bool connectedFlg = false;
static volatile bool ipConnectedFlg = false;
static volatile bool tcpConnectProblem = false;
static volatile bool txPendingFlg = false;
static char uartChar;
static uint16_t ipd_data_length = 0;
static int8_t interface_id = NOT_INITIALIZED;
static bool _blocking = false;
static bool _configured = false;
static bool _isTcp;
static ns_address_t peer_addr = { 0 };

static UART2_Handle uart;

struct cat_object at;

static int8_t socket_id = -1;

static uint8_t net_buffer[NET_BUF_SIZE] = { 0 };

/* variables to help fetch rssi of neighbor nodes */
uint8_t cur_num_nbrs;
uint8_t nbr_idx = 0;
nbr_node_metrics_t nbr_nodes_metrics[SIZE_OF_NEIGH_LIST];

uint8_t get_current_net_state(void);

configurable_props_t cfg_props = { .phyTxPower = CONFIG_TRANSMIT_POWER,
    .ccaDefaultdBm = CONFIG_CCA_THRESHOLD,
    .uc_channel_function = CONFIG_CHANNEL_FUNCTION,
    .uc_fixed_channel = CONFIG_UNICAST_FIXED_CHANNEL_NUM,
    .uc_dwell_interval = CONFIG_UNICAST_DWELL_TIME,
    .bc_channel_function = 0,
    .bc_fixed_channel = 0,
    .bc_interval = 0,
    .bc_dwell_interval = 0,
    .pan_id = CONFIG_PAN_ID,
    .network_name = CONFIG_NETNAME,
    .bc_channel_list = CONFIG_BROADCAST_CHANNEL_MASK,
    .uc_channel_list = CONFIG_UNICAST_CHANNEL_MASK,
    .async_channel_list = CONFIG_ASYNC_CHANNEL_MASK,
    .wisun_device_type = CONFIG_WISUN_DEVICE_TYPE,
    .ch0_center_frequency = CONFIG_CENTER_FREQ * 1000,
    .config_channel_spacing = CONFIG_CHANNEL_SPACING,
    .config_phy_id = CONFIG_PHY_ID,
    .config_reg_domain = CONFIG_REG_DOMAIN,
    .operating_class = CONFIG_OP_MODE_CLASS,
    .operating_mode = CONFIG_OP_MODE_ID,
    .hwaddr = CONFIG_INVALID_HWADDR };

static pthread_mutex_t cat_mutex;
static sem_t sem;

#ifdef COAP_SERVICE_ENABLE
int8_t service_id = -1;
#endif

/* Linker file created symbol for addressing MCUBoot header */
extern uint8_t __PRIMARY_SLOT_BASE;

/******************************************************************************
 AT command parser globals
 *****************************************************************************/

static uint8_t at_buf[128];

static bool at_echo = false;

static uint16_t bytesToSend;
static uint16_t bytesRead = 0;

static char connection_type[4];
static char connection_ip[46];
static uint16_t connection_port;

static uint16_t recv_data_length;

static uint8_t sendBuffer[64] = { 0 };

static struct cat_variable cipsend_vars[] = {
    {
        .type = CAT_VAR_INT_DEC,
        .data = &bytesToSend,
        .data_size = sizeof(bytesToSend),
        .name = "bytesToSend",
        .access = CAT_VAR_ACCESS_READ_WRITE,
    },
};

static struct cat_variable cipstart_vars[] = {
    { .type = CAT_VAR_BUF_STRING,
        .data = connection_type,
        .data_size = sizeof(connection_type),
        .name = "type" },
    { .type = CAT_VAR_BUF_STRING,
        .data = connection_ip,
        .data_size = sizeof(connection_ip),
        .name = "remote IP" },
    { .type = CAT_VAR_INT_DEC,
        .data = &connection_port,
        .data_size = sizeof(connection_port),
        .name = "remote port" }
};

static struct cat_variable ciprecvdata_vars[] = {
    { .type = CAT_VAR_INT_DEC,
        .data = &recv_data_length,
        .data_size = sizeof(recv_data_length),
        .name = "receive length" }
};

static struct cat_variable ipd_vars[] = {
    { .type = CAT_VAR_INT_DEC,
        .data = &ipd_data_length,
        .data_size = sizeof(ipd_data_length),
        .name = "ipd length" }
};

/******************************************************************************
Callback definitions
 *****************************************************************************/

/*!
 * Callback for handling incoming UART characters
 */
void uartCallback(UART2_Handle handle, void* buf, size_t count, void* userArg, int_fast16_t status)
{
    uartCharReceived = true;
    sem_post(&sem);
}

/*!
 * Callback for handling any status while a node is joining a network.
 */
void nanostackNetworkHandler(mesh_connection_status_t status)
{
    tr_debug("nanostackNetworkHandler: %x", status);

    if (_blocking) {
        if (_connect_status == CON_STATUS_CONNECTING
            && (status == MESH_CONNECTED || status == MESH_CONNECTED_LOCAL
                || status == MESH_CONNECTED_GLOBAL)) {
            connectedFlg = true;
        } else if (status == MESH_DISCONNECTED) {
            connectedFlg = false;
        }
    }

    if (status == MESH_CONNECTED) {
        uint8_t temp_ipv6_global[16];
        uint8_t temp_ipv6_local[16];
        if (arm_net_address_get(interface_id, ADDR_IPV6_LL, temp_ipv6_local) == 0) {
            tr_info("nanostackNetworkHandler: CON_STATUS_LOCAL_UP, IP %02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x",
                temp_ipv6_local[0], temp_ipv6_local[1],
                temp_ipv6_local[2], temp_ipv6_local[3],
                temp_ipv6_local[4], temp_ipv6_local[5],
                temp_ipv6_local[6], temp_ipv6_local[7],
                temp_ipv6_local[8], temp_ipv6_local[9],
                temp_ipv6_local[10], temp_ipv6_local[11],
                temp_ipv6_local[12], temp_ipv6_local[13],
                temp_ipv6_local[14], temp_ipv6_local[15]);
            _connect_status = CON_STATUS_LOCAL_UP;
        }
        if (arm_net_address_get(interface_id, ADDR_IPV6_GP, temp_ipv6_global) == 0
            && (memcmp(temp_ipv6_global, temp_ipv6_local, 16) != 0)) {
            tr_info("nanostackNetworkHandler: CON_STATUS_GLOBAL_UP, IP %02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x",
                temp_ipv6_global[0], temp_ipv6_global[1],
                temp_ipv6_global[2], temp_ipv6_global[3],
                temp_ipv6_global[4], temp_ipv6_global[5],
                temp_ipv6_global[6], temp_ipv6_global[7],
                temp_ipv6_global[8], temp_ipv6_global[9],
                temp_ipv6_global[10], temp_ipv6_global[11],
                temp_ipv6_global[12], temp_ipv6_global[13],
                temp_ipv6_global[14], temp_ipv6_global[15]);
            _connect_status = CON_STATUS_GLOBAL_UP;
        }
    } else if (status == MESH_CONNECTED_LOCAL) {
        tr_info("nanostackNetworkHandler: CON_STATUS_LOCAL_UP");
        _connect_status = CON_STATUS_LOCAL_UP;
    } else if (status == MESH_CONNECTED_GLOBAL) {
        tr_info("nanostackNetworkHandler: CON_STATUS_GLOBAL_UP");
        _connect_status = CON_STATUS_GLOBAL_UP;
    } else if (status == MESH_BOOTSTRAP_STARTED || status == MESH_BOOTSTRAP_FAILED) {
        tr_info("nanostackNetworkHandler: CON_STATUS_CONNECTING");
        _connect_status = CON_STATUS_CONNECTING;
    } else {
        _connect_status = CON_STATUS_DISCONNECTED;
        tr_info("nanostackNetworkHandler: CON_STATUS_DISCONNECTED");
    }
}

/* forward declarations */
static struct cat_command ipd_cmd;
static struct cat_command closed_cmd;
static struct cat_command sendfail_cmd;

/*!
 * Callback for handling any activity on the network socket
 */
void socket_callback(void* cb)
{
    socket_callback_t* sock_cb = (socket_callback_t*)cb;

    tr_debug("socket_callback() sock=%d, event=0x%x, interface=%d, data len=%d",
        sock_cb->socket_id, sock_cb->event_type, sock_cb->interface_id, sock_cb->d_len);

    switch (sock_cb->event_type & SOCKET_EVENT_MASK) {
        case SOCKET_DATA:
            pthread_mutex_lock(&cat_mutex);
            ipd_data_length += sock_cb->d_len;
            pthread_mutex_unlock(&cat_mutex);
            if (sock_cb->d_len > 0) {
                cat_trigger_unsolicited_read(&at, &ipd_cmd);
            } else {
                ipConnectedFlg = false;
                cat_trigger_unsolicited_read(&at, &closed_cmd);
            }
            tr_info("socket_callback: SOCKET_DATA, sock=%d, bytes=%d", sock_cb->socket_id, sock_cb->d_len);
            break;
        case SOCKET_CONNECT_DONE:
            ipConnectedFlg = true;
            txPendingFlg = false;
            tr_info("socket_callback: SOCKET_CONNECT_DONE");
            break;
        case SOCKET_CONNECT_FAIL:
            tr_info("socket_callback: SOCKET_CONNECT_FAIL");
            tcpConnectProblem = true;
            break;
        case SOCKET_CONNECT_AUTH_FAIL:
            tr_info("socket_callback: SOCKET_CONNECT_AUTH_FAIL");
            break;
        case SOCKET_INCOMING_CONNECTION:
            tr_info("socket_callback: SOCKET_INCOMING_CONNECTION");
            break;
        case SOCKET_TX_FAIL:
            txPendingFlg = false;
            cat_trigger_unsolicited_read(&at, &sendfail_cmd);
            tr_info("socket_callback: SOCKET_TX_FAIL");
            break;
        case SOCKET_CONNECT_CLOSED:
            ipConnectedFlg = false;
            txPendingFlg = false;
            tr_info("socket_callback: SOCKET_CONNECT_CLOSED");
            break;
        case SOCKET_CONNECTION_RESET:
            tr_info("socket_callback: SOCKET_CONNECTION_RESET");
            break;
        case SOCKET_NO_ROUTE:
            tr_info("socket_callback: SOCKET_NO_ROUTE");
            break;
        case SOCKET_TX_DONE:
            txPendingFlg = false;
            tr_info("socket_callback: SOCKET_TX_DONE");
            break;
        case SOCKET_NO_RAM:
            tr_info("socket_callback: SOCKET_NO_RAM");
            break;
        case SOCKET_CONNECTION_PROBLEM:
            tcpConnectProblem = true;
            ipConnectedFlg = false;
            tr_info("socket_callback: SOCKET_CONNECTION_PROBLEM");
            break;
        default:
            break;
    }

    // Wake up main thread
    sem_post(&sem);
}

/******************************************************************************
 AT command parser functions
 *****************************************************************************/

static int lock_mutex()
{
    return pthread_mutex_lock(&cat_mutex);
}

static int unlock_mutex()
{
    return pthread_mutex_unlock(&cat_mutex);
}

static struct cat_mutex_interface cmutex = {
    .lock = lock_mutex,
    .unlock = unlock_mutex
};

static int write_char(char ch)
{
    size_t bytes_written = 0;
    UART2_write(uart, &ch, 1, &bytes_written);
    return bytes_written;
}

static int read_char(char* ch)
{
    uintptr_t key;

    key = HwiP_disable();
    if (uartCharReceived) {
        uartCharReceived = false;
        HwiP_restore(key);

        // Read byte from buffer
        *ch = uartChar;

        // Echo byte
        if (at_echo) {
            write_char(*ch);
        }

        // Trigger next read
        UART2_read(uart, &uartChar, 1, NULL);

        return 1;
    } else {
        HwiP_restore(key);
    }

    return 0;
}

static void write_string(const char* str)
{
    while (*str) {
        write_char(*str++);
    }
}

static struct cat_io_interface iface = {
    .read = read_char,
    .write = write_char
};

static cat_return_state echo_write(const struct cat_command* cmd, const uint8_t* data, const size_t data_size, const size_t args_num)
{
    if (data_size == 0) {
        at_echo = !at_echo;
        return CAT_RETURN_STATE_DATA_OK;
    }

    if (data_size == 1) {
        switch (data[0]) {
            case '0':
                at_echo = false;
                return CAT_RETURN_STATE_DATA_OK;
            case '1':
                at_echo = true;
                return CAT_RETURN_STATE_DATA_OK;
            default:
                break;
        }
    }

    return CAT_RETURN_STATE_ERROR;
}

static cat_return_state rst_run(const struct cat_command* cmd)
{
    SysCtrlSystemReset();

    return CAT_RETURN_STATE_DATA_OK;
}

static cat_return_state cgmm_run(const struct cat_command* cmd)
{
    write_string("CC1352P7\r\n");
    return CAT_RETURN_STATE_DATA_OK;
}

static cat_return_state cgmr_run(const struct cat_command* cmd)
{
    mcuboot_image_version_t iv;
    char version_string[30];
    memcpy(&iv, MCUBOOT_VERSION_PTR, sizeof(iv));
    snprintf(version_string, 30, "%u.%u.%u %u\r\n", iv.iv_major, iv.iv_minor, iv.iv_revision, iv.iv_build_num);
    write_string(version_string);
    return CAT_RETURN_STATE_DATA_OK;
}

static cat_return_state cipstart_write(const struct cat_command* cmd, const uint8_t* data, const size_t data_size, const size_t args_num)
{
    if (ipConnectedFlg) {
        write_string("ALREADY CONNECTED\r\n");
        tr_error("Already connected");
        return CAT_RETURN_STATE_ERROR;
    }

    if (strcmp(connection_type, "TCP") == 0) {
        _isTcp = true;
    } else if (strcmp(connection_type, "UDP") == 0) {
        _isTcp = false;
    } else {
        tr_error("Invalid protocol");
        return CAT_RETURN_STATE_ERROR;
    }

    int8_t ret;

    tcpConnectProblem = false;

    if (socket_id >= 0) {
        // Make sure socket is closed before re-opening
        socket_close(socket_id);
    }

    tr_info("opening network socket");
    socket_id = socket_open(_isTcp ? SOCKET_TCP : SOCKET_UDP, 0, socket_callback);
    if (socket_id < 0) {
        tr_debug("socket open failed with error %d", socket_id);
        write_string("socket_open() failed\r\n");
        return CAT_RETURN_STATE_ERROR;
    }

    static const int32_t buf_size = NET_BUF_SIZE;
    int32_t rtn;
    rtn = socket_setsockopt(socket_id, SOCKET_SOL_SOCKET, SOCKET_SO_RCVBUF, &buf_size, sizeof buf_size);
    tr_info("set Rx buffer len %d, status %d", buf_size, rtn);
    rtn = socket_setsockopt(socket_id, SOCKET_SOL_SOCKET, SOCKET_SO_SNDBUF, &buf_size, sizeof buf_size);
    tr_info("set Tx buffer len %d, status %d", buf_size, rtn);

    peer_addr.type = ADDRESS_IPV6;
    peer_addr.identifier = connection_port;
    stoip6(connection_ip, strlen(connection_ip), peer_addr.address);

    if (_isTcp) {
        ret = socket_connect(socket_id, &peer_addr, 0);
    } else {
        ns_address_t bind_addr;
        bind_addr.type = ADDRESS_IPV6;
        bind_addr.identifier = connection_port;
        memcpy(bind_addr.address, ns_in6addr_any, 16);
        ret = socket_bind(socket_id, &bind_addr);
        ipConnectedFlg = true;
    }

    if (ret < 0) {
        tr_error("socket connect failed with error %d", ret);
        write_string("socket_connect() failed\r\n");
        goto error;
    }

    for (int i = 0; i < 2000; i++) {
        if (ipConnectedFlg || tcpConnectProblem) {
            break;
        }
        usleep(1000);
    }

    if (ipConnectedFlg && !tcpConnectProblem) {
        return CAT_RETURN_STATE_DATA_OK;
    }

error:
    socket_close(socket_id);
    tr_error("CIPSTART failed");
    return CAT_RETURN_STATE_ERROR;
}

static cat_return_state cipclose_run(const struct cat_command* cmd)
{
    if (_isTcp) {
        if (socket_shutdown(socket_id, SOCKET_SHUT_RDWR) == 0) {
            for (int i = 0; i < 2000; i++) {
                if (!ipConnectedFlg) {
                    break;
                }
                usleep(1000);
            }
        }
    }

    ipd_data_length = 0;
    recv_data_length = 0;

    txPendingFlg = false;
    ipConnectedFlg = false;

    socket_close(socket_id);
    socket_id = -1;

    return CAT_RETURN_STATE_DATA_OK;
}

static struct cat_command cipsend_cmd;
static cat_return_state cipsend_write(const struct cat_command* cmd, const uint8_t* data, const size_t data_size, const size_t args_num)
{
    // Make sure this command is not executed at the same time as an unsolicited command
    if (at.unsolicited_fsm.state != CAT_UNSOLICITED_STATE_IDLE) {
        return CAT_RETURN_STATE_DATA_NEXT;
    }

    if (!ipConnectedFlg) {
        tr_error("CIPSEND but not connected");
        return CAT_RETURN_STATE_ERROR;
    }

    if (bytesRead == 0) {
        write_char('>');
    }

    uint16_t min_size = bytesToSend > sizeof(net_buffer) ? sizeof(net_buffer) : bytesToSend;

    for (uint16_t i = 0; i < min_size; i++) {
        while (read_char((char*)&net_buffer[i]) == 0) {
            usleep(500);
        }
    }

    int16_t ret;
    if (_isTcp) {
        while (txPendingFlg) {
            usleep(1000);
        }
        txPendingFlg = true;
        ret = socket_send(socket_id, net_buffer, min_size);
    } else {
        ret = socket_sendto(socket_id, &peer_addr, net_buffer, min_size);
    }
    if (ret < 0) {
        tr_error("socket_sendto() returned %d", ret);
        return CAT_RETURN_STATE_ERROR;
    }
    bytesToSend -= min_size;
    if (bytesToSend > 0) {
        bytesRead += min_size;
        return CAT_RETURN_STATE_DATA_NEXT;
    }
    bytesRead = 0;
    return CAT_RETURN_STATE_OK;
}

static struct cat_command ciprecvdata_cmd;
static cat_return_state ciprecvdata_write(const struct cat_command* cmd, const uint8_t* data, const size_t data_size, const size_t args_num)
{
    // Make sure this command is not executed at the same time as an unsolicited command
    if (at.unsolicited_fsm.state != CAT_UNSOLICITED_STATE_IDLE) {
        return CAT_RETURN_STATE_DATA_NEXT;
    }

    if (recv_data_length == 0) {
        tr_error("No data in receive buffer when calling CIPRECVDATA");
        return CAT_RETURN_STATE_ERROR;
    }

    uint16_t min_size = recv_data_length > NET_BUF_SIZE ? NET_BUF_SIZE : recv_data_length;

    int16_t bytes_recv;
    if (_isTcp) {
        bytes_recv = socket_recv(socket_id, net_buffer, min_size, 0);
    } else {
        bytes_recv = socket_read(socket_id, &peer_addr, net_buffer, min_size);
    }

    if (bytes_recv > 0) {
        write_string(cmd->name);
        write_char(':');
        char size_data[6] = { 0 };
        sprintf(size_data, "%u", (unsigned int)bytes_recv);
        write_string(size_data);
        write_string("\r\n");
        for (uint16_t i = 0; i < bytes_recv; i++) {
            while (write_char(net_buffer[i]) == 0) {
                usleep(500);
            };
        }
        write_string("\r\n");

    } else {
        tr_error("socket_recv() returned %d", bytes_recv);
        return CAT_RETURN_STATE_ERROR;
    }

    return CAT_RETURN_STATE_DATA_OK;
}

static cat_return_state ipd_read(const struct cat_command* cmd, uint8_t* data, size_t* data_size, const size_t max_data_size)
{
    if (ipd_data_length == 0) {
        return CAT_RETURN_STATE_OK;
    }

    ipd_data_length = 0;
    return CAT_RETURN_STATE_DATA_OK;
}

static cat_return_state empty_read(const struct cat_command* cmd, uint8_t* data, size_t* data_size, const size_t max_data_size)
{
    write_string(cmd->name);
    write_string("\r\n");

    return CAT_RETURN_STATE_OK;
}

static cat_return_state csq_run(const struct cat_command* cmd)
{
    ws_stack_info_t ws_stack_info = { 0 };
    ws_stack_info_get(interface_id, &ws_stack_info);
    uint8_t rsl = ws_stack_info.rsl_in < ws_stack_info.rsl_out ? ws_stack_info.rsl_in : ws_stack_info.rsl_out;
    // Convert raw rsl to dBm
    // Conversion according EWMA specified by Wi-SUN
    int16_t rsl_converted = -174 + (int16_t)rsl;
    char rsl_str[5] = { 0 };
    snprintf(rsl_str, 5, "%d", (int)rsl_converted);
    write_string("+CSQ:");
    write_string(rsl_str);
    write_string("\r\n");
    return CAT_RETURN_STATE_DATA_OK;
}

/******************************************************************************
 AT command parser commands
 *****************************************************************************/

static struct cat_command cmds[] = {
    { .name = "E",
        .description = "AT commands echoing",
        .write = echo_write,
        .implicit_write = true },
    { .name = "+RST",
        .description = "Reset device",
        .run = rst_run },
    { .name = "+CGMM",
        .description = "Modem model",
        .run = cgmm_run },
    { .name = "+CGMR",
        .description = "Modem firmware version",
        .run = cgmr_run },
    { .name = "+CSQ",
        .description = "Get RSL value",
        .run = csq_run },
    { .name = "+CIPSTART",
        .description = "Connect TCP",
        .write = cipstart_write,
        .var = cipstart_vars,
        .var_num = sizeof(cipstart_vars) / sizeof(cipstart_vars[0]),
        .need_all_vars = true },
    { .name = "+CIPCLOSE",
        .description = "Close connection",
        .run = cipclose_run },
    {
        .name = "+CIPSEND",
        .description = "Send data to peer",
        .write = cipsend_write,
        .var = cipsend_vars,
        .var_num = sizeof(cipsend_vars) / sizeof(cipsend_vars[0]),
    },
    { .name = "+CIPRECVDATA",
        .description = "Receive data from peer",
        .write = ciprecvdata_write,
        .var = ciprecvdata_vars,
        .var_num = sizeof(ciprecvdata_vars) / sizeof(ciprecvdata_vars[0]),
        .need_all_vars = true },
};

static struct cat_command ipd_cmd = {
    .name = "+IPD",
    .description = "Notify when new data is available on socket",
    .read = ipd_read,
    .var = ipd_vars,
    .var_num = sizeof(ipd_vars) / sizeof(ipd_vars[0]),
};

static struct cat_command closed_cmd = {
    .name = "CLOSED",
    .description = "Notify when peer closed the connection",
    .read = empty_read
};

static struct cat_command sendfail_cmd = {
    .name = "SEND FAIL",
    .description = "Notify when sending failed",
    .read = empty_read
};

static struct cat_command_group cmd_group = {
    .cmd = cmds,
    .cmd_num = sizeof(cmds) / sizeof(cmds[0]),
};

static struct cat_command_group* cmd_desc[] = {
    &cmd_group
};

static struct cat_descriptor desc = {
    .cmd_group = cmd_desc,
    .cmd_group_num = sizeof(cmd_desc) / sizeof(cmd_desc[0]),

    .buf = at_buf,
    .buf_size = sizeof(at_buf)
};

/******************************************************************************
Function declarations Local & Global
 *****************************************************************************/

#ifdef FEATURE_TIMAC_SUPPORT
extern void timacExtaddressRegister();
#endif

mesh_error_t nanostack_wisunInterface_bringup();
mesh_error_t nanostack_wisunInterface_connect(bool blocking);
void nanostack_wait_till_connect();

/******************************************************************************
Function definitions
 *****************************************************************************/

/*!
 * Uart Setup
 */
bool uartSetup(void)
{
    UART2_Params uartParams;

    /* Create a UART in CALLBACK read mode */
    UART2_Params_init(&uartParams);
    uartParams.readMode = UART2_Mode_CALLBACK;
    uartParams.writeMode = UART2_Mode_NONBLOCKING;
    uartParams.readCallback = uartCallback;
    uartParams.baudRate = 115200;

    uart = UART2_open(CONFIG_UART2_1, &uartParams);

    if (uart == NULL) {
        tr_error("Uart open() failed");
        /* UART2_open() failed */
        while (1) {
        }
    }

    // Trigger reading first byte from UART
    UART2_read(uart, &uartChar, 1, NULL);

    return true;
}

/*!
 * Configure the network settings like network name,
 * regulatory domain etc. before starting the network
 */
mesh_error_t nanostack_wisunInterface_configure(void)
{
    int ret;

    if (_configured) {
        // Already configured
        return MESH_ERROR_NONE;
    }
#ifndef TI_WISUN_FAN_OPT
    ret = ws_management_network_name_set(interface_id, cfg_props.network_name);
    if (ret < 0) {
        return MESH_ERROR_PARAM;
    }

    ret = ws_management_regulatory_domain_set(interface_id, CONFIG_REG_DOMAIN,
        CONFIG_OP_MODE_CLASS,
        CONFIG_OP_MODE_ID);
    if (ret < 0) {
        return MESH_ERROR_PARAM;
    }

    ret = ws_management_fhss_unicast_channel_function_configure(interface_id, cfg_props.uc_channel_function,
        cfg_props.uc_fixed_channel,
        cfg_props.uc_dwell_interval);
    if (ret < 0) {
        return MESH_ERROR_PARAM;
    }

    ret = ws_management_fhss_broadcast_channel_function_configure(interface_id, cfg_props.bc_channel_function,
        cfg_props.bc_fixed_channel,
        cfg_props.bc_dwell_interval,
        cfg_props.bc_interval);
    if (ret < 0) {
        return MESH_ERROR_PARAM;
    }

    ret = ws_management_network_size_set(interface_id, MBED_CONF_MBED_MESH_APP_WISUN_NETWORK_SIZE);
    if (ret < 0) {
        return MESH_ERROR_PARAM;
    }
#else
    ws_cfg_set_intferface_all();
#endif   // TI_WISUN_FAN_OPT

    _configured = true;
    return MESH_ERROR_NONE;
}

/*!
 *
 */
mesh_error_t nanostack_wisunInterface_bringup()
{
    int8_t device_id = 0;

#ifndef FEATURE_TIMAC_SUPPORT
    NanostackTiRfPhy_init();
    device_id = NanostackTiRfPhy_rf_register();
#else
    timacExtaddressRegister();
#endif
    // After the RF is up, we can seed the random from it.
    randLIB_seed_random();

    wisun_tasklet_init();

    interface_id = wisun_tasklet_network_init(device_id);
    if (interface_id < 0) {
        return MESH_ERROR_UNKNOWN;
    }

    return MESH_ERROR_NONE;
}

/*!
 * Connect to the Wi-SUN network. Should be called only after calling
 * nanostack_wisunInterface_bringup(). This function also submits
 * the network handler function for handling different events
 * during the connection process, while calling "connect".
 * Only blocking mode has been tested. So, it is recommended for
 * the input parameter blocking to be set to true.
 */
mesh_error_t nanostack_wisunInterface_connect(bool blocking)
{

    int8_t tasklet_id;

    _blocking = blocking;

    tasklet_id = wisun_tasklet_connect(nanostackNetworkHandler, interface_id);

    if (tasklet_id < 0) {
        return MESH_ERROR_UNKNOWN;
    }

    return MESH_ERROR_NONE;
}

/*!
 * Connect to the Wi-SUN network. Should be called only after calling
 * nanostack_wisunInterface_bringup().
 * It is recommended to start the node join process in blocking mode
 */
void nanostack_wait_till_connect()
{
    uint8_t _net_state;
    if (_blocking) {

        // wait till connection goes through
        while (connectedFlg == false) {
            /* Toggle red LED at rate of state*100 ms. Slower the blinking, closer it is to joining */
            _net_state = get_current_net_state();
            usleep((_net_state + 1) * 100000);
            // max usleep value possible is 1000000
            GPIO_toggle(CONFIG_GPIO_RLED);
        }
        /* Solid Green to Indicate that node has Joined */
        GPIO_write(CONFIG_GPIO_RLED, CONFIG_GPIO_LED_OFF);
        GPIO_write(CONFIG_GPIO_GLED, CONFIG_GPIO_LED_ON);
    }
}

/*
 *  ======== mainThread ========
 */
void* mainThread(void* arg0)
{
    int16_t ret;

    pthread_mutex_init(&cat_mutex, NULL);
    sem_init(&sem, 0, 0);

    /* Configure the LED pins */
    GPIO_setConfig(CONFIG_GPIO_GLED, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW);
    GPIO_setConfig(CONFIG_GPIO_RLED, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW);
    /* default user LED's */
    GPIO_write(CONFIG_GPIO_GLED, CONFIG_GPIO_LED_OFF);
    GPIO_write(CONFIG_GPIO_RLED, CONFIG_GPIO_LED_OFF);

    /* Configure UART */
    uartSetup();

    /* Initialize AT parser library */
    cat_init(&at, &desc, &iface, &cmutex);

    write_string("CC1352P7 Wi-SUN AT command modem firmware\r\n");

    nanostack_wisunInterface_configure();

    // Release mutex before blocking
    nanostack_lock();

    if (MESH_ERROR_NONE != nanostack_wisunInterface_bringup()) {
        // release mutex
        nanostack_unlock();
        // do not proceed further
        while (1)
            ;
    }

    if (MESH_ERROR_NONE != nanostack_wisunInterface_connect(true)) {
        // release mutex
        nanostack_unlock();
        // do not proceed further
        while (1)
            ;
    }

    // if here all good so far: proceed further
    // Release mutex before blocking
    nanostack_unlock();

    nanostack_wait_till_connect();

#ifdef COAP_SERVICE_ENABLE
    // Setup Coap over-the-air download
    service_id = coap_service_initialize(interface_id, COAP_PORT, 0, NULL, NULL);
    oad_tasklet_start();

    /* Initialize CoAP OAD services */
    coap_service_register_uri(service_id, OAD_FWV_REQ_URI,
        COAP_SERVICE_ACCESS_GET_ALLOWED | COAP_SERVICE_ACCESS_PUT_ALLOWED | COAP_SERVICE_ACCESS_POST_ALLOWED,
        coap_oad_cb);
    coap_service_register_uri(service_id, OAD_NOTIF_URI,
        COAP_SERVICE_ACCESS_GET_ALLOWED | COAP_SERVICE_ACCESS_PUT_ALLOWED | COAP_SERVICE_ACCESS_POST_ALLOWED,
        coap_oad_cb);
#endif

    write_string("ready\r\n\r\n");

    // Main loop
    while (1) {
        // Perform AT command parsing
        if (cat_service(&at) == CAT_STATUS_OK) {
            sem_wait(&sem);
        }
    }

    return NULL;
}

/*!
 * Returns the current state of the node based on
 * at what state is the node during the network joining process
 * Return value used to vary the rate of red led blinking
 * while the node is joining the network
 */
uint8_t get_current_net_state(void)
{
    protocol_interface_info_entry_t* cur;
    cur = protocol_stack_interface_info_get(IF_6LoWPAN);

    uint8_t curNetState = 0;

    switch (cur->nwk_bootstrap_state) {
        case ER_IDLE:
            curNetState = 0;
            break;
        case ER_ACTIVE_SCAN:
            curNetState = 1;
            break;
        case ER_PANA_AUTH:
            curNetState = 2;
            break;
        case ER_SCAN:
            curNetState = 3;
            break;
        case ER_RPL_SCAN:
            curNetState = 4;
            break;
        case ER_BOOTSRAP_DONE:
            curNetState = 5;
            break;
        case ER_WAIT_RESTART:
            curNetState = 6;
            break;
        default:
            break;
    }

    return (curNetState);
}

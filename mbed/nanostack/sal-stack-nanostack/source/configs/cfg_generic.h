/*
 * Copyright (c) 2014, 2016-2019, Arm Limited and affiliates.
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "base/cfg_thread_border_router.h"
#include "base/cfg_ethernet.h"
#include "base/cfg_lowpan_border_router.h"
#include "base/cfg_local_socket.h"
#include "base/cfg_rf_tunnel.h"
#include "base/cfg_ws_border_router.h"

//#define FEA_TRACE_SUPPORT
//#define EXTRA_CONSISTENCY_CHECKS
#define HAVE_AES
#define ECC
//#define PANA
//#define PANA_SERVER_API
#define HAVE_DHCPV6_SERVER
//#define TCP_TEST
//#define THREAD_THCI_SUPPORT
#define HAVE_WS
//#define MLE_TEST

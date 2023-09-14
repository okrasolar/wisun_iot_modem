#!/usr/bin/env python3

import asyncio
import os
import sys
import aiocoap.resource as resource
from aiocoap import *

# Platform type constants
PLATFORM_TYPE_CC1312R7 = "CC1312R7"
PLATFORM_TYPE_CC1352P7 = "CC1352P7"
PLATFORM_TYPE_CC1314R10 = "CC1314R10"
PLATFORM_TYPE_CC1354P10 = "CC1354P10"

# Platform type mapping
PLATFORM_CHIP_TYPE_LOOKUP = {
    PLATFORM_TYPE_CC1312R7: 23,
    PLATFORM_TYPE_CC1352P7: 26,
    PLATFORM_TYPE_CC1314R10: 30,
    PLATFORM_TYPE_CC1354P10: 31,
}

# OAD constants
OAD_BLOCK_SIZE = 960
MCUBOOT_VERSION_BYTE = 20
OAD_REQ_URI_BASE  = "oad" # Shared base URI for OAD
OAD_FWV_REQ_URI   = "fwv" # Full URI: oad/fwv
OAD_NOTIF_REQ_URI = "ntf" # Full URI: oad/ntf
OAD_BLOCK_REQ_URI = "img" # Full URI: oad/img
OAD_ABORT_URI     = "abort" # Full URI: oad/abort

OAD_IMG_ID        = 123
OAD_COMPLETE_FLAG = 0xFFFF

# OAD globals
oad_file = None
oad_img_len = 0
oad_block_size = OAD_BLOCK_SIZE
oad_complete = None

async def request_fwv(target_ip):
    protocol = await Context.create_client_context()
    req_uri = "coap://[" + target_ip + "]/" + OAD_REQ_URI_BASE + '/' + OAD_FWV_REQ_URI
    request = Message(code=GET, uri=req_uri)

    response = await protocol.request(request).response

    img_id = response.payload[0]
    platform = response.payload[1]
    platform_str = None
    try:
        platform_str = next(key for key, value in PLATFORM_CHIP_TYPE_LOOKUP.items() if value == platform)
    except:
        platform_str = "Unknown"
    version = [response.payload[2], response.payload[3], 0, 0]
    version[2] = int.from_bytes(response.payload[4:6], "little")
    version[3] = int.from_bytes(response.payload[6:10], "little")
    print("Img ID: {}, Platform: {} ({}).".format(img_id, platform, platform_str))
    print("OAD firmware version: {}.{}.{}.{}".format(version[0], version[1], version[2], version[3]))

class ImageBlockResource(resource.Resource):
    async def render_get(self, request):
        global oad_block_size
        global oad_complete

        print("Image block request")

        # Parse oad block request packet
        oad_img_id = request.payload[0]
        oad_block_num = int.from_bytes(request.payload[1:3], "little")
        oad_total_blocks = int.from_bytes(request.payload[3:5], "little")
        oad_block_num_bytes = list(oad_block_num.to_bytes(2, "little"))

        # OAD block transfer end signal, close the file and ack the frame
        if oad_block_num == OAD_COMPLETE_FLAG:
            print("\nOAD complete!")
            oad_complete.set_result(True)
            return Message(mtype=ACK)

        # Calculate OAD block start byte, block size, and OAD duration
        oad_block_start = oad_block_num * oad_block_size
        oad_block_end = oad_block_start + oad_block_size
        if oad_block_end > oad_img_len:
            oad_block_end = oad_img_len
        oad_block_size = oad_block_end - oad_block_start
        oad_log_str = "Block {:04}/{:04} sent. Block size: {:03d}".format(
                oad_block_num + 1, oad_total_blocks, oad_block_size)
        print(oad_log_str)

        # Read oad image from file
        oad_payload = []
        try:
            oad_file.seek(oad_block_start, os.SEEK_SET)
            for _ in range (0, oad_block_size):
                byte = oad_file.read(1)
                oad_payload.append(int.from_bytes(byte, 'big'))
        except:
            print("\nError reading oad binary file")
            oad_complete.set_result(False)
            return Message()

        # Construct and send OAD block payload
        oad_payload = [oad_img_id] + oad_block_num_bytes + oad_payload
        return Message(payload=bytes(oad_payload))

class OadAbortResource(resource.Resource):
    async def render_post(self, request):
        print("OAD aborted!")
        oad_complete.set_result(False)
        return Message()

async def start_oad(target_ip, filename):
    global oad_file
    global oad_img_len
    global oad_block_size
    global oad_complete

    # Open OAD img file
    try:
        oad_file = open(filename, "rb")
        oad_file.seek(0, os.SEEK_END)
        oad_img_len = oad_file.tell()
        oad_file.seek(0, os.SEEK_SET)
        print("OAD image filesize: ", oad_img_len, "bytes")
    except:
        print("Error reading oad image file")
        return

    # Read oad image from file
    platform_type = PLATFORM_CHIP_TYPE_LOOKUP[PLATFORM_TYPE_CC1352P7]
    mcuboot_version = []
    try:
        oad_file.seek(MCUBOOT_VERSION_BYTE, os.SEEK_SET)
        for _ in range(0, 8):
            byte = oad_file.read(1)
            mcuboot_version.append(int.from_bytes(byte, 'big'))
    except:
        print("Error reading mcuboot version from oad image")
        return

    # Build ntf req payload
    oad_block_size = OAD_BLOCK_SIZE
    img_len_bytes = list(oad_img_len.to_bytes(4, "little"))
    oad_block_size_lower = oad_block_size & 0xFF
    oad_block_size_upper = oad_block_size >> 8
    payload_notif_req = [
        OAD_IMG_ID,                # img_id
        platform_type,             # platform/chip type
        oad_block_size_lower,      # block_size
        oad_block_size_upper,
    ] + img_len_bytes + mcuboot_version

    # Start coap server for image block requests
    oad_complete = asyncio.get_running_loop().create_future()
    root = resource.Site()
    root.add_resource([OAD_REQ_URI_BASE, OAD_BLOCK_REQ_URI], ImageBlockResource())
    root.add_resource([OAD_REQ_URI_BASE, OAD_ABORT_URI], OadAbortResource())
    await Context.create_server_context(root)

    # Construct coap message
    protocol = await Context.create_client_context()
    req_uri = "coap://[" + target_ip + "]/" + OAD_REQ_URI_BASE + '/' + OAD_NOTIF_REQ_URI
    request = Message(code=PUT, payload=bytes(payload_notif_req), uri=req_uri)

    try:
        response = await asyncio.wait_for(protocol.request(request).response, timeout=60)
    except asyncio.TimeoutError:
        print("OAD notification timeout")
        return

    # OAD notification response
    status = response.payload[1]
    if status == 1:
        print("OAD upgrade accepted. Starting block transfer")

        # Block until OAD is complete
        await oad_complete

    elif status == 0:
        print("OAD upgrade rejected")
    else:
        print("Invalid OAD notification response status")

if __name__ == "__main__":
    num_arguments = len(sys.argv)
    if num_arguments == 3:
        asyncio.run(start_oad(sys.argv[1], sys.argv[2]))
    elif num_arguments == 2:
        asyncio.run(request_fwv(sys.argv[1]))
    else:
        print("Send a binary OAD to a target device.")
        print()
        print("Usage:")
        print("{} <target ip-address> [file name]".format(sys.argv[0]))
        print("If [file name] is omitted, send a firmware version request")

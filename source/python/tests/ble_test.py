
import asyncio
from bleak import BleakScanner
from bleak import BleakClient
from bleak.backends.device import BLEDevice

async def get_device_by_name(name: str):

    devices = await BleakScanner.discover()

    for device in devices:

        if device.name == name:
            return device

    return None

async def get_device_services(device: BLEDevice):

    async with BleakClient(device) as client:
        return(client.services)

async def write_sscp_request_packet(device: BLEDevice, packet: str):

    async with BleakClient(device) as client:
        await client.write_gatt_char('0000fff6-0000-1000-8000-00805f9b34fb', bytes(packet, encoding='utf-8'))

device = asyncio.run(get_device_by_name("SSCP_DEVICE"))
asyncio.run(write_sscp_request_packet(device, "test"))
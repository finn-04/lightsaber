import asyncio
from bleak import BleakScanner, BleakClient

# Arduino device name
DEVICE_NAME = "NanoBLE"
CHARACTERISTIC_UUID = "12345678-1234-5678-1234-56789abcdef0"

async def send_number (number):
	devices = await BleakScanner.discover ()
	target_device = None

	for device in devices:
		print (f"found device: {device.name} -- {device.address}")
		if device.name == DEVICE_NAME:
			target_device = device
			break

	if target_device:
		async with BleakClient (target_device) as client:
			print (f"connected to {DEVICE_NAME}")
			# wait before writing
			await asyncio.sleep (1)
			await client.write_gatt_char (CHARACTERISTIC_UUID, bytearray (number.encode ()))
			print (f"sent number: {number}")
	else:
		print ("could not find target Bluetooth device")

def main ():
	presets = {
		"Darth Vader": "0",
		"Anakin Skywalker": "1",
		"Rey Skywalker": "2",
		"Luke Skywalker": "3",
		"Mace Windu": "4",
		"Ahsoka Tano": "5"
		}
	print ("CHARACTER PRESETS")
	print (presets)
	while True:
		number_to_send = input ("enter number corresponding to preset: ")
		if number_to_send not in presets.values ():
			print ("please enter a valid preset number\n")
			continue
		loop = asyncio.get_event_loop ()
		loop.run_until_complete (send_number (number_to_send))

if __name__ == "__main__":
	main ()

import asyncio
from bleak import BleakScanner, BleakClient
from flask import Flask, render_template, request
import threading

# Arduino device name
DEVICE_NAME = "NanoBLE"
CHARACTERISTIC_UUID = "12345678-1234-5678-1234-56789abcdef0"

presets = {
	"Darth Vader": "0",
	"Anakin Skywalker": "1",
	"Rey Skywalker": "2",
	"Luke Skywalker": "3",
	"Mace Windu": "4",
	"Ahsoka Tano": "5"
	}

app = Flask (__name__)

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
# to run async function in separate thread
def run_async_send_number (number):
	loop = asyncio.new_event_loop ()
	asyncio.set_event_loop (loop)
	loop.run_until_complete (send_number (number))

# route to render home page
@app.route ("/")
def home ():
	return render_template ("index.html", presets = presets)
# route to handle button click and send preset number
@app.route ("/send_number", methods = ["POST"])
def send ():
	character = request.form.get ("character")
	if character in presets:
		number_to_send = presets [character]
		threading.Thread (target = run_async_send_number, args = (number_to_send,)).start ()
		# asyncio.run (send_number (number_to_send))
		# loop = asyncio.get_event_loop ()
		# loop.run_until_complete (send_number (number_to_send))
		return f"sent preset for {character}"
	else:
		return "invalid character", 400

if __name__ == "__main__":
	app.run (debug = True, use_reloader = False)

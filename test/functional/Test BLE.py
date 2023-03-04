import bleak
import asyncio
from bleak import BleakScanner
import datetime
import re

# dictionary with all characteristics their UUIDs and default vaules
gemma_characteristics = {
    "FW_VERSION": {"uuid": "99704284-4d6b-4812-a599-cfd570230c01", "default_value": 0},
    "HW_VERSION": {"uuid": "99704284-4d6b-4812-a599-cfd570230c02", "default_value": 0},

    "SWITCH_CENTER": {"uuid": "14cdad1f-1b15-41ee-9f51-d5caaf940d01", "default_value": 0},
    "SWITCH_LEFT": {"uuid": "14cdad1f-1b15-41ee-9f51-d5caaf940d02", "default_value": 0},
    "SWITCH_RIGHT": {"uuid": "14cdad1f-1b15-41ee-9f51-d5caaf940d03", "default_value": 0},
    "COLOR_CENTER": {"uuid": "14cdad1f-1b15-41ee-9f51-d5caaf940d04", "default_value": 0xFF000000},
    "COLOR_LEFT": {"uuid": "14cdad1f-1b15-41ee-9f51-d5caaf940d05", "default_value": 0xFF000000},
    "COLOR_RIGHT": {"uuid": "14cdad1f-1b15-41ee-9f51-d5caaf940d06", "default_value": 0xFF000000},
    "BRIGHTNESS_CENTER": {"uuid": "14cdad1f-1b15-41ee-9f51-d5caaf940d07", "default_value": 255},
    "BRIGHTNESS_LEFT": {"uuid": "14cdad1f-1b15-41ee-9f51-d5caaf940d08", "default_value": 255},
    "BRIGHTNESS_RIGHT": {"uuid": "14cdad1f-1b15-41ee-9f51-d5caaf940d09", "default_value": 255},
    "CURRENT_TIME": {"uuid": "21ec2541-a87d-45f6-a5d8-27aa9f742501", "default_value": 0},
    "ALARM_1": {"uuid": "21ec2541-a87d-45f6-a5d8-27aa9f742502", "default_value": 0},
    "ALARM_2": {"uuid": "21ec2541-a87d-45f6-a5d8-27aa9f742503", "default_value": 0},
    "ALARM_3": {"uuid": "21ec2541-a87d-45f6-a5d8-27aa9f742504", "default_value": 0},
    "ALARM_4": {"uuid": "21ec2541-a87d-45f6-a5d8-27aa9f742505", "default_value": 0},
    "ALARM_5": {"uuid": "21ec2541-a87d-45f6-a5d8-27aa9f742506", "default_value": 0},
    "ALARM_6": {"uuid": "21ec2541-a87d-45f6-a5d8-27aa9f742507", "default_value": 0},
    "ALARM_7": {"uuid": "21ec2541-a87d-45f6-a5d8-27aa9f742508", "default_value": 0},
    "ALARM_8": {"uuid": "21ec2541-a87d-45f6-a5d8-27aa9f742509", "default_value": 0},
    "ALARM_9": {"uuid": "21ec2541-a87d-45f6-a5d8-27aa9f742510", "default_value": 0},
    "ALARM_10": {"uuid": "21ec2541-a87d-45f6-a5d8-27aa9f742511", "default_value": 0},
    "LEDS_NL_LEFT": {"uuid": "f0e9cb41-1b2b-4799-ab36-0ddb25e70901", "default_value": 0},
    "LEDS_NL_RIGHT": {"uuid": "f0e9cb41-1b2b-4799-ab36-0ddb25e70902", "default_value": 0},
    "SATURATION_ROT_ENC": {"uuid": "f0e9cb41-1b2b-4799-ab36-0ddb25e70903", "default_value": 0},
    "RESET_DEVICE": {"uuid": "2f7980c8-28d0-4c1c-ad2c-78036e8faf01", "default_value": 0},
    "START_BOOTLOADER": {"uuid": "2f7980c8-28d0-4c1c-ad2c-78036e8faf02", "default_value": 0},
    "RESET_FACTORY_SETTINGS": {"uuid": "2f7980c8-28d0-4c1c-ad2c-78036e8faf03", "default_value": 0},
    "LEDS_STRIP": {"uuid": "2f7980c8-28d0-4c1c-ad2c-78036e8faf10", "default_value": 0},
    "CAPTOUCH_THRESHOLD": {"uuid": "2f7980c8-28d0-4c1c-ad2c-78036e8faf11", "default_value": 0},
    "ENCODER_RESOLUTION": {"uuid": "2f7980c8-28d0-4c1c-ad2c-78036e8faf12", "default_value": 0},

}

gemma_services = {
    "DEVICE_INFO_SERVICE_UUID": {"uuid": "21ec2541-a87d-45f6-a5d8-27aa9f742502"},
    "CONTROL_SERVICE_UUID": {"uuid": "667d724e-4540-4123-984f-9ad6082212ba"},
    "ALARM_SERVICE_UUID": {"uuid": "052699e8-1a9b-40fb-a14b-00b0772187d9"},
    "CONFIGURATION_SERVICE_UUID": {"uuid": "4b698caa-abfa-4f8b-b136-42590f64652e"},
    "SERVICE_COMMANDS_UUID": {"uuid": "12095a43-bcc4-4988-8d45-d2afcad7cd28"}
}


white_device_address = "E8:31:CD:6E:DC:6E"
dev_board_address = "58:BF:25:38:EF:32"
cyrils_bedroom_address = "E8:31:CD:6E:DB:DE"
 
# Finds all devices with a string in the name and creates a dictionary with the name and address


async def get_available_devices(string_filter="Gemma"):
    devices = await BleakScanner.discover()
    found_devices = []
    for device in devices:
        if string_filter in device.name:
            found_devices[device.name] = device.address


def check_fw_version_format(version_string):
    pattern = r"^[0-9]{1,2}\.[0-9]{1,2}\.[0-9]{1,2}\.[0-9]{6}[0-9]{4}$"
    if re.match(pattern, version_string):
        return True
    return False


async def test_fw_version(client):
    # Get fw version
    fw_version = await client.read_gatt_char(gemma_characteristics["FW_VERSION"]["uuid"])
    try:
        fw_version = fw_version.decode("utf-8")
    except:
        print("FW version is not string")
        quit()

    # Check that the version follows the format X.X.X.X
    if check_fw_version_format(fw_version) == False:
        print("Version format is not correct")
        quit()

    else:
        print("FW Version: " + fw_version)


async def test_hw_version(client):
    # Get the hardware version
    hw_version = await client.read_gatt_char(gemma_characteristics["HW_VERSION"]["uuid"])

    try:
        hw_version = int(hw_version.decode("utf-8"))
    except:
        print("Hardware version is not int")
        quit()

    # Check that the hardware version is between 1 and 255
    if (hw_version < 1) or (hw_version > 255):
        print("Hardware version is not correct")
        quit()
    else:
        print(f"HW Version: {hw_version}")


async def test_device_control(lamp, client):

    lamp_switch_uuid = gemma_characteristics[f"SWITCH_{lamp.upper()}"]["uuid"]

    # Switch light on and off and read its value
    await client.write_gatt_char(lamp_switch_uuid, bytearray([0x01]))
    state = await client.read_gatt_char(lamp_switch_uuid)
    if state != bytearray([0x01]):
        print("Switch on failed")
        quit()
    print(f"Switch on {lamp.upper()} OK")

    await client.write_gatt_char(lamp_switch_uuid, bytearray([0x00]))
    state = await client.read_gatt_char(lamp_switch_uuid)
    if state != bytearray([0x00]):
        print("Switch off failed")
        quit()
    print(f"Switch off {lamp.upper()} OK")

    # Switch light on
    await client.write_gatt_char(lamp_switch_uuid, bytearray([0x01]))

    # Change the color of the light and read its value
    await client.write_gatt_char(gemma_characteristics[f"COLOR_{lamp.upper()}"]["uuid"], bytearray([0x00, 0x00, 0x20, 0x20]))
    color = await client.read_gatt_char(gemma_characteristics[f"COLOR_{lamp.upper()}"]["uuid"])
    if color != bytearray([0x00, 0x00, 0x20, 0x20]):
        print("Color change failed")
        quit()
    print(f"Color change {lamp.upper()} OK")

    # Change the brightness of the light and read its value
    for i in range(0, 255, 50):
        await client.write_gatt_char(gemma_characteristics[f"BRIGHTNESS_{lamp.upper()}"]["uuid"], bytearray([i]))
        brihgtness = await client.read_gatt_char(gemma_characteristics[f"BRIGHTNESS_{lamp.upper()}"]["uuid"])
        if brihgtness != bytearray([i]):
            print("Brightness change failed")
            quit()

    print(f"Brightness change {lamp.upper()} OK")

    # Switch light off
    await client.write_gatt_char(lamp_switch_uuid, bytearray([0x00]))


# Test the alarm

async def test_alarm(client):
    
    # Set current time to computer's time
    
    current_time = datetime.datetime.now()
    # Set Time Characteristic format
    # Seconds[0], Minutes[1], Hours[2], Day[3], Month[4], Year[5], Weekday[6]
    # Weekday = Monday = 0x01, Tuesday = 0x02, Wednesday = 0x04, Thursday = 0x08, Friday = 0x10, Saturday = 0x20, Sunday = 0x40
    current_time_array = bytearray ([current_time.second, current_time.minute, current_time.hour, current_time.day, current_time.month, current_time.year - 2000, current_time.weekday()])
    await client.write_gatt_char(gemma_characteristics["CURRENT_TIME"]["uuid"], current_time_array)
    read_time = await client.read_gatt_char(gemma_characteristics["CURRENT_TIME"]["uuid"])
    current_time = datetime.datetime.now()
    # Convert read time to datetime
    read_time = datetime.datetime(read_time[5] + 2000, read_time[4], read_time[3], read_time[2], read_time[1], read_time[0])
    
    if read_time.replace(second = 0) != current_time.replace(second = 0, microsecond=0):
        print(f"Set time failed. Current time is {current_time} and read time is {read_time}")
        quit()
    
    print(f"Set time OK. Current time is {current_time.replace(microsecond=0)} and read time is {read_time}")
    
    # Alarm Characteristic format
    # Enabled[0]
    # Time ON: Minutes[1], Hours[2], Weekdays[3]
    # Time OFF: Minutes[4], Hours[5], Weekdays[6]
    # Max Brightness: Brightness[7]
    # Delay Max Brightness: SecondsLSB[8] SecondsMSB[9]
    # Color: Red[10], Green[11], Blue[12], White[13]
    
    current_time = datetime.datetime.now()
        
    #set_alarm = bytearray([0x01, current_time.minute,  current_time.hour, 0x7F, current_time.minute+5, current_time.hour, 0x7F, 0xFF, 255, 0x00, 0x00, 0xFF, 0x00, 0x00])
    set_alarm = bytearray([0x01, 0,  8, 0x7F, 30, 8, 0x7F, 0xFF, 255, 0x00, 0x00, 0xFF, 0x00, 0x00])
    await client.write_gatt_char(gemma_characteristics["ALARM_1"]["uuid"], set_alarm)
    read_alarm = await client.read_gatt_char(gemma_characteristics["ALARM_1"]["uuid"])
    if read_alarm != set_alarm:
        print(f"Set alarm failed. Set alarm is {set_alarm} and read alarm is {read_alarm}")
        quit()
        
    print(f"Set alarm OK. Set alarm is {set_alarm} and read alarm is {read_alarm}")
    

# test night lights leds and hue of encoder
async def test_config_parameters(client):
    
    # Change the number of leds in the left night light
    set_value= bytearray([0x02, 0x00, 0x00, 0x00])
    await client.write_gatt_char(gemma_characteristics["LEDS_NL_LEFT"]["uuid"], set_value)
    leds_nl = await client.read_gatt_char(gemma_characteristics["LEDS_NL_LEFT"]["uuid"])
    if leds_nl != set_value:
        print(f"Number of leds change failed.Expected NL Left {set_value}, read NL Left {leds_nl}")
        quit()
    print("Number NL Left leds change OK")

    # Change the number of leds in the right night light
    set_value= bytearray([0x06, 0x00, 0x00, 0x00])
    await client.write_gatt_char(gemma_characteristics["LEDS_NL_RIGHT"]["uuid"], set_value)
    leds_nl = await client.read_gatt_char(gemma_characteristics["LEDS_NL_RIGHT"]["uuid"])
    if leds_nl != set_value:
        print(f"Number of leds change failed. Expected NL Right {set_value}, read NL Right {leds_nl}")
        quit()
    print("Number NL Right leds change OK")
    
    # Change the saturation of the encoder
    set_value= bytearray([80, 0x00, 0x00, 0x00])
    await client.write_gatt_char(gemma_characteristics["SATURATION_ROT_ENC"]["uuid"], set_value)
    saturation = await client.read_gatt_char(gemma_characteristics["SATURATION_ROT_ENC"]["uuid"])
    if saturation != set_value:
        print(f"Saturation change failed. Expected saturation {set_value}, read saturation {saturation}")
        quit()

    print("Hue change OK")


# Test service commands
async def test_service_commands(client):

    # Change the number of LEDs in the strip
    set_value= bytearray([0x0A, 0x00, 0x00, 0x00])
    await client.write_gatt_char(gemma_characteristics["LEDS_STRIP"]["uuid"], set_value)
    leds_strip = await client.read_gatt_char(gemma_characteristics["LEDS_STRIP"]["uuid"])
    if leds_strip != set_value:
        print(f"Number of leds change failed. Set value to {set_value} and read value is:  {leds_strip}");
        quit()
        
    print("Number of leds change OK")

    set_value= bytearray([0x20, 0x00, 0x00, 0x00])    
    # Change the captouch threshold
    await client.write_gatt_char(gemma_characteristics["CAPTOUCH_THRESHOLD"]["uuid"], set_value)
    captouch_thld = await client.read_gatt_char(gemma_characteristics["CAPTOUCH_THRESHOLD"]["uuid"])
    if captouch_thld != set_value:
        print(f"Cap Threshold change failed. Set value to {set_value} and read value is:  {captouch_thld}");
        quit()
        
    print("Cap Threshold change OK")
    
    set_value= bytearray([0x21, 0x00, 0x00, 0x00])     
    
    # Change encoder resolution
    await client.write_gatt_char(gemma_characteristics["ENCODER_RESOLUTION"]["uuid"], set_value)
    encoder_res = await client.read_gatt_char(gemma_characteristics["ENCODER_RESOLUTION"]["uuid"])
    if encoder_res != set_value:
        print("Encoder resolution change failed. Set value to {set_value} and read value is:  {encoder_res}");
        quit()
        
    print("Encoder resolution change OK")

    # Read number of LEDs in strip
    await client.write_gatt_char(gemma_characteristics["LEDS_STRIP"]["uuid"], bytearray([0x10, 0x00, 0x00, 0x00])) #0x10 is the default number of leds
    leds_strip = await client.read_gatt_char(gemma_characteristics["LEDS_STRIP"]["uuid"])

    # Reset to default values
    await client.write_gatt_char(gemma_characteristics["RESET_FACTORY_SETTINGS"]["uuid"], bytearray([0x01]))

    # Read the default number of LEDs in strip
    await client.write_gatt_char(gemma_characteristics["LEDS_STRIP"]["uuid"], bytearray([0x30]))
    leds_strip = await client.read_gatt_char(gemma_characteristics["LEDS_STRIP"]["uuid"])

    if leds_strip != bytearray([106]):  # 106 is the default value
        print("Reset to default values failed")
        quit()


async def main():

    # get_available_devices()
    # connect to device
    device = dev_board_address
    device = white_device_address
    device = cyrils_bedroom_address
    async with bleak.BleakClient(device) as client:


        
        
        await test_alarm(client)
        '''await test_fw_version(client)
        await test_hw_version(client)
        await test_device_control("CENTER", client)
        await test_device_control("LEFT", client)
        await test_device_control("RIGHT", client)
        await test_config_parameters(client)
        await test_service_commands(client)'''
        
        
        # Read number of LEDs in strip
        #await client.write_gatt_char(gemma_characteristics["RESET_DEVICE"]["uuid"], bytearray([0x01, 0x00, 0x00, 0x00])) #0x10 is the default number of leds
    
        

        # wait for 1 seconds
        await asyncio.sleep(1.0)

loop = asyncio.get_event_loop()
loop.run_until_complete(main())

import bleak
import asyncio
from bleak import BleakScanner
import time
import re

'''
    "667d724e-4540-4123-984f-9ad6082212ba", // SWITCH_SERVICE_UUID
    "4b698caa-abfa-4f8b-b136-42590f64652e", // PARAMETERS_SERVICE_UUID
    "052699e8-1a9b-40fb-a14b-00b0772187d9", // TIME_SERVICE_UUID
    "21ec2541-a87d-45f6-a5d8-27aa9f742502", // DEVICE_INFO_SERVICE_UUID
    '''
# Dictionary with all services and their UUIDs


''' {"14cdad1f-1b15-41ee-9f51-d5caaf940d01", CONTROL_SERVICE_UUID, READ_WRITE}, // Swtich Center
    {"14cdad1f-1b15-41ee-9f51-d5caaf940d02", CONTROL_SERVICE_UUID, READ_WRITE}, // Switch Left
    {"14cdad1f-1b15-41ee-9f51-d5caaf940d03", CONTROL_SERVICE_UUID, READ_WRITE}, // Switch Right
    {"14cdad1f-1b15-41ee-9f51-d5caaf940d04", CONTROL_SERVICE_UUID, READ_WRITE}, // Color Center
    {"14cdad1f-1b15-41ee-9f51-d5caaf940d05", CONTROL_SERVICE_UUID, READ_WRITE}, // Color Left
    {"14cdad1f-1b15-41ee-9f51-d5caaf940d06", CONTROL_SERVICE_UUID, READ_WRITE}, // Color Right
    {"14cdad1f-1b15-41ee-9f51-d5caaf940d07", CONTROL_SERVICE_UUID, READ_WRITE}, // Brightness Center
    {"14cdad1f-1b15-41ee-9f51-d5caaf940d08", CONTROL_SERVICE_UUID, READ_WRITE}, // Brightness Left
    {"14cdad1f-1b15-41ee-9f51-d5caaf940d09", CONTROL_SERVICE_UUID, READ_WRITE}, // Brightness Right
};'''

'''    {"21ec2541-a87d-45f6-a5d8-27aa9f742501", ALARM_SERVICE_UUID, READ_WRITE}, // Current Time
    {"21ec2541-a87d-45f6-a5d8-27aa9f742502", ALARM_SERVICE_UUID, READ_WRITE}, // Alarm 1
    {"21ec2541-a87d-45f6-a5d8-27aa9f742503", ALARM_SERVICE_UUID, READ_WRITE}, // Alarm 2
    {"21ec2541-a87d-45f6-a5d8-27aa9f742504", ALARM_SERVICE_UUID, READ_WRITE}, // Alarm 3
    {"21ec2541-a87d-45f6-a5d8-27aa9f742505", ALARM_SERVICE_UUID, READ_WRITE}, // Alarm 4
    {"21ec2541-a87d-45f6-a5d8-27aa9f742506", ALARM_SERVICE_UUID, READ_WRITE}, // Alarm 5
    {"21ec2541-a87d-45f6-a5d8-27aa9f742507", ALARM_SERVICE_UUID, READ_WRITE}, // Alarm 6
    {"21ec2541-a87d-45f6-a5d8-27aa9f742508", ALARM_SERVICE_UUID, READ_WRITE}, // Alarm 7
    {"21ec2541-a87d-45f6-a5d8-27aa9f742509", ALARM_SERVICE_UUID, READ_WRITE}, // Alarm 8
    {"21ec2541-a87d-45f6-a5d8-27aa9f742510", ALARM_SERVICE_UUID, READ_WRITE}, // Alarm 9
    {"21ec2541-a87d-45f6-a5d8-27aa9f742511", ALARM_SERVICE_UUID, READ_WRITE}, // Alarm 10
'''
'''
// Parameter Service Characteristics
const Characteristic ConfigurationCharacteristics[]{
    {"f0e9cb41-1b2b-4799-ab36-0ddb25e70901", CONFIGURATION_SERVICE_UUID, READ_WRITE}, // Number of LEDs Night Light Left
    {"f0e9cb41-1b2b-4799-ab36-0ddb25e70902", CONFIGURATION_SERVICE_UUID, READ_WRITE}, // Number of LEDs Night Light Right
    {"f0e9cb41-1b2b-4799-ab36-0ddb25e70903", CONFIGURATION_SERVICE_UUID, READ_WRITE}, // Hue value for Rotary Encoder
};'''

'''
    {"2f7980c8-28d0-4c1c-ad2c-78036e8faf01", SERVICE_COMMANDS_UUID, WRITE_ONLY}, // Reset Device
    {"2f7980c8-28d0-4c1c-ad2c-78036e8faf02", SERVICE_COMMANDS_UUID, WRITE_ONLY}, // Go to Bootloader
    {"2f7980c8-28d0-4c1c-ad2c-78036e8faf03", SERVICE_COMMANDS_UUID, WRITE_ONLY}, // Reset to Factory Defaults

    {"2f7980c8-28d0-4c1c-ad2c-78036e8faf10", SERVICE_COMMANDS_UUID, READ_WRITE}, // Number of LEDs in Strip
    {"2f7980c8-28d0-4c1c-ad2c-78036e8faf11", SERVICE_COMMANDS_UUID, READ_WRITE}, // Captouch Threshold
    {"2f7980c8-28d0-4c1c-ad2c-78036e8faf12", SERVICE_COMMANDS_UUID, READ_WRITE}, // Encoder Resolution
};'''
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
    "HUE_ROTARY_ENCODER": {"uuid": "f0e9cb41-1b2b-4799-ab36-0ddb25e70903", "default_value": 0},
    "RESET_DEVICE": {"uuid": "2f7980c8-28d0-4c1c-ad2c-78036e8faf01", "default_value": 0},
    "START_BOOTLOADER": {"uuid": "2f7980c8-28d0-4c1c-ad2c-78036e8faf02", "default_value": 0},
    "RESET_FACTORY_SETTINGS": {"uuid": "2f7980c8-28d0-4c1c-ad2c-78036e8faf03", "default_value": 0},
    "LEDS_STRIP": {"uuid": "2f7980c8-28d0-4c1c-ad2c-78036e8faf10", "default_value": 0},
    "ENCODER_RESOLUTION": {"uuid": "2f7980c8-28d0-4c1c-ad2c-78036e8faf11", "default_value": 0},  
}



gemma_services = {
    "DEVICE_INFO_SERVICE_UUID": {"uuid": "21ec2541-a87d-45f6-a5d8-27aa9f742502"},
    "CONTROL_SERVICE_UUID": {"uuid": "667d724e-4540-4123-984f-9ad6082212ba"},
    "ALARM_SERVICE_UUID": {"uuid": "052699e8-1a9b-40fb-a14b-00b0772187d9"},
    "CONFIGURATION_SERVICE_UUID": {"uuid": "4b698caa-abfa-4f8b-b136-42590f64652e"},
    "SERVICE_COMMANDS_UUID": {"uuid": "12095a43-bcc4-4988-8d45-d2afcad7cd28"}
}



white_device_address = "E8:31:CD:6E:DB:AE"
dev_board_address = "58:BF:25:38:EF:32"

# Finds all devices with a string in the name and creates a dictionary with the name and address
async def get_available_devices(string_filter="Gemma"):
    devices = await BleakScanner.discover()
    found_devices=[]
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
            print ("FW Version: " + fw_version)
            
            
async def test_hw_version(client):
    #Get the hardware version
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
            print (f"HW Version: {hw_version}")
            
async def test_device_control(lamp, client):   
    
    lamp_switch_uuid = gemma_characteristics[f"SWITCH_{lamp.upper()}"]["uuid"] 
            
    # Switch light on and off and read its value     
    await client.write_gatt_char(lamp_switch_uuid, bytearray([0x01]))
    state = await client.read_gatt_char(lamp_switch_uuid)
    if state != bytearray([0x01]):
        print("Switch on failed")
        quit()
        
    await client.write_gatt_char(lamp_switch_uuid, bytearray([0x00]))
    state = await client.read_gatt_char(lamp_switch_uuid)
    if state != bytearray([0x00]):
        print("Switch off failed")
        quit()
        
    # Change the color of the light and read its value
    await client.write_gatt_char(gemma_characteristics[f"COLOR_{lamp.upper()}"]["uuid"], bytearray([0xFF, 0x00, 0x00,0xFF]))
    color = await client.read_gatt_char(gemma_characteristics[f"COLOR_{lamp.upper()}"]["uuid"]) 
    if color != bytearray([0xFF, 0x00, 0x00,0xFF]):
        print("Color change failed")
        quit()
      
    

async def main():
    
    #get_available_devices()
    # connect to device
    device = dev_board_address
    async with bleak.BleakClient(dev_board_address) as client:
        
        await test_fw_version(client)
        await test_hw_version(client)
        await test_device_control("CENTER", client)
        await test_device_control("LEFT", client)
        await test_device_control("RIGHT", client)
        # wait for 1 seconds
        await asyncio.sleep(1.0)

loop = asyncio.get_event_loop()
loop.run_until_complete(main())

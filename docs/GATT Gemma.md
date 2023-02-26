# GATT Characteristics Summary

The following code defines several arrays of GATT characteristics for different services. 
<br />
<br />

# Device Info Service Characteristics
Read only characteristics that provide information about the device.

---
### **FW Version:** ###
 * **UUID:** `99704284-4d6b-4812-a599-cfd570230c01`
 * **Properties:** Read Only
 * **Description:** Firmware version of the device
 * **Format:** String -> "MM.mm.pp.bbbbbbbbb" (M = Major, m = minor, p = patch, b = build number based on date and time YYMMDDHHmm ) 
 * **Range:** 0-99 for each field and 0-999999999 for build number

---
### **HW Version:** ###
 * **UUID:** `99704284-4d6b-4812-a599-cfd570230c02`
  * **Properties:** Read Only
  * **Description:** Hardware version of the device
  * **Format:** String -> "V" 
---
<br />
<br />

# Control Service Characteristics
All basic control parameters.
---
### **Switch Center:** ###
  * **UUID:** `14cdad1f-1b15-41ee-9f51-d5caaf940d01`
  * **Properties:** Read/Write
  * **Description:** Switch full strip
  * **Format:** UINT8 -> 0 = OFF, 1 = ON
---
### **Switch Left:** ###
  * **UUID:** `14cdad1f-1b15-41ee-9f51-d5caaf940d02`
  * **Properties:** Read/Write
  * **Description:** Switch left night light
  * **Format:** UINT8 -> 0 = OFF, 1 = ON
---
### **Switch Right:** ###
  * **UUID:** `14cdad1f-1b15-41ee-9f51-d5caaf940d03`
  * **Properties:** Read/Write
  * **Description:** Switch right night light
  * **Format:** UINT8 -> 0 = OFF, 1 = ON
---
### **Color Center:** ###
  * **UUID:** `14cdad1f-1b15-41ee-9f51-d5caaf940d04`
  * **Properties:** Read/Write
  * **Description:** Color full strip
  * **Format** UINT32 -> 0xRRGGBBWW (RR = Red, GG = Green, BB = Blue, WW = White)
---
### **Color Left:** ###
  * **UUID:** `14cdad1f-1b15-41ee-9f51-d5caaf940d05`
  * **Properties:** Read/Write
  * **Description:** Color left strip
  * **Format** UINT32 -> 0xRRGGBBWW (RR = Red, GG = Green, BB = Blue, WW = White)
---
### **Color Right:** ###
  * **UUID:** `14cdad1f-1b15-41ee-9f51-d5caaf940d06`
  * **Properties:** Read/Write
  * **Description:** Color right strip
  * **Format** UINT32 -> 0xRRGGBBWW (RR = Red, GG = Green, BB = Blue, WW = White)
---
### **Brightness Center:** ###
  * **UUID:** `14cdad1f-1b15-41ee-9f51-d5caaf940d07`
  * **Properties:** Read/Write
  * **Description:** Brightness full strip
  * **Format:** UINT8 -> 0-255
---
### **Brightness Left:** ###
  * **UUID:** `14cdad1f-1b15-41ee-9f51-d5caaf940d08`
  * **Properties:** Read/Write
  * **Description:** Brightness left night light
  * **Format:** UINT8 -> 0-255
---
### **Brightness Right:** ###
  * **UUID:** `14cdad1f-1b15-41ee-9f51-d5caaf940d09`
  * **Properties:** Read/Write
  * **Description:** Brightness right night light
  * **Format:** UINT8 -> 0-255
---
<br />
<br />

# Alarm Service Characteristics
All alarms related parameters.

---
### **Current Time:** ###
  * **UUID:** `21ec2541-a87d-45f6-a5d8-27aa9f742501`
  * **Properties:** Read/Write
  * **Description:** Current Time
  * **Format:** UINT8[6] ->
    * Seconds[0] -> 0-59
    * Minutes[1] -> 0-59
    * Hours[2] -> 0-23
    * Day[3] -> 1-31
    * Month[4] -> 1-12
    * Year[5] -> 0-99 
---
### **Alarm 1-10:** ###
  * **UUID:** `21ec2541-a87d-45f6-a5d8-27aa9f742502` to `21ec2541-a87d-45f6-a5d8-27aa9f742511`
  * **Properties:** Read/Write
  * **Description:** Alarms of the device
  * **Format:** UINT8[14] ->
    * Enabled [0] -> 0 = OFF, 1 = ON
    * Time ON: Minutes [1], Hours [2], Weekdays [3]  -> 0-59, 0-23, 0-127
    * Time OFF: Minutes [4], Hours [5], Weekdays [6] -> 0-59, 0-23, 0-127
    * Max Brightness: Brightness [7] -> 0-255
    * Delay Max Brightness: SecondsLSB [8] SecondsMSB [9] -> 0-65535
    * Color: Red [10], Green [11], Blue [12], White [13] ->  0xRRGGBBWW (RR = Red, GG = Green, BB = Blue, WW = White)
---
<br />
<br />

# Parameter Service Characteristics

Parameters that are not supposed to be changed by the user. Only for testing and production.

### **LEDs Lefft Night Light:** ###    
  * **UUID:** `f0e9cb41-1b2b-4799-ab36-0ddb25e70901`
  * **Properties:** Read/Write
  * **Description:** Number of LEDs of the Left Night Light
  * **Format:** UINT32 -> 0-50 Must be lower than half the number of LEDs of the strip 
---
### **LEDs Right Night Light:** ###
  * **UUID:** `f0e9cb41-1b2b-4799-ab36-0ddb25e70902`
  * **Properties:** Read/Write
  * **Description:** Number of LEDs of the Right Night Light
  * **Format:** UINT32 -> 0-50. Must be lower than half the number of LEDs of the strip
---
### ** Enconder Saturation:** ###
  * **UUID:** `f0e9cb41-1b2b-4799-ab36-0ddb25e70903`
  * **Properties:** Read/Write
  * **Description:** Saturation value for the rotary encoder
  * **Format:** UINT32 -> 0-100 
---
<br />
<br />

# Service Commands Characteristics
---
### **Reset Device:** ###
  * **UUID:** `2f7980c8-28d0-4c1c-ad2c-78036e8faf01`
  * **Properties:** Write Only
  * **Description:** Resets Device
  * **Format:** UINT8 -> 0x01 to reset
---
### **Go to Wifi Bootloader:** ###
  * **UUID:** `2f7980c8-28d0-4c1c-ad2c-78036e8faf02`
  * **Properties:** Write Only
  * **Description:** Go to Wifi Bootloader. Resets the device and starts the Wifi Bootloader. Bluetooth connection will be lost.
  * **Format:** UINT8 -> 0x01 to go to bootloader
---
### **Reset to Factory Defaults:** ###
  * **UUID:** `2f7980c8-28d0-4c1c-ad2c-78036e8faf03`
  * **Properties:** Write Only
  * **Description:** Reset to Factory Defaults. New values will be loaded from the flash memory but only used after reset
  * **Format:** UINT8 -> 0x01 to reset to factory defaults
---
### **Number of LEDs in Strip** ###
  * **UUID:** `2f7980c8-28d0-4c1c-ad2c-78036e8faf10`
  * **Properties:** Read/Write
  * **Description:** Number of LEDs in Strip
  * **Format:** UINT32 -> 0-255
---
### **Captouch Threshold** ###
  * **UUID:** `2f7980c8-28d0-4c1c-ad2c-78036e8faf11`
  * **Properties:** Read/Write
  * **Description:** Captouch threshold in percentace of the value read at startup
  * **Format:** UINT32 -> 20-99
---
### **Encoder Resolution** ###
  * **UUID:** `2f7980c8-28d0-4c1c-ad2c-78036e8faf12`
  * **Properties:** Read/Write
  * **Description:** Encoder Resolution. The higher the value the more granularity the encoder will have when setting the brightness and color. 4 is the default value
  * **Format:** UINT32 -> 1 - 255
---


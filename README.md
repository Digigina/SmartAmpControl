**The SmartAmpControl** project project is inspired by the DIY 
"RelaiXedPassive -- audio attenuator" project form 
Jos van Eijndhoven, The Netherlands.  
For hardware description visit his website: https://www.joseijndhoven.nl/relaxedpassive/

I use his attenuator board design to control volume and select input channel.

For the control unit design, display and the input device I used a different approach.
Control unit is based on a RP2350 Pico processor using C++ and the Pico SDK.
The advantage of this is that the Pico has a lot more resources and can be programmed in C++ 
which is much more comfortable than the PIC18F25J50-I/SO assembly language.
For time ciritical operation like the IR signal decoding, the RP2350 has a PIO (Programmable Input/Output) 
which can be programmed to handle the IR signal decoding in hardware and thus relieve the CPU from this task.

Part of the project is:

1) central unit -> Raspi Pico2
2) Step-Encoder with i2c interface, I currently use the [Adafruit Rotary Encoder QT](https://eckstein-shop.de/Adafruit-I2C-QT-Rotary-Encoder) module. 
3) 8-Seg Display with SPI interface, I use [Waveshare Pico-8SEG-LED](https://www.waveshare.com/wiki/Pico-8SEG-LED?srsltid=AU7gw4XhmkxuPvsUeWTY91nInwRCAdZb86WWbl9sy5OwnAwqN1z40idX)
4) [RelaiXedPassive](https://www.vaneijndhoven.net/jos/relaixedpassive/index.html) attenuator board form Jos van Eijndhoven
5) ADC channel form the pico to measure quitesence tube current

The step encoder is read via I2C using the Adafruit Rotary Encoder QT module. 
The volume and channel switching is done via a MCP23017 I2C port expander which controls the relays 
for volume and channel selection. The current consumption is measured via a shunt resistor 
and read via the ADC of the RP2350.
This is also an advantage of the Pico an make the project more sutable for tube amplifiers.
The quitescent current of a tube amplifier can be displayed at the same display as the volume and channel information.

The SmartAmpControl firmware is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

The SmartAmpControl firmware is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this firmware.  If not, see <http://www.gnu.org/licenses/>.

# arduino-sound-sensor-with-wifi
Using Arduino Uno, ESP8266-01 and microphone (MAX4466) to capture and send sound level data

## Parts used
* Arduino Uno
* ESP8266-01 as wifi module
* MAX4466 Electret Microphone Amplifier with Adjustable Gain (may require soldering)
* RTCDS3231 as real time clock

Additional items needed:
* Breadboard to connect modules to Arduino Uno
* Jumper wires 
* Battery to RTC (CR2032 or LiRo3032)
* USB-cable to connect Arduino to computer :)

## Connecting the parts 
Make sure to NOT connect the wifi module to anything higher than 3.3v!

![](https://github.com/knappologi/arduino-sound-sensor-with-wifi/blob/master/fritzsketchwifiunortcmax4466_smaller.png)



## Initial configuration

### Set up IDE & download libraries


### Configure ESP8266-01 / wifi module



### Configure RTC module
Before using the RTC module to fetch the time, initial set up to set the time is needed.
This is done by connecting the RTC as shown in the image below.
![](https://github.com/knappologi/arduino-sound-sensor-with-wifi/blob/master/fritzsketch-setup-esp8266-01_smaller.png)

Then a code used to set the time according to when the codes compiles from the computer to arduino is used. 



## Sources
* Kjell & Co: Ansluta Arduino till wifi https://www.kjell.com/se/fraga-kjell/hur-funkar-det/arduino/arduino-projekt/anslut-arduino-till-wifi
* Kjell & Co: Håll tiden med Arduino https://www.kjell.com/se/fraga-kjell/hur-funkar-det/arduino/arduino-projekt/hall-tiden-med-arduino

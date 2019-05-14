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
If you dont have an IDE for Arduino, download and install such. 

For this project, several libraries are used. In Arduino IDE, additional libraries can be downloaded by Tools -> Manage Libraries...
Install the following:
* Rtc by Makuna - https://github.com/Makuna/Rtc
* WiFiEsp (by bportaluri) - https://github.com/bportaluri/WiFiEsp

Make sure the right board, for this project an Arduino Uno, is selected as the board for the IDE and found in the right COM port. 


### Configure ESP8266-01 / wifi module
Using the module, we first configure it to communicate through 9600 baud.
This can be done by connecting the module as shown in the image below, by connecting it to the boards TX and RX, and then by AT-commands configure it to send at the chosen baud rate.

1. After connecting the wifi module, open the Serial monitor. Set it to 115200 Bd and allow both NL & CR.

1. In the top field, write `AT`. If the module responds with OK its ready to communicate and accept instructions.

1. Now, in the top text field, send `AT+UART_DEF=9600,8,1,0,0`. A OK response should now appear. 

1. Reset the module by sending the following command: `AT+RST`.

1. Confirm that the module now can communicate at 9600 bd by changing the Serial monitor to 6900 baud, and again send `AT`. The response should be OK :)

Now the module can be connected to GPIO 6 & 7 and communicate through these. [Source: Kjell&Co](https://www.kjell.com/se/fraga-kjell/hur-funkar-det/arduino/arduino-projekt/anslut-arduino-till-wifi)

![](https://github.com/knappologi/arduino-sound-sensor-with-wifi/blob/master/fritzsketch-setup-esp8266-01_smaller.png)



### Configure RTC module
Before using the RTC module to fetch the current time, initial set up to configure the RTC is need.
After connecting the RTC the Arduino, using the library RTC by Makuna, compile and run [following code from Hur funkar Arduino?](https://github.com/kjellcompany/Arduino_701/blob/master/RtcOledSetTime/RtcOledSetTime.ino) to set the time according to the compiled time of the code.
 [Source: Kjell&Co](https://www.kjell.com/se/fraga-kjell/hur-funkar-det/arduino/arduino-projekt/hall-tiden-med-arduino)



## Sources
* Kjell & Co: Ansluta Arduino till wifi https://www.kjell.com/se/fraga-kjell/hur-funkar-det/arduino/arduino-projekt/anslut-arduino-till-wifi
* Kjell & Co: Håll tiden med Arduino https://www.kjell.com/se/fraga-kjell/hur-funkar-det/arduino/arduino-projekt/hall-tiden-med-arduino

/**
   * Send sound level data to Thingspeak, using wifi :)
   * 
   * Libraries used:
   * SoftwareSerial: https://www.arduino.cc/en/Reference/SoftwareSerial
   * WiFiEsp: https://github.com/bportaluri/WiFiEsp
   * RTC by Makuna: https://github.com/Makuna/Rtc
   * 
   */

// Include SoftwareSerial and Wifi library
#include <SoftwareSerial.h>
#include "WiFiEsp.h"

// For Rtc
#include <RtcDS3231.h>
#include <Wire.h> 

// --------------------- Setup for WiFi ------------------------------
// -------------------------------------------------------------------
// Create WiFi module object on GPIO pin 6 (RX) and 7 (TX)
SoftwareSerial Serial1(6, 7);

// Declare and initialise global arrays for WiFi settings
const char ssid[] = "";  // Change to the name of wifi you want to connect to
const char pass[] = ""; // Change to the wifis password

// Declare and initialise variable for radio status 
int status = WL_IDLE_STATUS;
// -------------------------------------------------------------------


// --------------------- Variables for sound level measuring ---------
// -------------------------------------------------------------------
const int sampleWindow = 50; // Sample window width in mS (50 mS = 20Hz)
unsigned int sample;
short soundValue;
// -------------------------------------------------------------------


// --------------------- Variables for time / RTC & JSON--------------
// -------------------------------------------------------------------

// Create new RTC module
RtcDS3231<TwoWire> rtcModule(Wire);

// To poll for values
RtcDateTime now;

// Hold time data to create strings
byte currentHour;
byte currentMinute;
byte currentSeconds;
byte currentDay;
byte currentMonth;
short currentYear;

// Strings for time
char timeHourMinute[5];   // Format: hh:mm
char timeYearMonthDay[10];  // Format: yyyy-mm-dd
// --------------------------------------------------------------------

// --------------------- Variables for GET/POST -----------------------
// --------------------------------------------------------------------

WiFiEspClient client;
char serverAddress[] = "httpbin.org";
String readingString;
String contentToSend;

// -------------------------------------------------------------------

void setup() {

   // Initialize serial for debugging
  Serial.begin(115200);
  
  // Initialize serial for ESP module
  Serial1.begin(9600);
  
  // Initialize ESP module
  WiFi.init(&Serial1);
 
  connectToWifi();
  getSoundLevelReading();
  updateTime();
  delay(3000);
  Serial.println("Entering loop...");

}

void connectToWifi() {
  // Check for the presence of the shield
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    
    // Don't continue
    while (true);
  }
  
  // Attempt to connect to WiFi network
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    
    // Connect to WPA/WPA2 network
    status = WiFi.begin(ssid, pass);
  }

  Serial.println("You're now connected to the internets :)");
  printWifiStatus();
 
}

void loop() {
  
  while (status != WL_CONNECTED) {
    Serial.println("Disconnected. Attempt to reconnect...");
    Serial.println("");
    
    connectToWifi();
    delay(8000);
  }
  
 printWifiStatus();
 getSoundLevelReading();
 delay(1000);
  
 sendToThingspeak();
   
    // Read response from HTTP request
  while(client.available()&& status == WL_CONNECTED){
    char c = client.read();
    Serial.write(c);
  }
  
  // Fix for socket error & disconnects, see https://forum.arduino.cc/index.php?topic=170460.0
   client.flush();  
   client.stop();

  // Delay of 10 sec for each loop
  Serial.println("waiting...");
  delay(10000);
}

void printWifiStatus() {
  
  // Print the SSID of the network
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // Print the IP address
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
}

/**
 * Based on code from Adafruit: 
 * https://learn.adafruit.com/adafruit-microphone-amplifier-breakout/measuring-sound-levels
 * Reads values from max4466 connected to A0
 */
short getSoundLevelReading() {
   unsigned long startMillis= millis();  // Start of sample window
   unsigned int peakToPeak = 0;   // peak-to-peak level
 
   unsigned int signalMax = 0;
   unsigned int signalMin = 1024;
 
   // collect data for 50 mS
   while (millis() - startMillis < sampleWindow)
   {
      sample = analogRead(0);
      if (sample < 1024)  // toss out spurious readings
      {
         if (sample > signalMax)
         {
            signalMax = sample;  // save just the max levels
         }
         else if (sample < signalMin)
         {
            signalMin = sample;  // save just the min levels
         }
      }
   }
   peakToPeak = signalMax - signalMin;  // max - min = peak-peak amplitude
   // Source to below code: https://forum.arduino.cc/index.php?topic=432991.0
   double volts = ((peakToPeak * 3.3) / 1024) * 0.707;  // convert to volts
   double first = log10(volts/0.00631)*20; // The microphone sensitivity is -44 ±2 so V RMS / PA is 0.00631
   double second = first + 10; // Adjusted value to microphone sensitivity
   soundValue = (short)second;

  Serial.println("SOUND VALUE:    ");
   Serial.print(soundValue);
   Serial.println("");
   return soundValue;
}


void sendToThingspeak(){
  
  const char server[] = "thingspeak.com";
  const char thingspeakAPIKey[] = ""; // Change to your thingspeak key

  // Use client.connect(server, 80) for HTTP
  if (client.connectSSL(server, 443)) {   // HTTPS/SSL = port 443
    Serial.println("Connected to server");
    client.println("GET /update?api_key=" + String(thingspeakAPIKey) + 
    "&field1=" + String(soundValue) + " HTTP/1.1");
    client.println("Host: api.thingspeak.com");
    client.println("Connection: close");
    client.println();
    Serial.println("Sent to server");
  }  else {
    Serial.println("Failed to connect to server :(");
    status = WL_IDLE_STATUS;
  }
}


void printCurrentTime(void) {
  
  char timeString[10];
  sprintf(timeString, "%02u:%02u:%02u", currentHour, currentMinute, currentSeconds);
  Serial.println(timeString);
}

// Below methods are not needed for sending to Thingspeak :)
void createAndUpdateStrings(){
  updateTime();
  createTimeStrings();
  getSoundLevelReading();
}

void updateTime() {
   now = rtcModule.GetDateTime();
 
   currentHour = now.Hour();
   currentMinute = now.Minute();
   currentSeconds = now.Second();
   currentYear = now.Year();
   currentMonth = now.Month();
   currentDay = now.Day();
} 

void createTimeStrings(void) {
  
  sprintf(timeHourMinute, "%02u:%02u", currentHour, currentMinute);
  sprintf(timeYearMonthDay, "%d-%d-%d", currentYear, currentMonth, currentDay);
}

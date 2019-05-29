/**
   * Send sound level data using a GET, or GET data :)
   * 
   * Libraries used:
   * 
   * EspSoftwareSerial: https://github.com/plerup/espsoftwareserial
   * WiFiEsp: https://github.com/bportaluri/WiFiEsp
   * ArduinoJson: https://arduinojson.org/
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
const char ssid[] = "";   // Add your wifi name/SSID
const char pass[] = "";   // Add the wifis password

// Declare and initialise variable for radio status 
int status = WL_IDLE_STATUS;
// -------------------------------------------------------------------


// --------------------- Variables for sound level measuring ---------
// -------------------------------------------------------------------
const int sampleWindow = 50; // Sample window width in mS (50 mS = 20Hz)
unsigned int sample;
short soundValue;
// -------------------------------------------------------------------


// --------------------- Variables for time / RTC --------------------
// -------------------------------------------------------------------

// Create new RTC module
RtcDS3231<TwoWire> rtcModule(Wire);

// To poll for values
RtcDateTime now;

// Allocate JSON document & set RAM

// Hold time data to create strings
byte currentHour;
byte currentMinute;
byte currentSeconds;
byte currentDay;
byte currentMonth;
short currentYear;

// Strings for content
char timeHourMinute[5];
char timeYearMonthDay[10];
// --------------------------------------------------------------------

// --------------------- Variables for POST / GET----------------------
// --------------------------------------------------------------------

WiFiEspClient client;
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

  // Get initial readings and update strings for content
  getSoundLevelReading();
  updateTime();
  formatStringForGetPost();
  delay(3000);
  createAndUpdateStrings();
  Serial.println("Entering loop...");

}

void loop() {
   while (status != WL_CONNECTED) {
    Serial.println("Disconnected. Attempt to reconnect...");
    Serial.println("");
    
    connectToWifi();
    delay(8000);
  }
  
  printWifiStatus();
  createAndUpdateStrings();
  delay(2000);
  
  getHTTPSWithGeneratedParam();
  // getRequestHTTP()     

    // Read response from HTTP request
    while(client.available()&& status == WL_CONNECTED){
    char c = client.read();
    Serial.write(c);
  }
  
  // Fix for socket error & disconnects, see https://forum.arduino.cc/index.php?topic=170460.0
   client.flush();  
   client.stop();

  // Delays the loop 10 sec
  Serial.println("waiting...");
  delay(10000);
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
 * Format: GET param/id/SENSORID/value/SOUNDVALUE/date/YYYY-MM-DD/time/hh:mm HTTP/1.1
 * 
**/
void formatStringForGetPost(){
  
  contentToSend = "";
  String sensorId = "";   // If needed, add sensor ID :) 
  String time2 = timeHourMinute;
  String sound = String(soundValue);
  contentToSend += "GET param/id/";   // Change param to whatever you want for the request
  contentToSend += sensorId;     
  contentToSend += "/value/";
  contentToSend += sound;
  contentToSend += "/date/" + String(timeYearMonthDay);
  contentToSend += "/time/" + String(time2);
  contentToSend += " HTTP/1.1";
  Serial.println(contentToSend);
  
}

/**
 * This method has sends sound level data and time data
 * to the server through a GET request
 * In my project, the backend then modified the data
 * and added the data from the GET-request to the database,
 * using this request as a hacky POST :)
 */
void getHTTPSWithGeneratedParam(){

  // Creates the content to send
  short lengthOfContent = contentToSend.length()+1;
  const char content[lengthOfContent];
  strcpy(content, contentToSend.c_str());
  
  Serial.println("Content to send: ");
  Serial.println(content);

  const char server[] = "";   // Add your server
  
  // use client.connect(server, 80) for HTTP 
  if (client.connectSSL(server, 443)) {   // HTTPS/SSL = port 443
    Serial.println("Connected to server");
    client.println(content);
    client.println("Host: HOSTNAME");   // Change the hostname
    client.println("Connection: close");
    client.println();
    Serial.println("Sent to server");
  }  else {
    Serial.println("Oh no! Failed to connect to server. :(");
    status = WL_IDLE_STATUS;
  }
}

void getRequestHTTP(){
  const char serverAddress[] = ""; // Change to server

  // For HTTPS use client.connectSSL(server, 443)
  if (client.connect(serverAddress, 80)) {   // HTTP = 80
    Serial.println("Connected to server");
    client.println("GET PARAM HTTP/1.1");   // Change param :)
    client.println("Host: YOUR-HOST");      // Change to your host
    client.println("Connection: close");
    client.println();
    Serial.println("Sent to server");
  }  else {
    Serial.println("Oh no! Failed to connect to server.");
    status = WL_IDLE_STATUS;
  }
}



/**
 * Based on code from Adafruit: 
 * https://learn.adafruit.com/adafruit-microphone-amplifier-breakout/measuring-sound-levels
 * Reads values from A0
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


/**
 * Updates time, creates new time strings
 * gets sound level data
 * and creates the formated strings for content
 * 
**/
void createAndUpdateStrings(){
  updateTime();
  createTimeStrings();
  getSoundLevelReading();
  delay(2000);
  formatStringForGetPost();
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

void printCurrentTime(void) {
  
  char timeString[10];
  sprintf(timeString, "%02u:%02u:%02u", currentHour, currentMinute, currentSeconds);
  Serial.println(timeString);
}

void createTimeStrings(void) {
  
  sprintf(timeHourMinute, "%02u:%02u", currentHour, currentMinute);
  sprintf(timeYearMonthDay, "%d-%d-%d", currentYear, currentMonth, currentDay);
}

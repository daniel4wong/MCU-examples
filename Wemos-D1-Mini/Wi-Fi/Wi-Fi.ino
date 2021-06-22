//#include <AuthClient.h>
//#include <debug.h>
//#include <MicroGear.h>
//#include <MQTTClient.h>
//#include <PubSubClient.h>
//#include <SHA1.h>
#include <ESP8266WiFi.h>        // Include the Wi-Fi library
#include <ESP8266WiFiMulti.h>   // Include the Wi-Fi-Multi library

//https://tttapa.github.io/ESP8266/Chap07%20-%20Wi-Fi%20Connections.html
//https://www.mischianti.org/2019/11/21/wemos-d1-mini-esp8266-the-three-type-of-sleep-mode-to-manage-energy-savings-part-4/

ESP8266WiFiMulti wifiMulti;     // Create an instance of the ESP8266WiFiMulti class, called 'wifiMulti'

const char *ssid = "DW"; // The name of the Wi-Fi network that will be created
const char *password = "27842821";   // The password required to connect to it, leave blank for an open network

void stopWiFi() {
  Serial.println('\n');
  Serial.print("Stop Wi-Fi");
  
  WiFi.disconnect();
  WiFi.forceSleepBegin();
  delay(1);
}

void enterModemSleepMode() {
  Serial.println("Enter modem sleep mode");

  uint32_t sleep_time_in_ms = 10000;
  //WiFi.disconnect();
  WiFi.forceSleepBegin();
  delay(sleep_time_in_ms + 1);

  WiFi.forceSleepWake();
  delay(1);
  Serial.println("Exit modem sleep mode");

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  wifi_set_sleep_type(NONE_SLEEP_T);
  delay(10000);  //  Put the esp to sleep for 10s
}

void callback() {
  Serial.println("Callback");
  Serial.flush();
}

void enterLightSleepMode() {
  Serial.println("Enter light sleep mode");
 
  // Here all the code to put con light sleep
  // the problem is that there is a bug on this
  // process
  //wifi_station_disconnect(); //not needed
  uint32_t sleep_time_in_ms = 10000;
  wifi_set_opmode(NULL_MODE);
  wifi_fpm_set_sleep_type(LIGHT_SLEEP_T);
  wifi_fpm_open();
  wifi_fpm_set_wakeup_cb(callback);
  wifi_fpm_do_sleep(sleep_time_in_ms *1000 );
  delay(sleep_time_in_ms + 1);

  Serial.println("Exit light sleep mode");

  WiFi.begin(ssid, password);
  Serial.println("");
 
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial1.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
   
  wifi_set_sleep_type(NONE_SLEEP_T);
  delay(10000);  //  Put the esp to sleep for 10s
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);         // Start the Serial communication to send messages to the computer
  delay(10);
  Serial.println('\n');

  wifiMulti.addAP(ssid, password);   // add Wi-Fi networks you want to connect to

  Serial.println("Connecting ...");
  int i = 0;
  while (wifiMulti.run() != WL_CONNECTED) { // Wait for the Wi-Fi to connect: scan for Wi-Fi networks, and connect to the strongest of the networks above
    delay(1000);
    Serial.print('.');
  }
  Serial.println('\n');
  Serial.print("Connected to ");
  Serial.println(WiFi.SSID());              // Tell us what network we're connected to
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());           // Send the IP address of the ESP8266 to the computer
}

void loop() {
  // put your main code here, to run repeatedly:

  //enterModemSleepMode();
  enterLightSleepMode();
}

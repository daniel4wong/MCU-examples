/* Board: LOLIN(WEMOS) D1 mini (clone)
 * Upload Speed: 512000
 * ROM: nodemcu-release-7-modules-integer.bin
Reference:
- https://wiki.dfrobot.com/URM07-UART_Ultrasonic_Sensor_SKU__SEN0153
- https://diyi0t.com/how-to-reduce-the-esp8266-power-consumption/
- https://www.waveshare.com/wiki/File:SIM7600X-4G-HAT-Demo.7z
*/

#include <ESP8266WiFi.h>        // Include the Wi-Fi library
#include <WiFiClient.h>
#include <SoftwareSerial.h>

#define disableDeepSleepPin D6
#define urmRxPin D4 // URM07 TX <> ESP8255 D1 Mini RX
#define urmTxPin D3 // URM07 RX <> ESP8255 D1 Mini TX

#define simPwPin D7
#define simRxPin D2 // SIM TXD <> ESP8255 D1 Mini RX
#define simTxPin D1 // SIM RXD <> ESP8255 D1 Mini TX

#define header_H    0x55 //Header
#define header_L    0xAA //Header
#define device_Addr 0x11 //Address
#define data_Length 0x00 //Data length
#define get_Dis_CMD 0x02 //Command: Read Distance
#define checksum    (header_H+header_L+device_Addr+data_Length+get_Dis_CMD) //Checksum

const char *ssid = "DW"; // The name of the Wi-Fi network that will be created
const char *password = "27842821";   // The password required to connect to it, leave blank for an open network
const int sleepSeconds = 20;

unsigned char i = 0;
unsigned int  Distance = 0;
unsigned char Rx_DATA[8];
unsigned char CMD[6] = { header_H,header_L,device_Addr,data_Length,get_Dis_CMD,checksum }; //Distance command package

SoftwareSerial urmSerial(urmRxPin, urmTxPin); // RX, TX
SoftwareSerial simSerial(simRxPin, simTxPin); // RX, TX

void setup() {
  pinMode(disableDeepSleepPin, INPUT);
  pinMode(urmRxPin, INPUT);
  pinMode(urmTxPin, OUTPUT);

  pinMode(simPwPin, OUTPUT);
  pinMode(simRxPin, INPUT);
  pinMode(simTxPin, OUTPUT);
  digitalWrite(simPwPin, HIGH);

  Serial.begin(74880);
  while(!Serial) {
    ; // wait for serial port to connect. Needed for native USB
  }
  Serial.println("Setup...");
  
  urmSerial.begin(19200);
  if (!simSerial.begin(115200)) {
    Serial.println("SIM7600 is not ready");
  } else {
    Serial.println("SIM7600 is ready");
  }
  delay(1000);
}

void modemSleep(int timeInSeconds) {
  WiFi.mode(WIFI_OFF);
  WiFi.forceSleepBegin();
  
  Serial.print("Modem sleep " + String(timeInSeconds) + " seconds!");
  delay(sleepSeconds * 1000);
  Serial.println();

  WiFi.forceSleepWake();
  delay(1);
  
  Serial.println("Wake...");
}

void lightSleep(int timeInSeconds) {
  wifi_set_sleep_type(LIGHT_SLEEP_T);
  
  Serial.print("Light sleep " + String(timeInSeconds) + " seconds!");
  delay(sleepSeconds * 1000);
  Serial.println();

  wifi_set_sleep_type(NONE_SLEEP_T);
  delay(1);
  
  Serial.println("Wake...");
}

void deepSleep(int timeInSeconds) {
  Serial.print("Deep sleep " + String(timeInSeconds) + " seconds!");
  ESP.deepSleep(timeInSeconds * 1000000);

  Serial.println("Wake...");
}

void connectWifi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected!");
  
  Serial.print("- IP address: ");
  Serial.println(WiFi.localIP());
}

void readUrm07() {
  for(i=0;i<sizeof(CMD);i++) {
    urmSerial.write(CMD[i]);
  }
  delay(150);  //Wait for the result
  i=0;
  while (urmSerial.available()) {  //Read the return data (Note: this demo is only for the reference, no data verification)
    Rx_DATA[i++]=(urmSerial.read());
  }
  Distance=((Rx_DATA[5]<<8)|Rx_DATA[6]); //Read the distance value
  Serial.print("- Distance: ");  
  Serial.print(Distance);               //print distance value
  Serial.println("cm");
  Rx_DATA[0] = '\0';
}

void powerOn() {
  uint8_t answer = 0;

  // checks if the module is started
  Serial.println("Check module is on (SIM)...");
  answer = sendATcommand("AT", "OK", 5000);
  if (answer == 0) {
    Serial.println("Starting up (SIM)...");
    
    // power on pulse
    digitalWrite(simPwPin, HIGH);
    delay(500);
    digitalWrite(simPwPin, LOW);
    
    // waits for an answer from the module
    while (answer == 0) {     // Send AT every two seconds and wait for the answer
      answer = sendATcommand("AT", "OK", 5000);
      delay(1000);
    }
  }

  delay(1000);

  while (sendATcommand("AT+CGSN", "OK", 5000) > 0) //866323031945941\n\nOK
    delay(1000);
  while (sendATcommand("AT+IPREX?", "OK", 5000) > 0) //+IPREX: 115200\n\nOK
    delay(1000);
  while (sendATcommand("AT+CSQ", "OK", 5000) > 0) //+CSQ: 5,99
    delay(1000);
  while (sendATcommand("AT+CPOS?", "OK", 5000) == 0)
    delay(1000);
  while (sendATcommand("AT+CREG?", "+CREG: 0,1", 5000) == 0)
    delay(1000);

  //while ((sendATcommand("AT+CREG?", "+CREG: 0,1", 500) || sendATcommand("AT+CREG?", "+CREG: 0,6", 500)) == 0)
  //  delay(500);
}

uint8_t sendATcommand(const char* ATcommand, const char* expected_answer, unsigned int timeout) {
  uint8_t x=0,  answer=0;
  char response[100];
  unsigned long previous;

  memset(response, '\0', 100);    // Initialize the string
  delay(100);
    
  while(simSerial.available() > 0)
    simSerial.read();    // Clean the input buffer

  Serial.println(ATcommand);
  simSerial.println(ATcommand);    // Send the AT command 

  x = 0;
  previous = millis();
  // this loop waits for the answer
  do {
    if (simSerial.available() != 0) {    
      // if there are data in the UART input buffer, reads it and checks for the asnwer
      response[x] = simSerial.read();      
      Serial.print(response[x]);
      x++;
      // check if the desired answer  is in the response of the module
      if (strstr(response, expected_answer) != NULL) {
          answer = 1;
          Serial.println();
      }
    } // Waits for the asnwer with time out
  } while((answer == 0) && ((millis() - previous) < timeout));
  
  return answer;
}

void runSim() {
  Serial.println("Write: " + String(simSerial.write("AT\r\n")));
  delay(2000);  //Wait for the result
  i=0;
  while (simSerial.available()){  //Read the return data (Note: this demo is only for the reference, no data verification)
    i++;
    Serial.print(simSerial.read());
  }
  Serial.println();
  Serial.println("Read: " + String(i));
}

void loop() {
  Serial.println("Start loop...");

  //runSim();
  powerOn();
  if (sendATcommand("ATD63581049;", "OK", 20000) == 1) {
    delay(30000);
  }
  //readUrm07();
  //connectWifi();

  if (!digitalRead(disableDeepSleepPin)) {
    //Serial.println("Go to sleep...");
    //deepSleep(sleepSeconds);
    //lightSleep(sleepSeconds);
  } else {
    delay(1000);
  }
}

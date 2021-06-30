// # Author: Strictus.zhang@dfrobot.com
// # Date: 20.08.2016
// # Product Name: URM07-UART Ultrasonic Sensor
// # SKU: SEN0153
// # version number: 1.0
// # Code Description: 20-750cm distance measurement, the received data is not verified

// # Connection: Arduino Uno GPIO Power Supply
// # Pin VCC (URM07 V1.0) -> D3     (Arduino Uno)
// # Pin GND (URM07 V1.0) -> D2     (Arduino Uno)
// # Pin  RX (URM07 V1.0) -> TX1/D1 (Arduino Uno)
// # Pin  TX (URM07 V1.0) -> RX1/D0 (Arduino Uno)

#include <SoftwareSerial.h>

#define header_H    0x55 //Header
#define header_L    0xAA //Header
#define device_Addr 0x11 //Address
#define data_Length 0x00 //Data length
#define get_Dis_CMD 0x02 //Command: Read Distance
#define checksum    (header_H+header_L+device_Addr+data_Length+get_Dis_CMD) //Checksum

#define rxPin 10 // URM07 TX <> RX
#define txPin 11 // URM07 RX <> TX

unsigned char i = 0;
unsigned int  Distance = 0;
unsigned char Rx_DATA[8];
unsigned char CMD[6] = {
  header_H,header_L,device_Addr,data_Length,get_Dis_CMD,checksum
}; //Distance command package

SoftwareSerial Serial1(rxPin, txPin); // RX, TX

void setup() {
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  digitalWrite(3, HIGH);  //Ultrasonic VCC
  digitalWrite(2, LOW);   //Ultrasonic GND
  Serial1.begin(19200);  //Serial1: Ultrasonic Sensor Communication Serial Port, Buadrate: 19200
  Serial.begin(19200);   //Serial: USB Serial Data output, baudrate: 19200
}

void loop() {
  for(i=0;i<6;i++){
    Serial1.write(CMD[i]);
  }
  delay(150);  //Wait for the result
  i=0;
  while (Serial1.available()){  //Read the return data (Note: this demo is only for the reference, no data verification)
    Rx_DATA[i++]=(Serial1.read());
  }
  Distance=((Rx_DATA[5]<<8)|Rx_DATA[6]); //Read the distance value
  Serial.print(Distance);               //print distance value
  Serial.println("cm");

  delay(1000);
}

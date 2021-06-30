/* Board: LOLIN(WEMOS) D1 R2 & mini
Reference:
- https://wiki.dfrobot.com/URM07-UART_Ultrasonic_Sensor_SKU__SEN0153
*/

#include <SoftwareSerial.h>

#define rxPin D4 // URM07 TX <> ESP8255 D1 Mini RX
#define txPin D3 // URM07 RX <> ESP8255 D1 Mini TX

#define header_H    0x55 //Header
#define header_L    0xAA //Header
#define device_Addr 0x11 //Address
#define data_Length 0x00 //Data length
#define get_Dis_CMD 0x02 //Command: Read Distance
#define checksum    (header_H+header_L+device_Addr+data_Length+get_Dis_CMD) //Checksum

unsigned char i = 0;
unsigned int  Distance = 0;
unsigned char Rx_DATA[8];
unsigned char CMD[6] = {
  header_H,header_L,device_Addr,data_Length,get_Dis_CMD,checksum
}; //Distance command package

SoftwareSerial serial(rxPin, txPin); // RX, TX

void setup() {
  pinMode(rxPin, INPUT);
  pinMode(txPin, OUTPUT);
  
  serial.begin(19200);
  Serial.begin(115200);
  
  for (int i=0; i<10; i++) {
    delay(100);
    Serial.print(".");
  }
  Serial.println();
}

void loop() {
  for(i=0;i<6;i++){
    serial.write(CMD[i]);
  }
  delay(150);  //Wait for the result
  i=0;
  while (serial.available()){  //Read the return data (Note: this demo is only for the reference, no data verification)
    Rx_DATA[i++]=(serial.read());
  }
  Distance=((Rx_DATA[5]<<8)|Rx_DATA[6]); //Read the distance value
  Serial.print(Distance);               //print distance value
  Serial.println("cm");
  Rx_DATA[0] = '\0';
  
  delay(1000);
}

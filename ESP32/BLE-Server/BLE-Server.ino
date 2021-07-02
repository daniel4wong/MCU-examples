/*
    Video: https://www.youtube.com/watch?v=oCMOYS71NIU
    Based on Neil Kolban example for IDF: https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleNotify.cpp
    Ported to Arduino ESP32 by Evandro Copercini
    updated by chegewara

   Create a BLE server that, once we receive a connection, will send periodic notifications.
   The service advertises itself as: 4fafc201-1fb5-459e-8fcc-c5c9c331914b
   And has a characteristic of: beb5483e-36e1-4688-b7f5-ea07361b26a8

   The design of creating the BLE server is:
   1. Create a BLE Server
   2. Create a BLE Service
   3. Create a BLE Characteristic on the Service
   4. Create a BLE Descriptor on the characteristic
   5. Start the service.
   6. Start advertising.

   A connect hander associated with the server starts a background task that performs notification
   every couple of seconds.
*/

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLEAdvertising.h>
#include <BLE2902.h>
#include "esp_bt_main.h"
#include "esp_bt_device.h"
#include <EEPROM.h>
#include <ArduinoJson.h>
#include <StreamUtils.h>

#define ROM_SIZE 512

#define SERVICE_UUID                "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_CONFIG_UUID  "beb5483e-36e1-4688-b7f5-ea07361b2600"
#define CHARACTERISTIC_DATA_UUID    "beb5483e-36e1-4688-b7f5-ea07361b2601"

StaticJsonDocument<ROM_SIZE> jsonDocument;
EepromStream eepromStream(0, ROM_SIZE);

BLEServer *pServer = NULL;
BLEService *pService = NULL;
BLECharacteristic *pConfigCharacteristic = NULL;
BLECharacteristic *pDataCharacteristic = NULL;
BLEAdvertisementData pAdvertisementData;
bool deviceConnected = false;
bool oldDeviceConnected = false;
uint32_t value = 0;
std::string configValue;

// https://randomnerdtutorials.com/esp32-bluetooth-low-energy-ble-arduino-ide/

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
    }
};

void setup() {
  Serial.begin(115200);
  while (!Serial) continue;
  
  // Init EEPROM
  EEPROM.begin(512);

  // Read current config
  Serial.println("Read config...");
  deserializeJson(jsonDocument, eepromStream);
  //serializeJson(jsonDocument, Serial);
  Serial.println();

  // Create the BLE Device
  BLEDevice::init("ESP32");

  // Create the BLE Server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());
  
  // Create the BLE Service
  pService = pServer->createService(SERVICE_UUID);
  
  // Create a BLE Characteristic
  pConfigCharacteristic = pService->createCharacteristic(
    CHARACTERISTIC_CONFIG_UUID,
    BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE
  );
  pDataCharacteristic = pService->createCharacteristic(
    CHARACTERISTIC_DATA_UUID,
    BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_NOTIFY | BLECharacteristic::PROPERTY_INDICATE
  );
  
  // https://www.bluetooth.com/specifications/gatt/viewer?attributeXmlFile=org.bluetooth.descriptor.gatt.client_characteristic_configuration.xml
  // Create a BLE Descriptor
  pConfigCharacteristic->setValue("Config");
  pDataCharacteristic->setValue("Data");
  //pCharacteristic->addDescriptor(new BLE2902());

  // Start the service
  pService->start();

  // Start advertising
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
  pAdvertising->setMinPreferred(0x12);
  //pAdvertising->setScanResponse(false);
  //pAdvertising->setMinPreferred(0x0);  // set value to 0x00 to not advertise this parameter
  //pAdvertisementData.setName("ESP1");
  //pAdvertisementData.setShortName("ESP2");
  
  // https://www.bluetooth.com/specifications/assigned-numbers/company-identifiers/
  //uint8_t DATA[] = {0xa3, 0x09};
  //pAdvertisementData.setManufacturerData(std::string((char *)&DATA[0], 2));
  //pAdvertising->setAdvertisementData(pAdvertisementData);
  
  pAdvertising->start();

  Serial.print("MAC address: ");
  printDeviceAddress();
  Serial.println();
  
  Serial.println("BLE server started");
  Serial.println("Waiting a client connection to notify...");
}

void loop() {
  // notify changed value
  if (deviceConnected) {
    pDataCharacteristic->setValue((uint8_t*)&value, 4);
    pDataCharacteristic->notify();
    value++;
    delay(3); // bluetooth stack will go into congestion, if too many packets are sent, in 6 hours test i was able to go as low as 3ms
  }
  // disconnecting
  if (!deviceConnected && oldDeviceConnected) {
    delay(500); // give the bluetooth stack the chance to get things ready
    pServer->startAdvertising(); // restart advertising
    Serial.println("start advertising");
    oldDeviceConnected = deviceConnected;
  }
  // connecting
  if (deviceConnected && !oldDeviceConnected) {
    // do stuff here on connecting
    oldDeviceConnected = deviceConnected;
  }

  std::string value = pConfigCharacteristic->getValue();
  if (value != configValue) {
    Serial.print("The new config characteristic value is: ");
    Serial.println(value.c_str());
    configValue = value;

    Serial.println("Read config...");
    //deserializeJson(jsonDocument, value.c_str());
    //serializeJson(jsonDocument, eepromStream);
    //eepromStream.flush(); 
    //serializeJson(jsonDocument, Serial);
    Serial.println();
    //delay(100);
  } 
}

void printDeviceAddress() {
  const uint8_t* point = esp_bt_dev_get_address();
 
  for (int i = 0; i < 6; i++) {
    char str[3];
    sprintf(str, "%02X", (int)point[i]);
    Serial.print(str);
 
    if (i < 5){
      Serial.print(":");
    }
  }
}

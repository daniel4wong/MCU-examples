#include <EEPROM.h>
#include <ArduinoJson.h>
#include <StreamUtils.h>

#define ROM_SIZE 512

struct Config {
  char ssid[32];
  char password[64];
};

void setup() {
  EEPROM.begin(512);
  EepromStream eepromStream(0, ROM_SIZE);
  StaticJsonDocument<ROM_SIZE> jsonDocument;
  
  Serial.begin(74880); 
  while (!Serial) continue;

  Serial.println("Write config...");
  //writeConfig(jsonDocument, eepromStream);
  //jsonDocument["ssid"] = "DW";
  //jsonDocument["passwordid"] = "27842821";
  //serializeJson(jsonDocument, eepromStream);
  //eepromStream.flush(); 
  //serializeJson(jsonDocument, Serial);
  //Serial.println();

  Serial.println("Read config...");
  //readConfig(jsonDocument, eepromStream);
  deserializeJson(jsonDocument, eepromStream);
  serializeJson(jsonDocument, Serial);
  Serial.println();
}
 
void loop() {
  //checkFlash();
  //delay(5 * 1000);
}

void writeConfig(StaticJsonDocument<ROM_SIZE> document, EepromStream stream) {
  document["ssid"] = "DW";
  document["passwordid"] = "27842821";

  serializeJson(document, stream);
  stream.flush();  
}

void readConfig(StaticJsonDocument<ROM_SIZE> document, EepromStream stream) {
  deserializeJson(document, stream);
  //serializeJsonPretty(document, Serial);
}

void checkFlash() {
  uint32_t realSize = ESP.getFlashChipRealSize();
  uint32_t ideSize = ESP.getFlashChipSize();
  FlashMode_t ideMode = ESP.getFlashChipMode();

  Serial.printf("Flash real id:   %08X\n", ESP.getFlashChipId());
  Serial.printf("Flash real size: %u bytes\n\n", realSize);

  Serial.printf("Flash ide  size: %u bytes\n", ideSize);
  Serial.printf("Flash ide speed: %u Hz\n", ESP.getFlashChipSpeed());
  Serial.printf("Flash ide mode:  %s\n", (ideMode == FM_QIO ? "QIO" : ideMode == FM_QOUT ? "QOUT" : ideMode == FM_DIO ? "DIO" : ideMode == FM_DOUT ? "DOUT" : "UNKNOWN"));

  if (ideSize != realSize) {
    Serial.println("Flash Chip configuration wrong!\n");
  } else {
    Serial.println("Flash Chip configuration ok.\n");
  }
}

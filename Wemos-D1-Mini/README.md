ESP8266

- https://randomnerdtutorials.com/flashing-micropython-firmware-esptool-py-esp32-esp8266/
- https://cyberblogspot.com/how-to-install-esptool-on-windows-10/
  `
  esptool.py --chip esp8266 --port COM12 erase_flash
  `
- https://micropython.org/download/esp8266/
- https://github.com/nodemcu/nodemcu-firmware/releases
- https://github.com/nodemcu/nodemcu-flasher
  `
  cd C:\Users\danielw\Documents\GitHub\MCU-examples\Wemos-D1-Mini
  `
  `
  esptool.py --port COM12 write_flash --flash_mode dio --flash_size detect 0x0 nodemcu-release-7-modules-integer.bin
  `
  Press reset twice quickly
- https://www.jianshu.com/p/78b1400aa56e
- https://nodemcu-build.com/
- https://esp8266.ru/esplorer/
- https://www.wemos.cc/en/latest/tutorials/d1/get_started_with_micropython_d1.html
- https://nodemcu.readthedocs.io/en/latest/flash/#upgrading-firmware
- https://github.com/nodemcu/nodemcu-firmware
- https://github.com/espressif/esptool
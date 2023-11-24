# Flipper Zero Wardriving

## Requirements

- Flipper Zero with [Xtreme Firmware](https://flipper-xtre.me/) installed
- GPIO board with the following modules:    [like this one](https://www.tindie.com/products/sce/flipper-zero-multi-fucc/)
  - ESP32. Could be an WROOM, S2, S3
  - GPS Module. As long as the GPS NMEA app outputs data you're fine

## Setup

The ESP32 and GPS module cannot share the same UART channel, so make sure to have them seperated on `pin 13,14` (aka USUART) and `pin 15,16` (aka LPUART). Then you need to setup the channels inside the Xtreme app, navigate to Xtreme->Protocols->GPIO Pins and set your `UART ESP32 & NMEA channel` to the corresponding pins. This doesn't work on other firmwares due to lack of features. 
<br> <br>
After setting the configuration right, we need to flash the ESP32 microcontroller. Download the binary file in the [release](https://github.com/Sil333033/flipperzero-wardriver/releases) tab that corresponds to your ESP32 model and copy it to your Flipper's SD card. 
<br> <br>
Head to Apps->GPIO->ESP Flasher, enter the bootloader of your board (it should say `Waiting for download` in the terminal) and head to `Manual Flash`. Press `[] Bootloader (0x1000)` and select the binary file you downloaded. Scroll down to `[>] FLASH` and wait for the magic to happen. 
<br> <br>
Now you're ready to go! Open the app inside `Apps->WiFi->[ESP32GPS] Wardriver`.


## How to use

The screen will start with an waiting dolphin that says "No AP's Found". Wait a few seconds and the screen will change to an list. Press `UP` and `DOWN` to scroll through the list. Press `LEFT` to see your actual GPS data.
<br> <br>
The arrow down up means how much packets the AP received and the arrow up means how much packets the AP transmitted.
<br> <br>
Once you exited the app, it will save an csv file to your SD card under `ext/apps_data/ll-wardriver`. The file can be uploaded to Wigle without problems.

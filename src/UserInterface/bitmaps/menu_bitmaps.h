#ifndef MENU_BITMAPS_H
#define MENU_BITMAPS_H

#include <TFT_eSPI.h>
#include <Arduino.h>

// RGB565 image declarations for 320x170
extern const uint16_t image_wifi_Layer_2_bits[320 * 170] PROGMEM;
extern const uint16_t image_bt_Layer_2_bits[320 * 170] PROGMEM;
extern const uint16_t image_rfid_Layer_2_bits[320 * 170] PROGMEM;
extern const uint16_t image_nfc_Layer_2_bits[320 * 170] PROGMEM;
extern const uint16_t image_rf_Layer_2_bits[320 * 170] PROGMEM;
extern const uint16_t image_ir_Layer_2_bits[320 * 170] PROGMEM;
extern const uint16_t image_files_Layer_2_bits[320 * 170] PROGMEM;
extern const uint16_t image_extras_Layer_2_bits[320 * 170] PROGMEM;
extern const uint16_t image_settings_Layer_2_bits[320 * 170] PROGMEM;
extern const uint16_t image_badusb_Layer_2_bits[320 * 170] PROGMEM;
extern const uint16_t image_rpi_Layer_2_bits[320 * 170] PROGMEM;
extern const uint16_t image_radio_Layer_2_bits[320 * 170] PROGMEM;



// Draw function prototypes
void drawWiFiMenu();
void drawBluetoothMenu();
void drawRFIDMenu();
void drawNFCMenu();
void drawRFMenu();
void drawIRMenu();
void drawFilesMenu();
void drawExtrasMenu();
void drawSettingsMenu();
void drawBadUSBMenu();
void drawRPIMenu();
void drawRadioMenu();
void drawOptionsLayerBackground(TFT_eSPI &tft);


#endif
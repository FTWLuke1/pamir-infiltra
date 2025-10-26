#include "menu_submenus.h"
#include "../bitmaps/menu_bitmaps.h"
#include <TFT_eSPI.h>
#include "../../Modules/Functions/Settings/time.h"

extern TFT_eSPI tft;

// ---------------------------------------------------------------------------
// 🧭 Utility Draw Functions
// ---------------------------------------------------------------------------

static void drawMenuBase() {
    tft.fillScreen(TFT_BLACK);
    tft.drawRect(0, 0, tft.width(), tft.height(), TFT_WHITE);
}

static void drawMenuHeader(const char* title) {
    tft.setTextDatum(MC_DATUM);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.drawString(title, tft.width() / 2, 10, 2);
}

static void drawBackButton() {
    const int btnW = 60, btnH = 18;
    const int btnX = 10;
    const int btnY = tft.height() - (btnH + 7);
    tft.fillRoundRect(btnX, btnY, btnW, btnH, 3, TFT_WHITE);
    tft.setTextColor(TFT_BLACK, TFT_WHITE);
    tft.drawString("BACK", btnX + btnW / 2, btnY + (btnH / 2) + 1, 1);
}

// ---------------------------------------------------------------------------
// 🧩 Submenu Implementations
// ---------------------------------------------------------------------------

void drawWiFisubmenu()       { drawMenuBase(); drawMenuHeader("Wi-Fi Tools");           drawBackButton(); }
void drawBluetoothSubmenu()  { drawMenuBase(); drawMenuHeader("Bluetooth Tools");       drawBackButton(); }
void drawRfidSubmenu()       { drawMenuBase(); drawMenuHeader("RFID Utilities");        drawBackButton(); }
void drawNfcSubmenu()        { drawMenuBase(); drawMenuHeader("NFC Toolkit");           drawBackButton(); }
void drawIrSubmenu()         { drawMenuBase(); drawMenuHeader("Infrared Tools");        drawBackButton(); }
void drawFilesSubmenu()      { drawMenuBase(); drawMenuHeader("File Manager");          drawBackButton(); }
void drawExtrasSubmenu()     { drawMenuBase(); drawMenuHeader("Extras");                drawBackButton(); }
void drawSettingsSubmenu()   { drawMenuBase(); drawMenuHeader("Settings");              drawBackButton(); }
void drawBadUsbSubmenu()     { drawMenuBase(); drawMenuHeader("BadUSB Payloads");       drawBackButton(); }
void drawRpiSubmenu()        { drawMenuBase(); drawMenuHeader("Raspberry Pi Tools");    drawBackButton(); }
void drawRfSubmenu()         { drawMenuBase(); drawMenuHeader("Radio & LoRa");          drawBackButton(); }
void drawNrfSubmenu()        { drawMenuBase(); drawMenuHeader("NRF Tools");             drawBackButton(); }
void drawGpsSubmenu()        { drawMenuBase(); drawMenuHeader("GPS Utilities");         drawBackButton(); }
void drawRadioSubmenu()      { drawMenuBase(); drawMenuHeader("Radio Menu");            drawBackButton(); }


#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <TFT_eSPI.h>
#if defined(LILYGO_T_EMBED) || defined(LILYGO_T_EMBED_CC1101)
#define XPOWERS_CHIP_BQ25896
#include <XPowersLib.h>
#include <bq27220.h>
#endif
#include "UserInterface/menus/menu_enums.h"
#include "UserInterface/bitmaps/menu_bitmaps.h"
#include "UserInterface/menus/menu_submenus.h"
#include "UserInterface/menus/submenu_options.h"
#include "Modules/Core/User Input/buttons.h"
#include "Modules/Functions/Extras/stopwatch.h"
#include "Modules/Core/User Config/Passlock.h"
#include "Modules/Functions/Settings/brightness.h"
#include "Modules/Functions/Settings/battery_saver.h"
#include "Modules/Core/User Config/ConfigManager.h"

#if defined(M5CARDPUTER)
#include <M5Cardputer.h>
static constexpr uint8_t ROT_TOP = 4;
#elif defined(M5STICK_C_PLUS_2)
#include <M5StickCPlus2.h>
static constexpr uint8_t ROT_TOP = 2;
#elif defined(M5STICK_C_PLUS_1_1)
static constexpr uint8_t ROT_TOP = 2;
#elif defined(LILYGO_T_EMBED) || defined(LILYGO_T_EMBED_CC1101)
// 🔧 FIXED ROTATION FOR LANDSCAPE (180° FLIPPED)
static constexpr uint8_t ROT_TOP = 3; 
#else
#include <M5StickCPlus2.h>
static constexpr uint8_t ROT_TOP = 2;
#endif

#ifndef TFT_BL
#define TFT_BL 27
#endif
#ifndef TFT_BACKLIGHT_ON
#define TFT_BACKLIGHT_ON 1
#endif

TFT_eSPI tft;
bool inOptionScreen = false;
bool inStopwatch = false;
bool inBGone = false;
bool inIRRead = false;
bool inWiFiScan = false;
bool inPacketScan = false;
MenuState currentMenu = WIFI_MENU;

#if defined(LILYGO_T_EMBED) || defined(LILYGO_T_EMBED_CC1101)
static XPowersPPM sPowerManager;
#if defined(USE_BQ27220_VIA_I2C)
static BQ27220 sFuelGauge;
#endif
static bool sPowerConfigured = false;

static void setupLilygoHardware()
{
  if (sPowerConfigured) {
    return;
  }
  sPowerConfigured = true;

  Serial.println("Power: configuring pins");
#ifdef PIN_POWER_ON
  pinMode(PIN_POWER_ON, OUTPUT);
  digitalWrite(PIN_POWER_ON, HIGH);
#endif
#ifdef BOARD_PWR_EN
  if (BOARD_PWR_EN != PIN_POWER_ON) {
    pinMode(BOARD_PWR_EN, OUTPUT);
    digitalWrite(BOARD_PWR_EN, HIGH);
  }
#endif
#ifdef BOARD_USER_KEY
  pinMode(BOARD_USER_KEY, INPUT);
#endif
#ifdef BOARD_IR_EN
  pinMode(BOARD_IR_EN, OUTPUT);
  digitalWrite(BOARD_IR_EN, HIGH);
#endif
#ifdef BOARD_IR_RX
  pinMode(BOARD_IR_RX, INPUT);
#endif
#ifdef CC1101_SW0_PIN
  pinMode(CC1101_SW0_PIN, OUTPUT);
  digitalWrite(CC1101_SW0_PIN, HIGH);
#endif
#ifdef CC1101_SW1_PIN
  pinMode(CC1101_SW1_PIN, OUTPUT);
  digitalWrite(CC1101_SW1_PIN, HIGH);
#endif
#ifdef CC1101_SS_PIN
  pinMode(CC1101_SS_PIN, OUTPUT);
  digitalWrite(CC1101_SS_PIN, HIGH);
#endif
#ifdef SDCARD_CS
  pinMode(SDCARD_CS, OUTPUT);
  digitalWrite(SDCARD_CS, HIGH);
#endif
#if defined(TFT_CS) && (TFT_CS) >= 0
  pinMode(TFT_CS, OUTPUT);
  digitalWrite(TFT_CS, HIGH);
#endif
#if defined(BOARD_LORA_CS)
  pinMode(BOARD_LORA_CS, OUTPUT);
  digitalWrite(BOARD_LORA_CS, HIGH);
#endif
#if defined(BOARD_LORA_SW0)
  pinMode(BOARD_LORA_SW0, OUTPUT);
  digitalWrite(BOARD_LORA_SW0, HIGH);
#endif
#if defined(BOARD_LORA_SW1)
  pinMode(BOARD_LORA_SW1, OUTPUT);
  digitalWrite(BOARD_LORA_SW1, HIGH);
#endif

#if defined(PIN_IIC_SDA) && defined(PIN_IIC_SCL)
  Wire.begin(PIN_IIC_SDA, PIN_IIC_SCL);
#else
  Wire.begin();
#endif

  bool pmicOk = sPowerManager.init(Wire, PIN_IIC_SDA, PIN_IIC_SCL, BQ25896_SLAVE_ADDRESS);
  if (pmicOk) {
    sPowerManager.setSysPowerDownVoltage(3300);
    sPowerManager.setInputCurrentLimit(3250);
    sPowerManager.disableCurrentLimitPin();
    sPowerManager.setChargeTargetVoltage(4208);
    sPowerManager.setPrechargeCurr(64);
    sPowerManager.setChargerConstantCurr(832);
    sPowerManager.enableMeasure(PowersBQ25896::CONTINUOUS);
    sPowerManager.disableOTG();
    sPowerManager.enableCharge();
    Serial.printf("Power: input limit %d mA\n", sPowerManager.getInputCurrentLimit());
    Serial.printf("Power: charge current %d mA\n", sPowerManager.getChargerConstantCurr());
  } else {
    Serial.println("Power: PMIC init failed");
  }

#if defined(USE_BQ27220_VIA_I2C)
  if (sFuelGauge.getDesignCap() != BATTERY_DESIGN_CAPACITY) {
    sFuelGauge.setDesignCap(BATTERY_DESIGN_CAPACITY);
  }
#endif
}
#endif

#if defined(LILYGO_T_EMBED) || defined(LILYGO_T_EMBED_CC1101)
struct LcdInitCmd {
  uint8_t cmd;
  uint8_t data[14];
  uint8_t len;
};

static const LcdInitCmd kLilygoLcdInit[] = {
    {0x11, {0}, 0x80},
    {0x3A, {0x05}, 1},
    {0xB2, {0x0B, 0x0B, 0x00, 0x33, 0x33}, 5},
    {0xB7, {0x75}, 1},
    {0xBB, {0x28}, 1},
    {0xC0, {0x2C}, 1},
    {0xC2, {0x01}, 1},
    {0xC3, {0x1F}, 1},
    {0xC6, {0x13}, 1},
    {0xD0, {0xA7}, 1},
    {0xD0, {0xA4, 0xA1}, 2},
    {0xD6, {0xA1}, 1},
    {0xE0, {0xF0, 0x05, 0x0A, 0x06, 0x06, 0x03, 0x2B, 0x32, 0x43, 0x36, 0x11, 0x10, 0x2B, 0x32}, 14},
    {0xE1, {0xF0, 0x08, 0x0C, 0x0B, 0x09, 0x24, 0x2B, 0x22, 0x43, 0x38, 0x15, 0x16, 0x2F, 0x37}, 14},
};

static void initLilygoPanel()
{
#ifdef PIN_POWER_ON
  pinMode(PIN_POWER_ON, OUTPUT);
  digitalWrite(PIN_POWER_ON, HIGH);
  delay(10);
  Serial.println("Panel: power rail enabled");
#endif
#ifdef CC1101_SW0_PIN
  pinMode(CC1101_SW0_PIN, OUTPUT);
  digitalWrite(CC1101_SW0_PIN, HIGH);
#endif
#ifdef CC1101_SW1_PIN
  pinMode(CC1101_SW1_PIN, OUTPUT);
  digitalWrite(CC1101_SW1_PIN, HIGH);
#endif
#if defined(TFT_RST) && (TFT_RST) >= 0
  pinMode(TFT_RST, OUTPUT);
  digitalWrite(TFT_RST, LOW);
  delay(20);
  digitalWrite(TFT_RST, HIGH);
  delay(20);
  Serial.println("Panel: hardware reset pulse");
#endif
#if defined(CC1101_SS_PIN)
  pinMode(CC1101_SS_PIN, OUTPUT);
  digitalWrite(CC1101_SS_PIN, HIGH);
#endif
#if defined(SDCARD_CS)
  pinMode(SDCARD_CS, OUTPUT);
  digitalWrite(SDCARD_CS, HIGH);
#endif
#if defined(TFT_SCLK)
#if defined(TFT_MISO) && (TFT_MISO >= 0)
  SPI.begin(TFT_SCLK, TFT_MISO, TFT_MOSI, TFT_CS);
#else
  SPI.begin(TFT_SCLK, -1, TFT_MOSI, TFT_CS);
#endif
  Serial.println("Panel: SPI.begin complete");
#endif
  tft.begin();
  Serial.println("Panel: tft.begin complete");
  for (const auto& entry : kLilygoLcdInit) {
    tft.writecommand(entry.cmd);
    const uint8_t dataLen = entry.len & 0x7F;
    for (uint8_t i = 0; i < dataLen; ++i) {
      tft.writedata(entry.data[i]);
    }
    if (entry.len & 0x80) {
      delay(120);
    }
  }
  Serial.println("Panel: init commands sent");

  // ✅ ROTATION FIXED (180° LANDSCAPE)
  tft.setRotation(3);
  tft.setSwapBytes(true);
  tft.fillScreen(TFT_BLACK);

#if defined(TFT_CS) && (TFT_CS) >= 0
  pinMode(TFT_CS, OUTPUT);
  digitalWrite(TFT_CS, HIGH);
#endif
  if (TFT_BL >= 0) {
    pinMode(TFT_BL, OUTPUT);
    digitalWrite(TFT_BL, TFT_BACKLIGHT_ON ? HIGH : LOW);
    Serial.println("Panel: backlight enabled");
  }
  tft.setTextColor(TFT_YELLOW, TFT_BLACK);
  tft.setTextDatum(MC_DATUM);
  tft.drawString("Booting...", tft.width() / 2, tft.height() / 2, 2);
  delay(200);
  tft.fillScreen(TFT_RED);
  delay(150);
  tft.fillScreen(TFT_BLACK);
  Serial.println("Panel: boot splash drawn");
}
#endif

void setup()
{
  bool panelInitialised = false;
#if defined(M5CARDPUTER)
  M5Cardputer.begin();
  M5Cardputer.Display.setRotation(ROT_TOP);
  M5Cardputer.Display.setBrightness(180);
#elif defined(M5STICK_C_PLUS_2)
  M5.begin();
  M5.Display.setRotation(ROT_TOP);
  M5.Display.setBrightness(200);
#elif defined(M5STICK_C_PLUS_1_1)
  axp192_init_for_stickc_plus11();
#elif defined(LILYGO_T_EMBED) || defined(LILYGO_T_EMBED_CC1101)
  Serial.begin(115200);
  delay(500);
  Serial.println();
  Serial.println("Infiltra: LilyGo T-Embed boot");
  setupLilygoHardware();
  initLilygoPanel();
  panelInitialised = true;
#else
  M5.begin();
  M5.Display.setRotation(ROT_TOP);
  M5.Display.setBrightness(200);
#endif
  if (!panelInitialised) {
    tft.init();
    tft.setRotation(ROT_TOP);
    tft.fillScreen(TFT_BLACK);
    delay(80);
  } else {
    delay(80);
#if defined(LILYGO_T_EMBED) || defined(LILYGO_T_EMBED_CC1101)
    Serial.println("Display init complete");
#endif
  }

  initButtons();
  initSubmenuOptions(&tft);
  ConfigManager::init(); 
  batterySaverInitAndApply(); 
  Core::Passlock::begin();
  Core::Passlock::promptForUnlock();
  drawWiFiMenu();
  delay(100); 
  brightnessInitAndApply(); 
}

void loop()
{
  updateButtons();
  handleAllButtonLogic(&tft,
                       inOptionScreen,
                       inStopwatch,
                       inBGone,
                       inIRRead,
                       inWiFiScan,
                       inPacketScan,
                       currentMenu);
}

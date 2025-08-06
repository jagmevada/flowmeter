#pragma once
#include <Arduino.h>
#include <LiquidCrystal_I2C.h>


// Extern declarations for shared objects
extern LiquidCrystal_I2C lcd;

extern float flowRate;
extern float totalVolume;
extern unsigned long totalTime;
extern bool useDummyData;
extern unsigned long dummyPulseInterval;

// Display update interval (ms)
#define DISP_UPDATE_INTERVAL 250

// Display modes/pages
enum DisplayPage {
    PAGE_MAIN,
    PAGE_INPUT,
    PAGE_QUICK_ADJUST,
    PAGE_CALIBRATION
};

// Function prototypes
void initDisplay();
void updateDisplay();
void forceDisplayRefresh();
void showMainPage();
void showInputPage();
void showQuickAdjustPage();
void showCalibrationPage();
void dispBlink();
bool lcdclearneeded();
void lcd_valvestate_update();
void lcd_vessel_update(); // Ensure this function is declared for vessel updates
void lcd_flowrate_update();
void lcd_currentvolume_update();
void lcd_timeupdate();
void caldataupdate();
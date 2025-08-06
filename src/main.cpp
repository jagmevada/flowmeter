#include <Wire.h>
#include <FlowSensor.h>
#include "global.h"
#include "disp.h"
#include "vessel.h"
#include "keypads.h"

FlowSensor flow(NON_LINEAR, 3); // or your actual constructor
unsigned long lastDisplayUpdate = 0;

void countPulse() {
  flow.count();
}
void setup() {
  Serial.begin(9600);
  pinMode(SOLENOID_VALVE_PIN, OUTPUT);
  valveoff(); // Ensure the valve is off at startup
  Wire.begin();
  Wire.setClock(400000);
  lcd.init();
  lcd.backlight();
  lcd.clear();
  updateDisplay();
  flow.begin(countPulse);
  flow.setCalibration(15, 3);  // Vessel-specific calibration
  flow.read();
  flow.resetVolume(); // Reset volume at startup
  flow.resetPulse(); // Reset pulse count at startup
  oldTime = millis();
  
}

void loop() {
  handleKeypress();
  static bool cal = false;
  if(currentMode == MODE_CALIBRATION) {

    if(cal == false){
      Serial.println("in cal mode");
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print ("Cal");
      caldataupdate();
      cal = true;
      flow.read();
      flow.resetPulse();
      flow.resettime();
      flow.resetVolume();
      flow.read();
      Serial.println("all reset");
    }

    if(valveState){
        static u32 ts= millis();
        if(millis()-ts >997){
          ts= millis();
          flow.read();
          caldataupdate();
        }
      }
      else{ 
        // Serial.println("Valve manual off");
      }

  }
  else{
    cal = false;
  static u32 lastTick = 0;
if (valveState ) {
  if(prevvalveState != valveState){
    lastTick = millis(); // Reset the tick counter when valve state changes
    lcd_valvestate_update();
  }  
  if (millis() - lastTick >= 956) {
      lastTick = millis(); // Update lastTick to current time
      elapsedSeconds++;
      lcd_timeupdate();

      flow.read();
      flowRate = flow.getFlowRate_m(); // Get flow rate in L/min
      
      dispensedVolumef += flow.getVolume(); // Update dispensed volume
      dispensedVolume = (uint16_t)dispensedVolumef; // Convert to integer for display
      flow.resetVolume();
      Serial.print("pulse :");
      Serial.print(flow.getPulse());
      Serial.print("Flow Rate: ");
      Serial.print(flow.getFlowRate_s());
      Serial.print("\tvolume: ");
      Serial.println(flow.getVolume());
  }  
  if(dispensedVolume >= setVolume){
    valveoff();
    valveState = false;
    lcd_valvestate_update();
  }
  prevvalveState = valveState;
  }
  else{
    // i=0;
  }
  
    
    detectVessel();
    if (millis() - lastDisplayUpdate > displayUpdateInterval) {
    updateDisplay();
    lastDisplayUpdate = millis();
    //  i++;
  }

  }
  delay(1);

}

    //   static u8 i=0;
    // if(i==2){
    //   flowRate++;
    //   if(flowRate>=30.0){
    //     flowRate=0.0;
    //     dispensedVolume = 0;
    //   }
    //   dispensedVolume+= 5;
    // i=0;
    // }
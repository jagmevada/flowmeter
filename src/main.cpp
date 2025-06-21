#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>



// LCD Configuration
LiquidCrystal_I2C lcd(0x27, 16, 2); // Adjust I2C address if needed

void pulseCounter();             // ISR for the YF-B5 flow sensor
void detectVessel();             // reads A0 and sets currentVessel
void updateDisplay();            // refreshes the 1602 LCD
void handleKeypress(char key);   // processes one keypad key
void simulateFlowProfile() ;

// Keypad Configuration
const byte ROWS = 5;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'F','F','#','*'},
  {'1','2','3','^'},
  {'4','5','6','v'},
  {'7','8','9','E'},
  {'<','0','>','E'}
};
byte rowPins[ROWS] = {A1, A2, A3, A4, A5}; // Adjust pins as needed
byte colPins[COLS] = {2, 3, 4, 5};         // Adjust pins as needed
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// Pin Definitions
#define FLOW_SENSOR_PIN 2    // Interrupt-capable pin for flow sensor
#define SOLENOID_VALVE_PIN 6 // Control pin for solenoid valve
#define VESSEL_DETECT_PIN A0 // Analog pin for vessel detection

// Flow Sensor Constants (YF-B5)
const float calibrationFactor = 7.5; // Pulses per liter per minute
volatile byte pulseCount = 0;
float flowRate = 0.0;
float totalVolume = 0.0;
unsigned long totalTime = 0;
unsigned long oldTime = 0;

// Vessel Detection
const int vesselResistors[] = {0, 150, 300, 470, 680, 1000}; // Example resistor values in ohms
const int numVessels = sizeof(vesselResistors) / sizeof(vesselResistors[0]);
int currentVessel = 0;

// Display Update Control
unsigned long lastDisplayUpdate = 0;
const long displayUpdateInterval = 200; // ms between display updates

// Dummy Data Generation
bool useDummyData = true; // Set to false when using real sensor
unsigned long lastDummyPulse = 0;
unsigned long dummyPulseInterval = 1000; // ms between dummy pulses

void setup() {
  // Initialize serial communication
  Serial.begin(9600);
  
  // Initialize I2C LCD at 400kHz
  Wire.begin();
  Wire.setClock(400000);
  lcd.init();
  lcd.backlight();
  lcd.clear();
  
  // Initialize pins
  pinMode(SOLENOID_VALVE_PIN, OUTPUT);
  digitalWrite(SOLENOID_VALVE_PIN, LOW); // Start with valve closed
  
  // Set up flow sensor interrupt
  if (!useDummyData) {
    pinMode(FLOW_SENSOR_PIN, INPUT);
    digitalWrite(FLOW_SENSOR_PIN, HIGH);
    attachInterrupt(digitalPinToInterrupt(FLOW_SENSOR_PIN), pulseCounter, FALLING);
  }
  
  // Initial display
  updateDisplay();
  
  // Record start time
  oldTime = millis();
}

void loop() {
  // Keypad scanning
  char key = keypad.getKey();
  if (key) {
    handleKeypress(key);
  }
  
  // Generate dummy pulses if in test mode
  // if (useDummyData && (millis() - lastDummyPulse > dummyPulseInterval)) {
  //   pulseCount++;
  //   lastDummyPulse = millis();
  // }
  if (useDummyData) simulateFlowProfile();
  
  // Flow measurement processing
  if ((millis() - oldTime) > 1000) { // Only process counters once per second
    detachInterrupt(digitalPinToInterrupt(FLOW_SENSOR_PIN));
    
    // Calculate flow rate in L/min
    flowRate = (1000.0 / (millis() - oldTime)) * pulseCount / calibrationFactor;
    
    // Add to total volume (convert L/min to L/sec and multiply by time in sec)
    totalVolume += (flowRate / 60.0) * ((millis() - oldTime) / 1000.0);
    
    // Update total time in seconds
    totalTime += (millis() - oldTime) / 1000;
    
    // Reset counters
    pulseCount = 0;
    oldTime = millis();
    
    // Re-enable interrupt
    if (!useDummyData) {
      attachInterrupt(digitalPinToInterrupt(FLOW_SENSOR_PIN), pulseCounter, FALLING);
    }
  }
  
  // Detect current vessel
  detectVessel();
  
  // Update display periodically
  if (millis() - lastDisplayUpdate > displayUpdateInterval) {
    updateDisplay();
    lastDisplayUpdate = millis();
  }
}

// Interrupt service routine for flow sensor pulses
void pulseCounter() {
  pulseCount++;
}

// Detect vessel based on analog input
void detectVessel() {
  int sensorValue = analogRead(VESSEL_DETECT_PIN);
  int detectedVessel = 0;
  
  // Simple resistor ladder detection (adjust thresholds as needed)
  if (sensorValue < 50) detectedVessel = 0;
  else if (sensorValue < 200) detectedVessel = 1;
  else if (sensorValue < 400) detectedVessel = 2;
  else if (sensorValue < 600) detectedVessel = 3;
  else if (sensorValue < 800) detectedVessel = 4;
  else detectedVessel = 5;
  
  if (detectedVessel != currentVessel) {
    currentVessel = detectedVessel;
    // Optional: Reset total volume when vessel changes
    // totalVolume = 0;
    // totalTime = 0;
  }
}

// ---- Update LCD display (16×2) ----
// Row-0 : "F:xx.xL/min   V:y"
// Row-1 : "T:xxxL   t:mm:ss"
void updateDisplay() {
  char buf[7];

  /* -------- first row -------- */
  lcd.setCursor(0, 0);          // F:xx.xL/min
  lcd.print("F:");
  dtostrf(flowRate, 4, 1, buf); // width 4, 1 decimal  (-0.0 … 30.0)
  lcd.print(buf);
  lcd.print("L/min");

  lcd.setCursor(13, 0);         // ………V:y   (right-most)
  lcd.print("V:");
  lcd.print(currentVessel);     // 0-9

  /* -------- second row -------- */
  lcd.setCursor(0, 1);          // T:xxxL
  lcd.print("T:");
  int liters = static_cast<int>(totalVolume + 0.5f);   // round to whole litres
  if      (liters < 10)   lcd.print("  ");  // 2 leading spaces
  else if (liters < 100)  lcd.print(' ');   // 1 leading space
  lcd.print(liters);                     // 0-999
  lcd.print('L');

  lcd.setCursor(9, 1);          // ……t:mm:ss (right-aligned)
  uint16_t minutes = totalTime / 60;
  uint8_t  seconds = totalTime % 60;
  lcd.print("t:");
  if (minutes < 10) lcd.print('0');
  lcd.print(minutes);
  lcd.print(':');
  if (seconds < 10) lcd.print('0');
  lcd.print(seconds);
}

// Handle keypad input
void handleKeypress(char key) {
  switch (key) {
    case 'E': // ESC key
      // Stop flow and reset counters
      digitalWrite(SOLENOID_VALVE_PIN, LOW);
      totalVolume = 0;
      totalTime = 0;
      break;
    case '^': // Up arrow - increase flow (simulated)
      if (useDummyData) dummyPulseInterval = max(100, dummyPulseInterval - 100);
      break;
    case 'v': // Down arrow - decrease flow (simulated)
      if (useDummyData)
      dummyPulseInterval += 100;
      break;
    case 'F': // Function key
      // Toggle solenoid valve
      digitalWrite(SOLENOID_VALVE_PIN, !digitalRead(SOLENOID_VALVE_PIN));
      break;
    // Add other key handlers as needed
  }
}



// Simulates a 50s dummy flow profile: 0→30L/min in 20s, hold 10s, then 30→0 in 20s
void simulateFlowProfile() {
  static unsigned long profileStartTime = millis();
  unsigned long elapsed = millis() - profileStartTime;

  if (elapsed > 50000) {
    // After 50s, reset the simulation
    profileStartTime = millis();
    return;
  }

  float simulatedFlow = 0.0;
  if (elapsed <= 20000) {
    // Ramp up: 0 to 30L/min over 20s
    simulatedFlow = 30.0 * (elapsed / 20000.0);
  } else if (elapsed <= 30000) {
    // Hold: 30L/min for 10s
    simulatedFlow = 30.0;
  } else if (elapsed <= 50000) {
    // Ramp down: 30 to 0L/min over 20s
    simulatedFlow = 30.0 * (1.0 - ((elapsed - 30000.0) / 20000.0));
  }

  // Convert flow to pulse rate based on calibrationFactor
  float pulsesPerSecond = (simulatedFlow * calibrationFactor) / 60.0;
  static unsigned long lastPulseMicros = micros();
  unsigned long nowMicros = micros();

  if ((nowMicros - lastPulseMicros) >= (1000000.0 / pulsesPerSecond) && pulsesPerSecond > 0) {
    pulseCount++;
    lastPulseMicros = nowMicros;
  }
}

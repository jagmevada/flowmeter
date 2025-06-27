/**
 * @file FlowSensor.h
 * @author Hafidh Hidayat (hafidhhidayat@hotmail.com)
 * @brief 
 * 
 * @copyright Copyright (c) 2022
 * 
 * Github :
 * https://github.com/hafidhh
 * https://github.com/hafidhh/FlowSensor-Arduino
 */

#ifndef FlowSensor_h
#define FlowSensor_h

#include "Arduino.h"
#include "FlowSensor_Type.h"
#define NON_LINEAR 9999 // Arbitrary large unique value
/**
 * @brief FlowSensor Class > Sensor(uint8_t Type, uint8_t Pin)
 * 
 */
class FlowSensor
{
private:
	uint8_t _pin;
	bool _isNonLinear = false;
	volatile unsigned long _totalpulse;
	volatile unsigned long _pulse;
	float _pulse1liter;
	float _flowratehour;
	float _flowrateminute;
	float _flowratesecond;
	float _volume;
	unsigned long _timebefore = millis();
	float _multiplier = 15.0f;
	float _offset = 3.0f;

public:
	FlowSensor(uint16_t type, uint8_t pin);
	void setCalibration(float multiplier, float offset);
	void begin(void (*userFunc)(void), bool pullup = false);
	void count();
	void read(long calibration = 0);
	void setType(uint16_t type);
	void setPin(uint8_t pin);
	unsigned long getPulse();
	float getFlowRate_h();
	float getFlowRate_m();
	float getFlowRate_s();
	float getVolume();
	void resetPulse();
	void resetVolume();
};

#endif
/**
 * @file FlowSensor.cpp
 * @author Hafidh Hidayat (hafidhhidayat@hotmail.com)
 * @brief 
 * 
 * @copyright Copyright (c) 2022
 * 
 * Github :
 * https://github.com/hafidhh
 * https://github.com/hafidhh/FlowSensor-Arduino
 */

#include "FlowSensor.h"

/**
 * @brief Construct a new FlowSensor::FlowSensor object
 * 
 * @param type Sensor Type
 * @param pin Interrupt Pin
 * @param isNonLinear If the sensor is non-linear, set to true (1), otherwise false (0)	
 */
FlowSensor::FlowSensor(uint16_t type ,uint8_t pin)
{
	this->_pin = pin;
	this->_pulse1liter = type;
	this->_isNonLinear = (type == NON_LINEAR);  // Add this
}

/**
 * @brief setType, change type
 * 
 * @param type 
 */
void FlowSensor::setType(uint16_t type)
{
	this->_pulse1liter = type;
}

/**
 * @brief setPin, change pin
 * 
 * @param pin 
 */
void FlowSensor::setPin(uint8_t pin)
{
	detachInterrupt(this->_pin);
	this->_pin = pin;
}

/**
 * @brief sensor begin
 * 
 * @param userFunc 
 * @param pullup default value is fasle (0), if you have external pull up you can set to true (1)
 */
void FlowSensor::begin(void (*userFunc)(void), bool pullup)
{
	if (pullup == true)
		pinMode(this->_pin, INPUT);
	else
		pinMode(this->_pin, INPUT_PULLUP); // Optional Internal Pull-Up
	
	attachInterrupt(digitalPinToInterrupt(this->_pin), userFunc, RISING); // For better compatibility with any board, for example Arduino Leonardo Boards
}

/**
 * @brief count pulse
 * 
 */
void FlowSensor::count()
{
	this->_pulse++;
}

/**
 * @brief read sensor, get value
 * 
 * @param calibration Calibration pulse/liter
 */
void FlowSensor::read(long calibration)
{
	unsigned long now = millis();
	float deltaTime = (now - this->_timebefore) / 1000.0f; // in seconds
	this->_timebefore = now;
	if (_isNonLinear) {
		float freq = this->_pulse / deltaTime;
		this->_flowratesecond = ((_offset + freq) / _multiplier) / 60.0f;  // L/sec
		this->_volume = this->_flowratesecond *deltaTime;
	} else {
		this->_flowratesecond = (this->_pulse / (this->_pulse1liter + calibration)) / deltaTime;
		this->_volume = (this->_pulse / (this->_pulse1liter + calibration));
	}

	this->_totalpulse += this->_pulse;
	this->_pulse = 0;
	
}



/**
 * @brief get total pulse
 * 
 * @return unsigned long  _totalpulse
 */
unsigned long FlowSensor::getPulse()
{
	return this->_totalpulse;
}

/**
 * @brief get value flowrate/hour (L/h)
 * 
 * @return float flow rate / hour
 */
float FlowSensor::getFlowRate_h()
{
	this->_flowratehour = this->_flowratesecond * 3600;
	return this->_flowratehour;
}

/**
 * @brief get value flowrate/minute (L/m)
 * 
 * @return float flow rate / minute
 */
float FlowSensor::getFlowRate_m()
{
	this->_flowrateminute = this->_flowratesecond * 60;
	return this->_flowrateminute;
}

/**
 * @brief get value flowrate/second (L/s)
 * 
 * @return float flow rate / second
 */
float FlowSensor::getFlowRate_s()
{
	return this->_flowratesecond;
}

/**
 * @brief get Volume value (L)
 * 
 * @return float volume
 */
float FlowSensor::getVolume()
{
	return this->_volume;
}

/**
 * @brief Reset pulse count
 * 
 */
void FlowSensor::resetPulse()
{
	this->_pulse=0;
	this->_totalpulse=0;
}

/**
 * @brief reset Volume
 * 
 */
void FlowSensor::resetVolume()
{
	this->_volume = 0;
}

void FlowSensor::setCalibration(float multiplier, float offset)
{
	this->_multiplier = multiplier;
	this->_offset = offset;
}
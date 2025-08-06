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
	this->_volume = 0.0f;
	this->_totaltime =0.0f;
	this->_totalpulse =0L;
	
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
	this->_deltatime = (now - this->_timebefore) / MILLISPERSEC;
	this->_timebefore = now;
	if (_isNonLinear) {
	if (this->_pulse > 0 && this->_deltatime > 0) {
		float freq = this->_pulse / this->_deltatime;
			if (!isnan(freq) && freq > 0) {
				this->_flowratesecond = ((_offset + freq) / _multiplier) / 60.0f;
				this->_volume = this->_flowratesecond * this->_deltatime;
			} else {
				this->_flowratesecond = 0;
				this->_volume = 0;
			}
		} else {
			this->_flowratesecond = 0;
			this->_volume = 0;
		}

	} else {
		if (this->_deltatime > 0) {
			this->_flowratesecond = (this->_pulse / (this->_pulse1liter + calibration)) / this->_deltatime;
			this->_volume = (this->_pulse / (this->_pulse1liter + calibration));
		} else {
			this->_flowratesecond = 0;
			this->_volume = 0;
		}
	}

	this->_totaltime += this->_deltatime;
	this->_totalpulse += this->_pulse;
	this->_pulse = 0;
	
}


float FlowSensor::gettime(){
	return this->_deltatime;
}

void FlowSensor::resettime(){
	this->_totaltime = 0.0f;
}

float FlowSensor::gettotaltime(){
	return this->_totaltime;
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
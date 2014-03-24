/*
	FILE: 		idDHT11.cpp
	VERSION: 	0.1
	PURPOSE: 	Interrupt driven Lib for DHT11 with Arduino.
	LICENCE:	GPL v3 (http://www.gnu.org/licenses/gpl.html)
	DATASHEET: http://www.micro4you.com/files/sensor/DHT11.pdf
	
	Based on DHT11 library: http://playground.arduino.cc/Main/DHT11Lib
*/

#include "idDHT11.h"
#define DEBUG_IDDHT11

idDHT11::idDHT11(int pin, int intNumber,void (*callback_wrapper)()) {
	init(pin, intNumber,callback_wrapper);
}

void idDHT11::init(int pin, int intNumber, void (*callback_wrapper) ()) {
	this->intNumber = intNumber;
	this->pin = pin;
	this->isrCallback_wrapper = callback_wrapper;
	hum = 0;
	temp = 0;
	pinMode(pin, OUTPUT);
	digitalWrite(pin, HIGH);
	state = STOPPED;
	status = IDDHTLIB_ERROR_NOTSTARTED;
}

int idDHT11::acquire() {
	if (state == STOPPED || state == ACQUIRED) {
		
		//set the state machine for interruptions analisis of the signal
		state = RESPONSE;
		
		// EMPTY BUFFER and vars
		for (int i=0; i< 5; i++) bits[i] = 0;
		cnt = 7;
		idx = 0;
		hum = 0;
		temp = 0;
	
		// REQUEST SAMPLE
		pinMode(pin, OUTPUT);
		digitalWrite(pin, LOW);
		delay(18);
		digitalWrite(pin, HIGH);
		delayMicroseconds(40);
		pinMode(pin, INPUT);
		
		// Analize the data in an interrupt
		us = micros();
		attachInterrupt(intNumber,isrCallback_wrapper,FALLING);
		
		return IDDHTLIB_ACQUIRING;
	} else
		return IDDHTLIB_ERROR_ACQUIRING;
}
int idDHT11::acquireAndWait() {
	acquire();
	while(acquiring())
		;
	return getStatus();
}
void idDHT11::isrCallback() {
	int newUs = micros();
	int delta = (newUs-us);
	us = newUs;
	if (delta>6000) {
		status = IDDHTLIB_ERROR_ISR_TIMEOUT;
		state = STOPPED;
		detachInterrupt(intNumber);
		return;
	}
	switch(state) {
		case RESPONSE:
			if(delta<25){
				us -= delta;
				break; //do nothing, it started the response signal
			} if(125<delta && delta<190) {
				state = DATA;
			} else {
				detachInterrupt(intNumber);
				status = IDDHTLIB_ERROR_RESPONSE_TIMEOUT;
				state = STOPPED;
			}
			break;
		case DATA:
			if(delta<10) {
				detachInterrupt(intNumber);
				status = IDDHTLIB_ERROR_DELTA;
				state = STOPPED;
			} else if(60<delta && delta<155) { //valid in timing
				if(delta>90) //is a one
					bits[idx] |= (1 << cnt);
				if (cnt == 0) {  // whe have fullfilled the byte, go to next
						cnt = 7;    // restart at MSB
						if(idx++ == 4) {      // go to next byte, if whe have got 5 bytes stop.
							detachInterrupt(intNumber);
							// WRITE TO RIGHT VARS
							// as bits[1] and bits[3] are allways zero they are omitted in formulas.
							hum    = bits[0]; 
							temp = bits[2]; 
							uint8_t sum = bits[0] + bits[2];  
							if (bits[4] != sum) {
								status = IDDHTLIB_ERROR_CHECKSUM;
								state = STOPPED;
							} else {
								status = IDDHTLIB_OK;
								state = ACQUIRED;
							}
							break;
						}
				} else cnt--;
			} else {
				detachInterrupt(intNumber);
				status = IDDHTLIB_ERROR_DATA_TIMEOUT;
				state = STOPPED;
			}
			break;
		default:
			break;
	}
}
bool idDHT11::acquiring() {
	if (state != ACQUIRED && state != STOPPED)
		return true;
	return false;
}
int idDHT11::getStatus() {
	return status;
}
float idDHT11::getCelsius() {
	IDDHT11_CHECK_STATE;
	return temp;
}

float idDHT11::getHumidity() {
	IDDHT11_CHECK_STATE;
	return hum;
}

float idDHT11::getFahrenheit() {
	IDDHT11_CHECK_STATE;
	return temp * 1.8 + 32;
}

float idDHT11::getKelvin() {
	IDDHT11_CHECK_STATE;
	return temp + 273.15;
}

// delta max = 0.6544 wrt dewPoint()
// 5x faster than dewPoint()
// reference: http://en.wikipedia.org/wiki/Dew_point
double idDHT11::getDewPoint() {
	IDDHT11_CHECK_STATE;
	double a = 17.271;
	double b = 237.7;
	double temp_ = (a * (double) temp) / (b + (double) temp) + log( (double) hum/100);
	double Td = (b * temp_) / (a - temp_);
	return Td;
	
}
// dewPoint function NOAA
// reference: http://wahiduddin.net/calc/density_algorithms.htm 
double idDHT11::getDewPointSlow() {
	IDDHT11_CHECK_STATE;
	double A0= 373.15/(273.15 + (double) temp);
	double SUM = -7.90298 * (A0-1);
	SUM += 5.02808 * log10(A0);
	SUM += -1.3816e-7 * (pow(10, (11.344*(1-1/A0)))-1) ;
	SUM += 8.1328e-3 * (pow(10,(-3.49149*(A0-1)))-1) ;
	SUM += log10(1013.246);
	double VP = pow(10, SUM-3) * (double) hum;
	double T = log(VP/0.61078);   // temp var
	return (241.88 * T) / (17.558-T);
}
// EOF

#include "DriverProfile.h"

DriverProfile::DriverProfile() {

}

DriverProfile::DriverProfile(string name, int maxSpeed, int maxDR, int accPlus, int accMinus, float probability, int probThreshold) {
    this->name = name;

    this->maxSpeed = maxSpeed;
	this->maxDR = maxDR;
	this->dR = ((float)maxDR / (float)maxSpeed);
	this->accPlus = accPlus;
    this->accMinus = accMinus;

    this->probability = probability;
    this->probThreshold = probThreshold;
}

DriverProfile::~DriverProfile() {

}


void DriverProfile::SetName(string name) {
    this->name = name;
}

string DriverProfile::GetName() {
    return name;
}

void DriverProfile::SetMaxSpeed(int maxSpeed) {
	this->maxSpeed = maxSpeed;
}

int DriverProfile::GetMaxSpeed() {
	return maxSpeed;
}

void DriverProfile::SetMaxDR(int maxDR) {
	this->maxDR = maxDR;
}

int DriverProfile::GetMaxDR() {
	return maxDR;
}

void DriverProfile::SetDR(float dR) {
	this->dR = dR;
}

float DriverProfile::GetDR() {
	return dR;
}

void DriverProfile::SetAccPlus(int accPlus) {
	this->accPlus = accPlus;
}

int DriverProfile::GetAccPlus() {
	return accPlus;
}

void DriverProfile::SetAccMinus(int accMinus) {
	this->accMinus = accMinus;
}

int DriverProfile::GetAccMinus() {
	return accMinus;
}

void DriverProfile::SetProbability(float probability) {
    this->probability = probability;
}

float DriverProfile::GetProbability() {
    return probability;
}

void DriverProfile::SetProbThreshold(int probThreshold) {
    this->probThreshold = probThreshold;
}

int DriverProfile::GetProbThreshold() {
    return probThreshold;
}

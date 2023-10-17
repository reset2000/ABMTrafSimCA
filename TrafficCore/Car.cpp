#include "Car.h"

#define CAR_METERS 1

long Car::nextID = 0;

Car::Car() {

}

Car::Car(int speed, double carLength) {
	this->speed = speed;
	carID = ++nextID;
	stopChangingLane = false;
	tailSegmentsNum = carLength - 1;

    this->driverType = -1;
}

Car::~Car() {

}

long Car::GetCarID() {
	return carID;
}

void Car::SetCarSpeed(int newSpeed) {
	this->speed = newSpeed;
}

int Car::GetCarSpeed() {
	return speed;
}

void Car::SetStopChangingLane(bool stopChangingLane) {
	this->stopChangingLane = stopChangingLane;
}

bool Car::GetStopChangingLane() {
	return stopChangingLane;
}

int Car::GetTailSegmentsNum() {
	return tailSegmentsNum;
}

void Car::SetDriverType(int driverType) {
	this->driverType = driverType;
}

int Car::GetDriverType() {
	return driverType;
}

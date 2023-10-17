#pragma once

#include <vector>
#include "Configuration.h"

using namespace std;

class Car {
private:
	static long nextID;

	long carID;
	int speed;
	bool stopChangingLane;
	int tailSegmentsNum;
    int driverType;
public:
	Car();
	Car(int speed, double carLength);
	~Car();

	long GetCarID();

	void SetCarSpeed(int newSpeed);
	int GetCarSpeed();

	void SetStopChangingLane(bool stopChangingLane);
	bool GetStopChangingLane();

	int GetTailSegmentsNum();

    void SetDriverType(int driverType);
    int GetDriverType();
};

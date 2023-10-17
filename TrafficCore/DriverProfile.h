#pragma once

#include <string>

using namespace std;

class DriverProfile {
private:
    string name;

    int maxSpeed;
	int maxDR;
    float dR;
    int accPlus;
    int accMinus;

    float probability;
    int probThreshold;
public:
	DriverProfile();
    DriverProfile(string name, int maxSpeed, int dR, int accPlus, int accMinus, float probability, int probThreshold);
	~DriverProfile();

    void SetName(string name);
    string GetName();

	void SetMaxSpeed(int maxSpeed);
	int GetMaxSpeed();

	void SetMaxDR(int maxDR);
	int GetMaxDR();

	void SetDR(float dR);
	float GetDR();

	void SetAccPlus(int accPlus);
	int GetAccPlus();

	void SetAccMinus(int accMinus);
	int GetAccMinus();

    void SetProbability(float probability);
    float GetProbability();

    void SetProbThreshold(int probThreshold);
    int GetProbThreshold();
};


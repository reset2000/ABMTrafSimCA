#include "Configuration.h"
#include "DriverProfile.h"

Configuration::Configuration(bool calculateDensities) {
	for (int i = 1; i < 100; i++) {
		AddDensity(i);
	}

	segmentsNum = 200;
	currentDensity = 50;
	this->calculateDensities = calculateDensities;
	useChangeLane = true;
	cyclesToSkip = 50;
	cyclesNum = 200 + cyclesToSkip;
	testsNumber = 1;
    data = "";
}

Configuration::~Configuration() {
	for (int i = 0; i < MAX_NUMBER_OF_DRIVER_TYPES; i++) {
		delete driverProfiles[i];
	}
}

void Configuration::AddShowFunction(SHOW_FUNCTION_PTR showFunctionPtr) {
	showFunctions.push_back(showFunctionPtr);
}

void Configuration::ClearShowFunctions() {
	showFunctions.clear();
}

int Configuration::GetShowFunctionsSize() {
	return showFunctions.size();
}

SHOW_FUNCTION_PTR Configuration::GetShowFunctionsElement(int idx) {
	return showFunctions.at(idx);
}

void Configuration::SetSegmentsNum(int segmentsNum) {
	this->segmentsNum = segmentsNum;
}

int Configuration::GetSegmentsNum() {
	return segmentsNum;
}

void Configuration::SetCurrentDensity(int currentDensity) {
	this->currentDensity = currentDensity;
}

int Configuration::GetCurrentDensity() {
	return currentDensity;
}

void Configuration::SetCalculateDensities(bool calculateDensities) {
	this->calculateDensities = calculateDensities;
}

bool Configuration::GetCalculateDensities() {
	return calculateDensities;
}

void Configuration::SetUseChangeLane(bool useChangeLane) {
	this->useChangeLane = useChangeLane;
}

bool Configuration::GetUseChangeLane() {
	return useChangeLane;
}

bool Configuration::AddCarProbability(int carLength, float probabilty) {
	if (carLength < 0 || carLength > MAX_CARS_SET) {
        throw "Car probablility index not valid.";
    }

	carProbability[carLength] = probabilty;

	return false;
}

float Configuration::GetCarProbability(int carLength) {
	if (carLength < 0 || carLength > MAX_CARS_SET) {
        throw "Car probablility index not valid.";
	}

	return carProbability[carLength];
}

void Configuration::ClearProbabilities() {
	for (int i = 0; i < MAX_CARS_SET; i++) {
		carProbability[i] = 0;
	}
}

void Configuration::SetCyclesToSkip(int cyclesToSkip) {
	this->cyclesToSkip = cyclesToSkip;
}

int Configuration::GetCyclesToSkip() {
	return cyclesToSkip;
}

void Configuration::SetCyclesNum(int cyclesNum) {
	this->cyclesNum = cyclesNum;
}

int Configuration::GetCyclesNum() {
	return cyclesNum;
}

void Configuration::AddDensity(int density) {
	return densities.push_back(density);
}

void Configuration::ClearDensities() {
	return densities.clear();
}

int Configuration::GetDensitiesElement(int idx) {
	return densities.at(idx);
}

int Configuration::GetDensitiesSize() {
	return densities.size();
}

void Configuration::SetTestsNumber(int testsNumber) {
	this->testsNumber = testsNumber;
}

int Configuration::GetTestsNumber() {
	return testsNumber;
}

void Configuration::SetVDriverType(DriverProfile *profile) {
    vDriverProfiles.push_back(*profile);
}

float Configuration::GetDriverTypeProbability(int driverTypeIndex) {
    if (driverTypeIndex < 0 || driverTypeIndex >= MAX_NUMBER_OF_DRIVER_TYPES) {
        throw "Driver type index not valid.";
	}

    return vDriverProfiles[driverTypeIndex].GetProbability();
}

DriverProfile *Configuration::GetDriverProfile(int driverTypeIndex) {
    if (driverTypeIndex < 0 || driverTypeIndex >= MAX_NUMBER_OF_DRIVER_TYPES) {
        throw "Driver type index not valid.";
	}

    return &vDriverProfiles[driverTypeIndex];
}

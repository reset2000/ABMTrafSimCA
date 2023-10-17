#pragma once

#include <string>
#include <vector>

#include "Consts.h"
#include "DriverProfile.h"

#define MAX_SHOW_FUNCTIONS 10

using namespace std;

enum ShowSection {
    SHOW_INIT = 0,
    SHOW_MID = 1,
    SHOW_FINAL = 2
};

class Configuration;
typedef void (*SHOW_FUNCTION_PTR)(Configuration &cfg, string *cycle, ShowSection section);

class Configuration {
private:
    vector<SHOW_FUNCTION_PTR> showFunctions;
    vector<int> densities;

    int cyclesNum;
    int cyclesToSkip;
    int segmentsNum;

    int currentDensity;
    bool calculateDensities;
    int testsNumber;

    bool useChangeLane;
    float carProbability[MAX_CARS_SET] = {0};

    float driverTypeProbabiltities[MAX_NUMBER_OF_DRIVER_TYPES] = {0.0};
    DriverProfile *driverProfiles[MAX_NUMBER_OF_DRIVER_TYPES] = {NULL};

public:
    string data;

    vector<DriverProfile> vDriverProfiles;

    Configuration(bool calculateDensities = false);
    ~Configuration();

    void AddShowFunction(SHOW_FUNCTION_PTR showFunctionPtr);
    void ClearShowFunctions();
    int GetShowFunctionsSize();
    SHOW_FUNCTION_PTR GetShowFunctionsElement(int idx);

    void SetSegmentsNum(int segmentsNum);
    int GetSegmentsNum();

    void SetCurrentDensity(int startingCars);
    int GetCurrentDensity();

    void SetCalculateDensities(bool calculateDensities);
    bool GetCalculateDensities();

    void SetUseChangeLane(bool useChangeLane);
    bool GetUseChangeLane();

    bool AddCarProbability(int carLength, float probabilty);
    float GetCarProbability(int carLength);
    void ClearProbabilities();

    void SetCyclesToSkip(int cyclesToSkip);
    int GetCyclesToSkip();

    void SetCyclesNum(int cyclesNum);
    int GetCyclesNum();

    void AddDensity(int density);
    void ClearDensities();
    int GetDensitiesElement(int idx);
    int GetDensitiesSize();

    void SetTestsNumber(int testsNumber);
    int GetTestsNumber();

    void SetVDriverType(DriverProfile *profile);
    float GetDriverTypeProbability(int driverType);
    DriverProfile *GetDriverProfile(int driverType);
};

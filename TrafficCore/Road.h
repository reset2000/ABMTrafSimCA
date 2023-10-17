#pragma once

#include <string>
#include <map>

#include "RoadSegment.h"
#include "Car.h"
#include "Consts.h"
#include "Configuration.h"

using namespace std;

class Road {
public:
	typedef struct {
		int vanishedCars;
		int changeLane;
	} CARS_STATS_T;

	Road(Configuration &config);
	~Road();

	void GenerateCycles();
    static void GenerateDensities(Configuration &config);
    void GenerateDensitiesQt(Configuration &config);
	int GetVanishedCars(int lane, int carLength);
private:
	Configuration *cfg;

	RoadSegment **startSegments;
	RoadSegment **cycleSegments;

	map<long, Car> cars;
    int carsSet[MAX_ROAD_LANES][MAX_CARS_SET] = {0};

	int segmentsNum;
    int carsNum[MAX_ROAD_LANES] = {0};

    CARS_STATS_T vanishedCars[MAX_ROAD_LANES][MAX_CARS_SET] = {0};
	int sumChangedLanes;

	void SetCarsSet();

	void SetStartingState(bool densities);

	int CalculateCarsAfter(int allCars);

    int GenerateRandomDriverTypeIndex();
    int GenerateRandomStartingCar();
	void SetTails();

	void SetAccelerateByFormula();
	void SetChangeLaneByFormula();
	int CheckFrontSameLane(int lane, int segment);
	int CheckFrontAltLane(int lane, int segment);
	int CheckBackAltLane(int lane, int segment);
	int CalculateDistanceBetweenCars(int segment1, int segment2, int tail2);

	void MoveSingleCarChangeLane(int oldLane, int laneToChange, int segment);
	void ClearStopChangingLane();

	void MoveCars();

	string *PrepareToShow(int cyclesNum);
};

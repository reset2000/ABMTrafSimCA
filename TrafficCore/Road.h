#pragma once

#include <string>
#include <map>

#include "RoadSegment.h"
#include "Car.h"
#include "Consts.h"
#include "Configuration.h"

using namespace std;

///
/// The Road class is the main component of the simulation engine. It contains all the functions that are called during this process.
/// It also includes all the necessary components, such as road created out of semgents and cars that are driving through the lanes.
/// \brief The Road class contains all the functions neccessary to carry out the simulation
///
class Road {
public:
    ///
    /// \brief Describes the parameters that are measured during the simulation
    ///
	typedef struct {
		int vanishedCars;
		int changeLane;
	} CARS_STATS_T;

    ///
    /// \brief The constructor of the Road class
    /// \param config - simulations initial configuration
    /// \see Configuration
    ///
	Road(Configuration &config);
    ///
    /// \brief The destructor of the Road class
    ///
	~Road();

    ///
    /// \brief Main method that carries out the simulation
    ///
	void GenerateCycles();
    ///
    /// \brief The method thar packages individual calls of GenerateCycles method for each density point of the fundamental diagram
    /// \see GenerateCycles
    /// \param config - simulations initial configuration
    /// \see Configuration
    ///
    static void GenerateDensities(Configuration &config);
    ///
    /// \brief The method thar packages individual calls of GenerateCycles method for each density point of the fundamental diagram optimized for Qt Charts
    /// \see GenerateCycles
    /// \param config - simulations initial configuration
    /// \see Configuration
    ///
    void GenerateDensitiesQt(Configuration &config);
    ///
    /// \brief The getter of the vanished cars field from CARS_STATS_T structure
    /// \see CARS_STATS_T
    /// \param lane - the specified lane for which the vanished cars were counted
    /// \param carLength - the specified length of the cars for which the number of vanished vehicles was counted
    /// \return Number of cars that left the road during the simulation (vanished cars)
    ///
    int GetVanishedCars(int lane, int carLength);
private:
    ///
    /// \brief Initial configuration of the simulation
    ///
	Configuration *cfg;

    ///
    /// \brief A dynamic array representing the starting state of the road (with initial car placements)
    ///
	RoadSegment **startSegments;
    ///
    /// \brief A dynamic array representing the following states of the road (iterations)
    ///
	RoadSegment **cycleSegments;

    ///
    /// \brief Map containing all generated cars
    ///
	map<long, Car> cars;
    ///
    /// \brief Set of initial cars generated at the start of the simulation
    ///
    int carsSet[MAX_ROAD_LANES][MAX_CARS_SET] = {0};

    ///
    /// \brief Number of road segments
    ///
	int segmentsNum;
    ///
    /// \brief Number of cars per road lane
    ///
    int carsNum[MAX_ROAD_LANES] = {0};

    ///
    /// \brief Instance of the structure containing number of cars that crossed the road during the simulation
    ///
    CARS_STATS_T vanishedCars[MAX_ROAD_LANES][MAX_CARS_SET] = {0};
    ///
    /// \brief Number of times cars changed lanes during the simulation
    ///
	int sumChangedLanes;

    ///
    /// \brief Method that sets up the initial car array
    ///
	void SetCarsSet();

    ///
    /// \brief Method that sets up the initial state of the road
    /// \param densities - parameter that determines if the simulation generates space-time or fundamental diagram
    ///
	void SetStartingState(bool densities);

    ///
    /// \brief Method that counts the number of cars after they are alredy placed on the road and after every iteration
    /// \param allCars - initial number of cars
    /// \return Number of cars after the count
    ///
	int CalculateCarsAfter(int allCars);

    ///
    /// \brief Method that randomly generates indices for driver types based on constraints from the configuration
    /// \return Randomly generated driver type index
    ///
    int GenerateRandomDriverTypeIndex();
    ///
    /// \brief Method that randomly generates car lengths based on constraints from the configuration
    /// \return Randomly generated car length
    ///
    int GenerateRandomStartingCar();
    ///
    /// \brief Method that iterates over the initial state of the road and marks all segments that contain any part of a car
    ///
	void SetTails();

    ///
    /// \brief Method that accelerates cars based on LAI model formula
    ///
	void SetAccelerateByFormula();
    ///
    /// \brief Method that checks for potential car lane changes based on LAI model formulas
    ///
	void SetChangeLaneByFormula();
    ///
    /// \brief Method that checks LAI model constraits for changing lanes (looking for preceding cars on the same lane)
    /// \param lane - lane that is checked
    /// \param segment - the first segment on which the currently examined car is placed
    /// \return Number of segments between currently examined car and preceding car on the same lane
    ///
	int CheckFrontSameLane(int lane, int segment);
    ///
    /// \brief Method that checks LAI model constraits for changing lanes (looking for preceding cars on the other lane)
    /// \param lane - lane that is checked
    /// \param segment - the first segment on which the currently examined car is placed
    /// \return Number of segments between currently examined car and preceding car on the other lane
    ///
	int CheckFrontAltLane(int lane, int segment);
    ///
    /// \brief Method that checks LAI model constraits for changing lanes (looking for succeeding cars on the other lane)
    /// \param lane - lane that is checked
    /// \param segment - the first segment on which the currently examined car is placed
    /// \return Number of segments between currently examined car and succeeding car on the other lane
    ///
	int CheckBackAltLane(int lane, int segment);
    ///
    /// \brief Method that calculates distance between two cars
    /// \param segment1 - the first segment on which the first car is placed
    /// \param segment2 - the first segment on which the second car is placed
    /// \param tail2 - length of the second car
    /// \return Distance between two cars
    ///
	int CalculateDistanceBetweenCars(int segment1, int segment2, int tail2);

    ///
    /// \brief Method that moves the car who was marked as one that needs to change lane during the previous step
    /// \param oldLane - the lane the car currently is on
    /// \param laneToChange - new lane the car will be placed on
    /// \param segment - current segment, on which the car is standing
    ///
    void MoveSingleCarChangeLane(int oldLane, int laneToChange, int segment);
    ///
    /// \brief Method that clears the lane change flags for cars after all vehicles are moved
    ///
    void ClearStopChangingLane();

    ///
    /// \brief Method that moves every car according to their speed at the end of each iteration
    ///
	void MoveCars();

    ///
    /// \brief Method that prepares the string with the road cycles for further analysis
    /// \param cyclesNum - number of iterations of the simulation
    /// \return
    ///
	string *PrepareToShow(int cyclesNum);
};

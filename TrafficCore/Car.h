#pragma once

#include <vector>

using namespace std;

///
/// \brief The Car class decribes the structure of a single car on the road
///
class Car {
private:
    ///
    /// \brief The static identifier of the next car to be added to the road
    ///
	static long nextID;

    ///
    /// \brief The identifier of a signle car
    ///
	long carID;
    ///
    /// \brief The speed value of a car
    ///
	int speed;
    ///
    /// \brief The value that determines if the car can change lanes or not
    ///
	bool stopChangingLane;
    ///
    /// \brief The number of segments of the car (excluding first segment - head)
    ///
	int tailSegmentsNum;
    ///
    /// \brief The value representing assigned driver type value
    ///
    int driverType;
public:
    ///
    /// \brief The default constructor of the Car class
    ///
	Car();
    ///
    ///
    /// \brief The overloaded constructor of the Car class
    /// \param speed - speed value of the vehicle (in segments)
    /// \param carLength - the length of the car
    ///
	Car(int speed, double carLength);
    ///
    /// \brief The destructor of the Car class
    ///
	~Car();

    ///
    /// \brief The getter of the carID field
    /// \return carID
    /// \see carID
    ///
    long GetCarID();

    ///
    /// \brief The setter of the speed field
    /// \param newSpeed - new speed of the car
    /// \see speed
    ///
    void SetCarSpeed(int newSpeed);
    ///
    /// \brief The getter of the speed field
    /// \return speed
    /// \see speed
    ///
    int GetCarSpeed();

    ///
    /// \brief The setter of the stopChangingLane field
    /// \param stopChangingLane
    /// \see stopChangingLane
    ///
    void SetStopChangingLane(bool stopChangingLane);
    ///
    /// \brief The getter of the stopChangingLane field
    /// \return stopChangingLane
    /// \see stopChangingLane
    ///
    bool GetStopChangingLane();

    ///
    /// \brief The getter of the tailSegmentsNum field
    /// \return tailSegmentsNum
    /// \see tailSegmentsNum
    ///
	int GetTailSegmentsNum();

    ///
    /// \brief The setter of the driverType field
    /// \param driverType
    /// \see driverType
    ///
    void SetDriverType(int driverType);
    ///
    /// \brief The getter of the driverType field
    /// \return driverType
    /// \see driverType
    ///
    int GetDriverType();
};

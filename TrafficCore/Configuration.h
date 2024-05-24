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

///
/// \brief The Configuration class describes configuration parameters of the core simulation module
///
class Configuration {
private:
    ///
    /// \brief Vector with all the functions that are responsible for gathering data necessary to display diagrams
    ///
    vector<SHOW_FUNCTION_PTR> showFunctions;
    ///
    /// \brief Vector with all avaliable density points
    ///
    vector<int> densities;

    ///
    /// \brief The number of iterations of the simulation
    ///
    int cyclesNum;
    ///
    /// \brief The number of iterations that will not be displayed
    /// \note These iterations will not be displayed, but the simulation is still generated for them, allowing cars to spread out
    ///
    int cyclesToSkip;
    ///
    /// \brief The length of the road in segments
    ///
    int segmentsNum;

    ///
    /// \brief The road density percentage.
    /// \note Density represents the percentage value of the cars on the road during the whole simulation (closed loop)
    ///
    int currentDensity;
    ///
    /// \brief The value determining if the result of the simulation will be space-time or fundamental diagram
    ///
    bool calculateDensities;
    ///
    /// \brief The number of simulations that are averaged for a single density point on the fundamental diagram
    ///
    int testsNumber;

    ///
    /// \brief The saved value defining if the lane changes will occur during the simulation
    ///
    bool useChangeLane;
    ///
    /// \brief Array with probabilites for generating car with random lengths
    ///
    float carProbability[MAX_CARS_SET] = {0};

    ///
    /// \brief Array with probabilites for assigning random driver types to cars
    ///
    float driverTypeProbabiltities[MAX_NUMBER_OF_DRIVER_TYPES] = {0.0};
    ///
    /// \brief Array with all avaliable driver types (depricated)
    /// \see DriverProfile
    ///
    DriverProfile *driverProfiles[MAX_NUMBER_OF_DRIVER_TYPES] = {NULL};

public:
    ///
    /// \brief Simulation data in the form of a string
    ///
    string data;

    ///
    /// \brief Vector containing driver profiles
    /// \see DriverProfile
    ///
    vector<DriverProfile> vDriverProfiles;

    ///
    /// \brief The constructor of the Configuration class
    /// \param calculateDensities - parameter determining if the space-time or fundamental diagram will be generated
    ///
    Configuration(bool calculateDensities = false);
    ///
    /// \brief The destructor of the Configuration class
    ///
    ~Configuration();

    ///
    /// \brief Method that adds another function preparing display data (show function) to the array
    /// \param showFunctionPtr - pointer to the show function
    /// \see showFunctions
    ///
    void AddShowFunction(SHOW_FUNCTION_PTR showFunctionPtr);
    ///
    /// \brief Method that clears showFunctions vector
    /// \see showFunctions
    ///
    void ClearShowFunctions();
    ///
    /// \brief Methods that calculates the size of showFunctions vector
    /// \see showFunctions
    /// \return Number of elements in showFunctions vector
    ///
    int GetShowFunctionsSize();
    ///
    /// \brief Method that gets chosen showFunctions vector element by index
    /// \see showFunctions
    /// \param idx - index of chosen show function
    /// \return Chosen show function
    ///
    SHOW_FUNCTION_PTR GetShowFunctionsElement(int idx);

    ///
    /// \brief The setter of the segmentsNum field
    /// \param segmentsNum
    /// \see segmentsNum
    ///
    void SetSegmentsNum(int segmentsNum);
    ///
    /// \brief The getter of the segmentsNum field
    /// \return segmentsNum
    /// \see segmentsNum
    ///
    int GetSegmentsNum();

    ///
    /// \brief The setter of the currentDensity field
    /// \param currentDensity
    /// \see currentDensity
    ///
    void SetCurrentDensity(int currentDensity);
    ///
    /// \brief The getter of the currentDensity field
    /// \return currentDensity
    /// \see currentDensity
    ///
    int GetCurrentDensity();

    ///
    /// \brief The setter of the calculateDensities field
    /// \param calculateDensities
    /// \see calculateDensities
    ///
    void SetCalculateDensities(bool calculateDensities);
    ///
    /// \brief The getter of the calculateDensities field
    /// \return calculateDensities
    /// \see calculateDensities
    ///
    bool GetCalculateDensities();

    ///
    /// \brief The setter of the useChangeLane field
    /// \param useChangeLane
    /// \see useChangeLane
    ///
    void SetUseChangeLane(bool useChangeLane);
    ///
    /// \brief The getter of the useChangeLane field
    /// \return useChangeLane
    /// \see useChangeLane
    ///
    bool GetUseChangeLane();

    ///
    /// \brief Method that adds new car probability for a specified length to the carProbability array
    /// \see carProbability
    /// \param carLength - length of a car in segments
    /// \param probabilty - probability of choosing car of a specified length
    /// \return Flag that indicates if the operation was successful or not
    ///
    bool AddCarProbability(int carLength, float probabilty);
    ///
    /// \brief The getter of the carProbability field of the specified length
    /// \param carLength - length of a car in segments
    /// \see carProbability
    ///
    float GetCarProbability(int carLength);
    ///
    /// \brief Method that clears all car probabilities from carProbability array
    /// \see carProbability
    ///
    void ClearProbabilities();

    ///
    /// \brief The setter of the cyclesToSkip field
    /// \param cyclesToSkip
    /// \see cyclesToSkip
    ///
    void SetCyclesToSkip(int cyclesToSkip);
    ///
    /// \brief The getter of the cyclesToSkip field
    /// \return cyclesToSkip
    /// \see cyclesToSkip
    ///
    int GetCyclesToSkip();

    ///
    /// \brief The setter of the cyclesNum field
    /// \param cyclesNum
    /// \see cyclesNum
    ///
    void SetCyclesNum(int cyclesNum);
    ///
    /// \brief The getter of the cyclesNum field
    /// \return cyclesNum
    /// \see cyclesNum
    ///
    int GetCyclesNum();

    ///
    /// \brief Methods that add new density to the densities array
    /// \see densities
    /// \param densities
    ///
    void AddDensity(int density);
    ///
    /// \brief Method that clears all densities from from densities array
    /// \see densities
    ///
    void ClearDensities();
    ///
    /// \brief The getter of the isTail field
    /// \return isTail
    /// \see isTail
    ///
    int GetDensitiesElement(int idx);
    ///
    /// \brief Method that calculates number of densities in densities array
    /// \return Number of densities in densities array
    /// \see densities
    ///
    int GetDensitiesSize();

    ///
    /// \brief The setter of the testsNumber field
    /// \param testsNumber
    /// \see testsNumber
    ///
    void SetTestsNumber(int testsNumber);
    ///
    /// \brief The getter of the testsNumber field
    /// \return testsNumber
    /// \see testsNumber
    ///
    int GetTestsNumber();

    ///
    /// \brief The setter of the vDriverProfiles field
    /// \param vDriverProfiles
    /// \see vDriverProfiles
    ///
    void SetVDriverType(DriverProfile *profile);
    ///
    /// \brief The getter of the vDriverProfiles probability by a specified index
    /// \param driverType - index of the specified driver type
    /// \return Probability from vDriverProfiles vector
    /// \see vDriverProfiles
    ///
    float GetDriverTypeProbability(int driverType);
    ///
    /// \brief The getter of the vDriverProfiles driver type by a specified index
    /// \see vDriverProfiles
    /// \param driverType - index of the specified driver type
    /// \return Specified driver type object
    /// \see DriverProfile
    ///
    DriverProfile *GetDriverProfile(int driverType);
};

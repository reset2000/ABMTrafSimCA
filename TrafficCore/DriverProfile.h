#pragma once

#include <string>

using namespace std;

///
/// \brief The DriverProfile class describes all the parameters that driver profiles consist of
///
class DriverProfile {
private:
    ///
    /// \brief Display name of the drive profile
    ///
    string name;

    ///
    /// \brief The maximum speed that driver specified by the profile can achieve
    ///
    int maxSpeed;
    ///
    /// \brief The maximum distance between other cars that driver specified by the profile (depricated)
    ///
	int maxDR;
    ///
    /// \brief The maximum distance between other cars that driver specified by the profile
    ///
    float dR;
    ///
    /// \brief The maximum acceleration value of the driver specified by the profile
    ///
    int accPlus;
    ///
    /// \brief The maximum distance between other cars that driver specified by the profile
    ///
    int accMinus;

    ///
    /// \brief The probability of choosing specified driver profile
    ///
    float probability;
    ///
    /// \brief Converted probability thresholds (out of 1000)
    /// \see probability
    ///
    int probThreshold;
public:
    ///
    /// \brief The default constructor of the RoadSegment class
    ///
	DriverProfile();
    ///
    /// \brief The overloaded constructor of the DriverProfile class
    /// \param name
    /// \param maxSpeed
    /// \param dR
    /// \param accPlus
    /// \param accMinus
    /// \param probability
    /// \param probThreshold
    /// \see probThreshold
    /// \see name
    /// \see maxSpeed
    /// \see dR
    /// \see accPlus
    /// \see accMinus
    /// \see probability
    ///
    DriverProfile(string name, int maxSpeed, int dR, int accPlus, int accMinus, float probability, int probThreshold);
    ///
    /// \brief The destructor of the Car class
    ///
	~DriverProfile();

    ///
    /// \brief The setter of the name field
    /// \param name
    /// \see name
    ///
    void SetName(string name);
    ///
    /// \brief The getter of the name field
    /// \return name
    /// \see name
    ///
    string GetName();

    ///
    /// \brief The setter of the maxSpeed field
    /// \param maxSpeed
    /// \see maxSpeed
    ///
    void SetMaxSpeed(int maxSpeed);
    ///
    /// \brief The getter of the maxSpeed field
    /// \return maxSpeed
    /// \see maxSpeed
    ///
	int GetMaxSpeed();

    ///
    /// \brief The setter of the maxDR field
    /// \param maxDR
    /// \see maxDR
    ///
    void SetMaxDR(int maxDR);
    ///
    /// \brief The getter of the maxDR field
    /// \return maxDR
    /// \see maxDR
    ///
	int GetMaxDR();

    ///
    /// \brief The setter of the dR field
    /// \param dR
    /// \see dR
    ///
    void SetDR(float dR);
    ///
    /// \brief The getter of the dR field
    /// \return dR
    /// \see dR
    ///
	float GetDR();

    ///
    /// \brief The setter of the accPlus field
    /// \param accPlus
    /// \see accPlus
    ///
    void SetAccPlus(int accPlus);
    ///
    /// \brief The getter of the accPlus field
    /// \return accPlus
    /// \see accPlus
    ///
	int GetAccPlus();

    ///
    /// \brief The setter of the accMinus field
    /// \param accMinus
    /// \see accMinus
    ///
    void SetAccMinus(int accMinus);
    ///
    /// \brief The getter of the accMinus field
    /// \return accMinus
    /// \see accMinus
    ///
	int GetAccMinus();

    ///
    /// \brief The setter of the probability field
    /// \param probability
    /// \see probability
    ///
    void SetProbability(float probability);
    ///
    /// \brief The getter of the probability field
    /// \return probability
    /// \see probability
    ///
    float GetProbability();

    ///
    /// \brief The setter of the probThreshold field
    /// \param probThreshold
    /// \see probThreshold
    ///
    void SetProbThreshold(int probThreshold);
    ///
    /// \brief The getter of the probThreshold field
    /// \return probThreshold
    /// \see probThreshold
    ///
    int GetProbThreshold();
};


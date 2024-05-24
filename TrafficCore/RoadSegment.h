#pragma once

///
/// \brief The RoadSegment class describes single road segment
///
class RoadSegment {
private:
    ///
    /// \brief The car identifier tied to a road segment
    ///
	long carID;
    ///
    /// \brief The value that determines if a road segment is part of the car or not (except car's first segment)
    ///
	bool isTail;
public:
    ///
    /// \brief The constructor of the RoadSegment class
    ///
	RoadSegment();
    ///
    /// \brief The destructor of the RoadSegment class
    ///
	~RoadSegment();

    ///
    /// \brief The setter of the carID field
    /// \param carID
    /// \see carID
    ///
	void SetCarID(long carID);
    ///
    /// \brief The getter of the carID field
    /// \return carID
    /// \see carID
    ///
	long GetCarID();

    ///
    /// \brief The setter of the isTail field
    /// \param isTail
    /// \see isTail
    ///
	void SetIsTail(bool isTail);
    ///
    /// \brief The getter of the isTail field
    /// \return isTail
    /// \see isTail
    ///
	bool GetIsTail();
};

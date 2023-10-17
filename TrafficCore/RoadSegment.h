#pragma once

class RoadSegment {
private:
	long carID;
	bool isTail;
public:
	RoadSegment();
	~RoadSegment();

	void SetCarID(long carID);
	long GetCarID();

	void SetIsTail(bool isTail);
	bool GetIsTail();
};

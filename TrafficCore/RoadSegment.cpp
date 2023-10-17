#include "RoadSegment.h"

RoadSegment::RoadSegment() {
	carID = 0;
	isTail = false;
}

RoadSegment::~RoadSegment() {
}

void RoadSegment::SetCarID(long carID) {
	this->carID = carID;
}

long RoadSegment::GetCarID() {
	return carID;
}

void RoadSegment::SetIsTail(bool isTail) {
	this->isTail = isTail;
}

bool RoadSegment::GetIsTail() {
	return isTail;
}

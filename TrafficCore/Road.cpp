#include <iostream>

#include <time.h>
#include <fstream>
#include <sstream>
#include <windows.h>
#include <string>
#include <math.h>

#include "Road.h"
#include "DriverProfile.h"

using namespace std;

float DAcc(int currentSpeed, int accPlus, int dR, int nextCarSpeed);
float DKeep(int currentSpeed, int dR, int nextCarSpeed);
float DDec(int currentSpeed, int accMinus, int dR, int nextCarSpeed);

///
/// \brief The ChangeLaneConfig structure describes the cars that are checked during LAI model lane change formula
///
struct ChangeLaneConfig {
    ///
    /// \brief The CarStatus structure contains all the parameters that are necessary to the LAI model lange change formula
    ///
    struct CarStatus {
        ///
        /// \brief The parameter describing car's velocity
        ///
        int v;
        ///
        /// \brief The parameter describing car's acceleration
        ///
        int accPlus;
        ///
        /// \brief The parameter describing car's deceleration
        ///
        int accMinus;
        ///
        /// \brief The parameter describing car's distance from other vehicle
        ///
        int dR;
        ///
        /// \brief The parameter describing car's delta value
        ///
        int d;
        ///
        /// \brief The parameter describing number of segments the car consists of
        ///
        int segment;
        ///
        /// \brief The parameter describing car's maximum velocity
        ///
        int vMax;

        ///
        /// \brief Car object
        ///
        Car *car;
    }
    ///
    /// \brief Currently examined car
    ///
    curr,
    ///
    /// \brief Succeeding car on the same lane
    ///
    next,
    ///
    /// \brief Preceding car on the other lane
    ///
    pred,
    ///
    /// \brief Succeeding car on the other lane
    ///
    succ;
} ;

Road::Road(Configuration &config) {
    this->cfg = &config;

    if (cfg->GetSegmentsNum() < MIN_SEGMENTS || cfg->GetSegmentsNum() > MAX_SEGMENTS) {
        throw "Incorrect road segments number.";
    }

    if (cfg->GetCurrentDensity() <= 0 || cfg->GetCurrentDensity() > 100) {
        throw "Incorrect cars percentage.";
    }

    srand(time(NULL));

    this->segmentsNum = cfg->GetSegmentsNum();

    startSegments = new RoadSegment*[MAX_ROAD_LANES];
    cycleSegments = new RoadSegment*[MAX_ROAD_LANES];
    for (int i = 0; i < MAX_ROAD_LANES; i++) {
        startSegments[i] = new RoadSegment[this->segmentsNum];
        cycleSegments[i] = new RoadSegment[this->segmentsNum];
    }

    for (int l = 0; l < MAX_ROAD_LANES; l++) {
        carsNum[l] = 0;

        for (int s = 0; s < this->segmentsNum; s++) {
            startSegments[l][s].SetCarID(0);
            cycleSegments[l][s].SetCarID(0);

            startSegments[l][s].SetIsTail(false);
            cycleSegments[l][s].SetIsTail(false);
        }

        for (int c = 0; c < MAX_CARS_SET; c++) {
            vanishedCars[l][c].vanishedCars = 0;
        }
    }

    sumChangedLanes = 0;

    SetCarsSet();
    SetStartingState(cfg->GetCalculateDensities());
}

Road::~Road() {
    cars.clear();

    for (int l = 0; l < MAX_ROAD_LANES; l++) {
        delete[] startSegments[l];
        delete[] cycleSegments[l];
    }

    delete[] startSegments;
    startSegments = nullptr;

    delete[] cycleSegments;
    cycleSegments = nullptr;
}

void Road::SetCarsSet() {
    int minCarLength = 0;
    for (int i = 1; i < MAX_CARS_SET; i++) {
        if (cfg->GetCarProbability(i) > 0) {
            minCarLength = i;
            break;
        }
    }

    for (int l = 0; l < MAX_ROAD_LANES; l++) {
        int segmentsToSet = segmentsNum * cfg->GetCurrentDensity() / 100.0;

        while (segmentsToSet > 0) {
            int carLength = GenerateRandomStartingCar();

            if (segmentsToSet == carLength) {
                carsSet[l][carLength]++;
                carsSet[l][0]++;
                segmentsToSet -= carLength;
                break;
            } else if (segmentsToSet > carLength) {
                carsSet[l][carLength]++;
                carsSet[l][0]++;
                segmentsToSet -= carLength;
            } else {
                if (segmentsToSet == minCarLength) {
                    carsSet[l][minCarLength]++;
                    carsSet[l][0]++;
                    segmentsToSet -= minCarLength;
                    break;
                } else if (segmentsToSet > minCarLength) {
                    continue;
                } else {
                    break;
                }
            }
        }
    }
}

void Road::SetStartingState(bool densities) {
    for (int l = 0; l < MAX_ROAD_LANES; l++) {
        carsNum[l] = carsSet[l][0];

        int currentIdx = 0;
        while (carsSet[l][0] > 0) {
            int carLength = GenerateRandomStartingCar();

            if (carsSet[l][carLength] > 0) {
                int randomDriverType = GenerateRandomDriverTypeIndex();

                Car car(0, carLength);
                car.SetDriverType(randomDriverType);

                cars.insert(pair<long, Car>(car.GetCarID(), car));
                startSegments[l][currentIdx + car.GetTailSegmentsNum()].SetCarID(car.GetCarID());

                currentIdx += carLength;

                carsSet[l][carLength]--;
                carsSet[l][0]--;
            }
        }
    }
}

int Road::GenerateRandomStartingCar() {
    int maxProb = 1000;
    int carProbs[MAX_CARS_SET] = {0};

    int probCurrent = 0;
    for (int i = 1; i < MAX_CARS_SET; i++) {
        carProbs[i] = probCurrent + maxProb * cfg->GetCarProbability(i);
        probCurrent = carProbs[i];
    }

    int randNum = rand() % maxProb + 1;
    for (int i = 1; i < MAX_CARS_SET; i++) {
        if (randNum <= carProbs[i] && randNum > carProbs[i - 1]) {
            if (cfg->GetCarProbability(i) != 0) {
                return i;
            }
        }
    }

    return -1;
}

int Road::GenerateRandomDriverTypeIndex() {
    int maxProb = 1000;

    int randNum = rand() % maxProb + 1;
    for (int i = 0; i < cfg->vDriverProfiles.size(); i++) {
        if ( randNum <= cfg->vDriverProfiles[i].GetProbThreshold() ) {
            return i;
        }
    }

    return -1;
}

int allCars = 0;
void Road::GenerateCycles() {
    if (cfg->GetCyclesNum() < 1) {
        return;
    }

    string* cycleToShow = NULL;

    allCars = 0;
    for (int l = 0; l < MAX_ROAD_LANES; l++) {
        allCars += carsNum[l];
    }

    if (allCars == 0) {
        return;
    }

    for (int l = 0; l < MAX_ROAD_LANES; l++) {
        for (int s = 0; s < segmentsNum; s++) {
            cycleSegments[l][s].SetCarID(startSegments[l][s].GetCarID());
        }
    }

    SetTails();
    cycleToShow = PrepareToShow(0);

    for (int i = 0; i < cfg->GetShowFunctionsSize() && !cfg->GetCalculateDensities(); i++) {
        if (cfg->GetShowFunctionsElement(i) != NULL) {
            cfg->GetShowFunctionsElement(i)(*cfg, NULL, SHOW_INIT);

            if (cfg->GetCyclesToSkip() == 0) {
                cycleToShow = PrepareToShow(0);
                cfg->GetShowFunctionsElement(i)(*cfg, cycleToShow, SHOW_MID);
            }
        }
    }

    for (int c = 0; c < cfg->GetCyclesNum(); c++) {
        int carsAfterAll = 0;
        if (cfg->GetUseChangeLane()) {
            SetChangeLaneByFormula();
        }
        SetAccelerateByFormula();

        MoveCars();

        if (c >= cfg->GetCyclesToSkip()) {
            cycleToShow = PrepareToShow(c);

            for (int i = 0; i < cfg->GetShowFunctionsSize() && !cfg->GetCalculateDensities(); i++) {
                if (cfg->GetShowFunctionsElement(i) != NULL && cycleToShow != NULL) {
                    cfg->GetShowFunctionsElement(i)(*cfg, cycleToShow, SHOW_MID);
                }
            }
        }

        ClearStopChangingLane();
    }

    int carsAfterAll = CalculateCarsAfter(allCars);

    for (int i = 0; i < cfg->GetShowFunctionsSize() && !cfg->GetCalculateDensities(); i++) {
        if (cfg->GetShowFunctionsElement(i) != NULL) {
            cfg->GetShowFunctionsElement(i)(*cfg, NULL, SHOW_FINAL);
        }
    }
}

int Road::CalculateCarsAfter(int allCars) {
    int startingCarsNum = allCars;
    int carsAfter = 0;

    for (int l = 0; l < MAX_ROAD_LANES; l++) {
        for (int s = segmentsNum - 1; s >= 0; s--) {
            if (cycleSegments[l][s].GetCarID() != 0) {
                carsAfter++;
            }
        }
    }

    if (startingCarsNum != carsAfter) {
        int tmp0 = 1;
    }

    return carsAfter;
}

void Road::GenerateDensities(Configuration &config) {
    string flowsStr[MAX_ROAD_LANES];
    int *ptrFlows = NULL;

    float averageFlows[MAX_ROAD_LANES][MAX_CARS_SET];
    for (int i = 0; i < config.GetShowFunctionsSize(); i++) {
        if (config.GetShowFunctionsElement(i) != NULL) {
            config.GetShowFunctionsElement(i)(config, NULL, SHOW_INIT);
        }
    }

    for (int d = 0; d < config.GetDensitiesSize(); d++) {
        config.SetCurrentDensity(config.GetDensitiesElement(d));
        for (int l = 0; l < MAX_ROAD_LANES; l++) {
            flowsStr[l].clear();

            for (int c = 0; c < MAX_CARS_SET; c++) {
                averageFlows[l][c] = 0;
            }
        }

        int ptrFlows[MAX_ROAD_LANES][MAX_CARS_SET];
        float average[MAX_ROAD_LANES][MAX_CARS_SET];
        for (int t = 0; t < config.GetTestsNumber(); t++) {
            Road *road = new Road(config);
            road->GenerateCycles();

            memset(ptrFlows, 0, sizeof(ptrFlows));
            memset(average, 0, sizeof(average));
            for (int l_tmp = 0; l_tmp < MAX_ROAD_LANES; l_tmp++) {
                for (int c_tmp = 0; c_tmp < MAX_CARS_SET; c_tmp++) {
                    int ptrFlows_tmp = road->GetVanishedCars(l_tmp, c_tmp);
                    average[l_tmp][c_tmp] = ptrFlows_tmp;
                }
            }

            delete road;
        }

        for (int l = 0; l < MAX_ROAD_LANES; l++) {
            for (int c = 0; c < MAX_CARS_SET; c++) {
                flowsStr[l].append(to_string(average[l][c]));
                flowsStr[l].append(",");
            }

        }

        for (int i = 0; i < config.GetShowFunctionsSize(); i++) {
            if (config.GetShowFunctionsElement(i) != NULL) {
                config.GetShowFunctionsElement(i)(config, flowsStr, SHOW_MID);
            }
        }
    }

    for (int i = 0; i < config.GetShowFunctionsSize(); i++) {
        if (config.GetShowFunctionsElement(i) != NULL) {
            config.GetShowFunctionsElement(i)(config, NULL, SHOW_FINAL);
        }
    }
}

int Road::CheckFrontSameLane(int lane, int segment) {
    int idxNextCar = -1;
    for (int s = segment + 1; s < cfg->GetSegmentsNum(); s++) {
        if (cycleSegments[lane][s].GetCarID() != 0) {
            idxNextCar = s;
            break;
        }
    }

    if (idxNextCar == -1) {
        for (int s = 0; s < segment; s++) {
            if (cycleSegments[lane][s].GetCarID() != 0) {
                idxNextCar = s;
                break;
            }
        }
    }

    return idxNextCar;
}

int Road::CheckFrontAltLane(int lane, int segment) {
    if (carsNum[lane] == 0 || cycleSegments[lane][segment].GetCarID() != 0 || cycleSegments[lane][segment].GetIsTail()) {
        return -1;
    }

    int idxNextCar = -1;
    for (int s = segment + 1; s < cfg->GetSegmentsNum(); s++) {
        if (cycleSegments[lane][s].GetCarID() != 0) {
            idxNextCar = s;
            break;
        }
    }

    if (idxNextCar == -1) {
        for (int s = 0; s < segment; s++) {
            if (cycleSegments[lane][s].GetCarID() != 0) {
                idxNextCar = s;
                break;
            }
        }
    }

    return idxNextCar;
}

int Road::CheckBackAltLane(int lane, int segment) {
    if (carsNum[lane] == 0 || cycleSegments[lane][segment].GetCarID() != 0 || cycleSegments[lane][segment].GetIsTail()) {
        return -1;
    }

    int idxNextCar = -1;
    for (int s = segment - 1; s >= 0; s--) {
        if (cycleSegments[lane][s].GetCarID() != 0) {
            idxNextCar = s;
            break;
        }
    }

    if (idxNextCar == -1) {
        for (int s = cfg->GetSegmentsNum() - 1; s > segment; s--) {
            if (cycleSegments[lane][s].GetCarID() != 0) {
                idxNextCar = s;
                break;
            }
        }
    }

    return idxNextCar;
}

int Road::CalculateDistanceBetweenCars(int segment1, int segment2, int tail2) {
    int distance = -1;
    if (segment2 < segment1) {
        distance = (cfg->GetSegmentsNum() - 1) - segment1 + segment2 - tail2;
    } else {
        distance = (segment2 - segment1 - tail2) - 1;
    }

    return distance;
}

bool ChangeLaneLeftToRight( ChangeLaneConfig clc ) {
    if ( (clc.next.d >= DKeep(clc.curr.v, clc.curr.dR, clc.next.v) ) && (clc.pred.d >= DKeep(clc.curr.v, clc.curr.dR, clc.pred.v) ) ) {
        if ( clc.succ.d >= DDec(clc.succ.v, clc.succ.accMinus, clc.succ.dR, clc.curr.v) ) {
            return true;
        }
    }

    return false;
}

bool ChangeLaneRightToLeft( ChangeLaneConfig clc ) {
    bool ic1 = ( ( DKeep(clc.curr.v, clc.curr.dR, clc.next.v) <= clc.next.d ) && ( clc.next.d < DAcc(clc.curr.v, clc.curr.accPlus, clc.curr.dR, clc.next.v) ) ) &&
        ( ( clc.pred.d >= DAcc(clc.curr.v, clc.curr.accPlus, clc.curr.dR, clc.pred.v) ) && ( clc.curr.v < clc.curr.vMax ) );
    bool ic2 = ( clc.next.d < DKeep(clc.curr.v, clc.curr.dR, clc.next.v) ) && ( clc.pred.d >= DKeep(clc.curr.v, clc.curr.dR, clc.pred.v) );

    if ( ic1 ||	ic2 ) {
        if (clc.succ.d >= DDec(clc.succ.v, clc.succ.accMinus, clc.succ.dR, clc.curr.v)) {
            return true;
        }
    }

    return false;
}

void Road::SetChangeLaneByFormula() {
    if (MAX_ROAD_LANES < 2) {
        return;
    }

    for (int l = 0; l < MAX_ROAD_LANES; l++) {
        if (carsNum[l] < 2) {
            continue;
        }

        for (int s = segmentsNum - 1; s >= 0; s--) {
            if (cycleSegments[l][s].GetCarID() != 0) {
                ChangeLaneConfig clc = { 0 };

                clc.curr.car = &cars[cycleSegments[l][s].GetCarID()];
                clc.curr.v = clc.curr.car->GetCarSpeed();
                clc.curr.vMax = cfg->GetDriverProfile(clc.curr.car->GetDriverType())->GetMaxSpeed();
                clc.curr.accPlus = cfg->GetDriverProfile(clc.curr.car->GetDriverType())->GetAccPlus();
                clc.curr.accMinus = cfg->GetDriverProfile(clc.curr.car->GetDriverType())->GetAccMinus();
                clc.curr.dR = round(cfg->GetDriverProfile(clc.curr.car->GetDriverType())->GetDR() * clc.curr.v);

                if (clc.curr.car->GetStopChangingLane()) {
                    continue;
                }

                int altLane = (l == LEFT_LANE) ? RIGHT_LANE : LEFT_LANE;

                clc.next.segment = CheckFrontSameLane(l, s);
                clc.pred.segment = CheckFrontAltLane(altLane, s);
                clc.succ.segment = CheckBackAltLane(altLane, s);

                if (clc.next.segment != -1 && clc.pred.segment != -1 && clc.succ.segment != -1) {
                    clc.next.car = &cars[cycleSegments[l][clc.next.segment].GetCarID()];
                    clc.pred.car = &cars[cycleSegments[altLane][clc.pred.segment].GetCarID()];
                    clc.succ.car = &cars[cycleSegments[altLane][clc.succ.segment].GetCarID()];

                    clc.next.d = CalculateDistanceBetweenCars(s, clc.next.segment, clc.next.car->GetTailSegmentsNum());
                    clc.pred.d = CalculateDistanceBetweenCars(s, clc.pred.segment, clc.pred.car->GetTailSegmentsNum());
                    clc.succ.d = CalculateDistanceBetweenCars(clc.succ.segment, s, clc.curr.car->GetTailSegmentsNum());

                    if (clc.next.d < 0 && clc.pred.d < 0 && clc.succ.d < 0) {
                        continue;
                    }

                    clc.next.v = clc.next.car->GetCarSpeed();
                    clc.pred.v = clc.pred.car->GetCarSpeed();

                    clc.succ.v = clc.succ.car->GetCarSpeed();
                    clc.succ.accMinus = cfg->GetDriverProfile(clc.succ.car->GetDriverType())->GetAccMinus();
                    clc.succ.dR = round(cfg->GetDriverProfile(clc.succ.car->GetDriverType())->GetDR() * clc.succ.v);

                    bool changeLane = false;
                    if (l == LEFT_LANE) {
                        changeLane = ChangeLaneLeftToRight(clc);
                    } else {
                        changeLane = ChangeLaneRightToLeft(clc);
                    }

                    if (changeLane) {
                        MoveSingleCarChangeLane(l, altLane, s);
                    }
                }
            }
        }
    }
}

int DBreak(int speed) {
    int dBreakSum = 0;

    int loopEnd = speed / DELTA_V_M;
    for (int i = 1; i <= loopEnd; i++) {
        dBreakSum += ( speed - i * DELTA_V_M );
    }

    return dBreakSum;
}

int DF(int speed) {
    return speed + DBreak(speed);
}

float R_A(int speed) {
    return min(R_D, ( R_0 + speed * (R_D - R_0) / V_S) );
}

float DAcc(int currentSpeed, int accPlus, int dR, int nextCarSpeed) {
    return max(0, ( DF(currentSpeed + accPlus) + dR ) - DBreak(nextCarSpeed));
}

float DKeep(int currentSpeed, int dR, int nextCarSpeed) {
    return max(0, ( DF(currentSpeed) + dR ) - DBreak(nextCarSpeed));
}

float DDec(int currentSpeed, int accMinus, int dR, int nextCarSpeed) {
    return max(0, ( DF(currentSpeed - accMinus) + dR ) - DBreak(nextCarSpeed));
}

int CalculateNewSpeed(int distanceToNext, int currentSpeed, int maxSpeed, int nextCarSpeed, int dR, int accPlus, int accMinus) {
    float dAcc = DAcc(currentSpeed, accPlus, dR, nextCarSpeed);
    float dKeep = DKeep(currentSpeed, dR, nextCarSpeed);
    float dDec = DDec(currentSpeed, accMinus, dR, nextCarSpeed);

    float randomNum = ((double)rand() / (RAND_MAX));

    if (distanceToNext >= dAcc) {
        currentSpeed = min(currentSpeed + accPlus, maxSpeed);
    }
    if ((dAcc > distanceToNext && distanceToNext >= dKeep) || (currentSpeed == maxSpeed)) {
        currentSpeed = randomNum <= R_S ? max(currentSpeed - accMinus, 0) : currentSpeed;
    }
    if (dKeep > distanceToNext && distanceToNext >= dDec) {
        currentSpeed = max(currentSpeed - accMinus, 0);
    }
    if (distanceToNext < dDec) {
        currentSpeed = max(currentSpeed - DELTA_V_M, 0);
    }

    if (currentSpeed == -1) {
        throw "Incorrect speed";
    }

    return currentSpeed;
}

void Road::SetAccelerateByFormula() {
    for (int l = 0; l < MAX_ROAD_LANES; l++) {
        if (carsNum[l] == 0) {
            continue;
        }

        int carNextOldSpeed = 0;
        int carDistance = 0;
        for (int s = 0; s < segmentsNum - 1; s++) {
            if (cycleSegments[l][s].GetCarID() != 0) {
                Car& car = cars[cycleSegments[l][s].GetCarID()];
                carNextOldSpeed = car.GetCarSpeed();

                break;
            } else if (!cycleSegments[l][s].GetIsTail()) {
                carDistance++;
            }
        }

        for (int s = segmentsNum - 1; s >= 0; s--) {
            if (cycleSegments[l][s].GetCarID() != 0) {
                Car& car = cars[cycleSegments[l][s].GetCarID()];

                int newSpeed = CalculateNewSpeed(
                    carDistance,
                    car.GetCarSpeed(),
                    cfg->GetDriverProfile(car.GetDriverType())->GetMaxSpeed(),
                    carNextOldSpeed,
                    round(cfg->GetDriverProfile(car.GetDriverType())->GetDR() * car.GetCarSpeed()),
                    cfg->GetDriverProfile(car.GetDriverType())->GetAccPlus(),
                    cfg->GetDriverProfile(car.GetDriverType())->GetAccMinus()
                );

                car.SetCarSpeed(newSpeed);
                carNextOldSpeed = newSpeed;

                carDistance = 0;
            } else if (!cycleSegments[l][s].GetIsTail()) {
                carDistance++;
            }
        }
    }
}

void Road::SetTails() {
    int allCars = 0;
    for (int l = 0; l < MAX_ROAD_LANES; l++) {
        allCars += carsNum[l];
    }

    if (allCars == 0) {
        return;
    }

    int tail = 0;
    bool secondIteration = false;
    int numIteration = 0;

    for (int l = 0; l < MAX_ROAD_LANES; l++) {
        if (carsNum[l] == 0) {
            continue;
        }

        secondIteration = false;
        numIteration = 0;
        tail = 0;
        for (int s = segmentsNum - 1; true; s--) {
            if (s < 0) {
                secondIteration = true;
                numIteration++;
                s = segmentsNum - 1;
            }

            if ((secondIteration && cycleSegments[l][s].GetCarID() != 0) || numIteration > 1) {
                break;
            }

            cycleSegments[l][s].SetIsTail(false);

            if (cycleSegments[l][s].GetCarID() != 0) {
                Car& car = cars[cycleSegments[l][s].GetCarID()];
                tail = car.GetTailSegmentsNum();
            } else {
                if (tail > 0) {
                    cycleSegments[l][s].SetIsTail(true);
                    tail--;
                }
            }
        }
    }
}

void Road::MoveSingleCarChangeLane(int currentLane, int laneToChange, int segment) {
    Car &car = cars[cycleSegments[currentLane][segment].GetCarID()];
    cycleSegments[currentLane][segment].SetCarID(0);
    cycleSegments[laneToChange][segment].SetCarID(car.GetCarID());

    int tail = car.GetTailSegmentsNum();
    for (int s = segment - 1; tail > 0 && s >= 0; s--) {
        cycleSegments[currentLane][s].SetIsTail(false);
        cycleSegments[laneToChange][s].SetIsTail(true);

        tail--;
    }

    for (int s = cfg->GetSegmentsNum() - 1; tail > 0; s--) {
        cycleSegments[currentLane][s].SetIsTail(false);
        cycleSegments[laneToChange][s].SetIsTail(true);

        tail--;
    }

    car.SetStopChangingLane(true);

    carsNum[currentLane]--;
    carsNum[laneToChange]++;
    sumChangedLanes++;

    vanishedCars[currentLane][0].changeLane++;
    vanishedCars[currentLane][car.GetTailSegmentsNum() + 1].changeLane++;

    return;
}

void Road::ClearStopChangingLane() {
    map<long, Car>::iterator it;

    for (it = cars.begin(); it != cars.end(); it++) {
        it->second.SetStopChangingLane(false);
    }
}

void Road::MoveCars() {
    int nextPosition;
    int tail = 0;
    bool secondIteration = false;
    static int cnt = 0;

    vector<long> vanishedIds;
    vector<int> pos;

    for (int l = 0; l < MAX_ROAD_LANES; l++) {
        if (carsNum[l] == 0) {
            continue;
        }

        vanishedIds.clear();
        pos.clear();

        for (int s = segmentsNum - 1; s >= 0; s--) {
            if (cycleSegments[l][s].GetCarID() != 0) {
                Car &car = cars[cycleSegments[l][s].GetCarID()];
                nextPosition = s + car.GetCarSpeed();

                if (nextPosition > segmentsNum - 1) {
                    vanishedIds.push_back(car.GetCarID());
                    pos.push_back(nextPosition - segmentsNum);

                    cycleSegments[l][s].SetCarID(0);

                    vanishedCars[l][0].vanishedCars++;
                    vanishedCars[l][car.GetTailSegmentsNum() + 1].vanishedCars++;
                } else {
                    cycleSegments[l][s].SetCarID(0);
                    cycleSegments[l][nextPosition].SetCarID(car.GetCarID());
                }
            }
        }

        if (vanishedIds.size() > 0 && pos.size() > 0) {
            for (int i = 0; i < vanishedIds.size(); i++) {
                cycleSegments[l][pos.at(i)].SetCarID(vanishedIds.at(i));
            }
        }

        SetTails();

        cnt++;
    }
}

string *Road::PrepareToShow(int cyclesNum) {
    static string segments[MAX_ROAD_LANES + 1];
    for (int l = 0; l < MAX_ROAD_LANES; l++) {
        segments[l].clear();
    }

    bool tailChangedLane = false;
    char ws[16 + 1];

    for (int l = 0; l < MAX_ROAD_LANES; l++) {
        if (carsNum[l] == 0) {
            for (int s = 0; s < segmentsNum; s++) {
                segments[l].append(".");
            }
            continue;
        }

        tailChangedLane = false;
        for (int s = 0; s < segmentsNum; s++) {
            if (cycleSegments[l][s].GetCarID() != 0) {
                Car& car = cars[cycleSegments[l][s].GetCarID()];
                tailChangedLane = car.GetStopChangingLane();

                sprintf_s(ws, "%c", SPEEDS_TO_CHAR[car.GetCarSpeed()]);

                segments[l].append(ws);
            } else if (cycleSegments[l][s].GetIsTail()) {
                if (tailChangedLane) {
                    segments[l].append("}");
                } else {
                    segments[l].append(">");
                }
            } else {
                segments[l].append(".");
            }
        }
    }

    return segments;
}

int Road::GetVanishedCars(int lane, int carLength) {
    return vanishedCars[lane][carLength].vanishedCars;
}

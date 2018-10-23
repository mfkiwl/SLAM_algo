/*
 * StateModel.cpp
 *
 *  Created on: 22.10.2018
 *      Author: grauvogs
 */

#include <StateModel.h>
#include <unistd.h>
#include <math.h>

#include "ComStructure.h"
#include "SPILibrary.h"

// currently 10cm deviation
#define MAX_ALLOWED_DEVIATION_M	0.01f
#define TRAVEL_SPEED	600u

ComStructureType COM_StructRX = {0};
ComStructureType COM_StructTX = {0};

// #########################################################################################
// helper functions:
// #########################################################################################
float getDirectionDegree(PositionStructureType &position, PathTravel &travel)
{
	float degree = 0.0f; // 0 degree as default
	float temp = 0.0f;

	// get the angle towards the target
	temp = atanf( (travel.getTargetY() - position.y) / (travel.getTargetX() - position.x)) - position.theta;

	// convert from radians to degree
	degree = (180.0f / 3.14159265358979323846f) * temp;

	// check and apply bounds
	degree = fmaxf(degree, -90.0f);
	degree = fminf(degree, 90.0f);

	return degree;
}

// #########################################################################################
// class definition
// #########################################################################################

// PRIVATE:
bool StateModel::newPathAvailable(void) {
	if (NULL != ptrPathGroup) {
		return ptrPathGroup->getPathChanged();
	} else {
		return false;
	}
}


// PUBLIC:
StateModel::StateModel() {

}

void StateModel::Init(void)
{
	ptrPathGroup = new PathGroup();
	posUpdater = new PositionUpdater();
	COM_StructTX.CurrentSteeringMode = COM_STEERING_MODE_MANUAL;
	COM_StructTX.CurrentSteeringDirection = COM_STEERING_DIRECTON_ZERO;
}



void StateModel::calcNextState(void) {
	PathTravel *currentTarget = NULL;
	PositionStructureType position = {0};
	position = posUpdater->getPosition(); // get current position from module
	float degree = 0.0f;

	printf("Current Position: (position.x: %.2f, position.y: %.2f, theta: %.2f)\n", position.x, position.y, position.theta);

	switch (currentState) {
	case STATE_IDLE:
		// wait for next path list
		printf("Current State: IDLE\n");
		if (newPathAvailable()) {
			currentPathTravelIndex = 0;
			currentState = STATE_FETCH_PATHS;
		}
		break;
	case STATE_FETCH_PATHS:
		// calculate next direction
		printf("Current State: FETCH_PATHS\n");
		ptrPathGroup->determinePathTravels("./path.txt");

		if(ptrPathGroup->getNumAvailPathTravels() > 0) currentState = STATE_GET_NEXT_SEGMENT;
		else currentState = STATE_CLEAR_STATES;
		break;
	case STATE_GET_NEXT_SEGMENT:
		// get next segment on path
		printf("Current State: GET_NEXT_SEGMENT\n");

		if (ptrPathGroup->pathAtIndexAvailable(currentPathTravelIndex)) {
			currentTarget = ptrPathGroup->getPathTravelFromIndex(currentPathTravelIndex);
			degree = getDirectionDegree(position, (*currentTarget));
			COM_StructTX.CurrentSteeringAngle = degree;
			COM_StructTX.CurrentSteeringSpeed = TRAVEL_SPEED;

			spiSend(COM_StructTX, COM_StructRX);
			currentState = STATE_TRAVEL;
		} else {
			currentState = STATE_CLEAR_STATES;
		}


		break;
	case STATE_TRAVEL:

		printf("Current State: TRAVEL\n");

		// travel to next position and watch for reached destination
		// get next path to travel
		currentTarget = ptrPathGroup->getPathTravelFromIndex(currentPathTravelIndex);

		// always track orientation towards target:
		degree = getDirectionDegree(position, (*currentTarget));
		COM_StructTX.CurrentSteeringAngle = degree;

		if((MAX_ALLOWED_DEVIATION_M > abs(position.x - currentTarget->getTargetX()))
				&& (MAX_ALLOWED_DEVIATION_M > abs(position.y - currentTarget->getTargetY())))
		{
			// stop traveling if target position is reached
			currentState = STATE_GET_NEXT_SEGMENT;
			COM_StructTX.CurrentSteeringSpeed = 0u; // shutdown motor
			currentPathTravelIndex++; // get next path segment, if available
		}

		spiSend(COM_StructTX, COM_StructRX);
		break;
	case STATE_CLEAR_STATES:

		printf("Current State: CLEAR_STATES\n");

		ptrPathGroup->clearPathTravels();

		currentState = STATE_IDLE;

		// clear path travel states and prepare for next iteration
		break;
	}
}

void StateModel::Main(void)
{
	struct timespec ts_sleep = {0}, ts_remaining = {0};
	ts_sleep.tv_nsec = 100000000L; // 100 ms delay
	Init();

	while(1)
	{
		printf("Calculating Next State...\n");
		calcNextState();
		nanosleep(&ts_sleep, &ts_remaining);
	}
}

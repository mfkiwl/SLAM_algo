/*
 * StateModel.h
 *
 *  Created on: 22.10.2018
 *      Author: grauvogs
 */

#ifndef STATEMODEL_H
#define STATEMODEL_H

#include "PositionUpdater.h"
#include "ComStructure.h"
#include "Path.h"

#define PATH_FILE "../PathFinder_data/driveway.txt"
#define POSITION_FILE "../PathFinder_data/egoPosAtMap.txt"
#define MOTOR_STATE_FILE "../STM_data/motor.txt"
#define ULTRASONIC_FILE "../STM_data/ultrasonic.txt"
#define CONFIG_FILE "../STM_data/config.cfg"
#define CONTROL_STATE_FILE "../STM_data/controlStates.txt"

typedef enum StateEnum
{
	STATE_IDLE,
	STATE_FETCH_PATHS,
	STATE_TRAVEL,
	STATE_GET_NEXT_SEGMENT,
	STATE_CLEAR_STATES
} StateEnumType;

class StateModel
{
private:
	StateEnumType currentState = STATE_IDLE;
	// current index of traveled path group
	unsigned int currentPathTravelIndex = 0u;
    unsigned int pathIndexIncrement = 1u;
    uint16_t defaultMotorSpeed = COM_STEERING_SPEED_ZERO;
    uint8_t defaultMotorDirection = COM_STEERING_DIRECTON_ZERO;
    float maxAllowedDeviation = 0.1f;
	PathGroup *ptrPathGroup = NULL;
	PositionUpdater *posUpdater = NULL;

	bool newPathAvailable(void);
	void getConfig(void);
    void writeUltrasonicDistancesToFile(void);
    void writeControlStateToFile(void);
    void writeMotorStateToFile(void);
public:
	StateModel();
	void calcNextState(void);
	void Init(void);
	void Main(void);
};

#endif /* STATEMODEL_H */
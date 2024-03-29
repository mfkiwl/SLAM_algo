/*
 * Path.h
 *
 *  Created on: 22.10.2018
 *      Author: grauvogs
 */

#ifndef PATH_H
#define PATH_H

#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define MAX_NUM_PATH_TRAVELS	1000

class PathTravel {
private:
	float targetX, targetY, theta;
	bool travelled;
public:
	PathTravel(float tX, float tY, float th);
	float getTargetX(void);
	float getTargetY(void);
};

class PathGroup {
private:
	// init number of path travels
	PathTravel *travels[MAX_NUM_PATH_TRAVELS] = { 0x0 };
	// creation timestamp of path
	time_t creationTime = 0;
	bool pathChanged = true;

public:
	PathGroup();

	/** determine path travels from file input */
	void determinePathTravels(const char * inputFile);
	/** determine path travels from internal sources */
	void determinePathTravels(std::string &pathTravels);

	void clearPathTravels(void);
	bool getPathChanged(const char * inputFile);
	bool pathAtIndexAvailable(int index);
	int getNumAvailPathTravels(void);
	PathTravel * getPathTravelFromIndex(int index);

};

#endif /* PATH_H */

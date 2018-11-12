/*
 * Path.cpp
 *
 *  Created on: 22.10.2018
 *      Author: grauvogs
 */

#include <Path.h>

PathTravel::PathTravel(float tX, float tY, float th) {
	targetX = tX;
	targetY = tY;
	theta = th;
	travelled = false;
}

float PathTravel::getTargetX(void)
{
	return this->targetX;
}
float PathTravel::getTargetY(void)
{
	return this->targetY;
}

PathGroup::PathGroup() {
}

/** determine path travels from file input */
void PathGroup::determinePathTravels(const char * inputFile) {
	int counter = 0;
	FILE * file;
	char buffer[100] = { 0 };
	int scanfError = 0;
	float targetX, targetY, theta;

	file = fopen(inputFile, "r");
	while ((NULL != file) && (NULL != fgets(buffer, 99, file))
			&& (counter < MAX_NUM_PATH_TRAVELS)) {
		printf("%s", buffer);
		// parse informations from file
		scanfError = sscanf(buffer, "%f, %f, %f", &targetX, &targetY, &theta);

		this->travels[counter] = new PathTravel(targetX, targetY, theta);

		if (scanfError <= 0)
			break;

		counter++;
	}

	if (NULL != file) {
		fclose(file);
	}
}

/** [DUMMY]  determine path travels from internal sources */
void PathGroup::determinePathTravels(void) {

}

// clear all generated paths
void PathGroup::clearPathTravels(void) {
	for (int i = 0; i < MAX_NUM_PATH_TRAVELS; i++) {
		if (NULL != travels[i])
			delete travels[i];
		travels[i] = NULL;
	}
}

bool PathGroup::getPathChanged(const char * inputFile)
{
	struct stat statBuffer = {0};
	time_t newCreationTime = 0;

	stat(inputFile, &statBuffer);
	newCreationTime = statBuffer.st_mtime;

	if (newCreationTime > this-> creationTime)
	{
		this->creationTime = newCreationTime;
		return true;
	}
	else
	{
		return false;
	}

}

bool PathGroup::pathAtIndexAvailable(int index)
{
	if(NULL != travels[index]) return true;
	else return false;
}

PathTravel * PathGroup::getPathTravelFromIndex(int index)
{
	if(index < MAX_NUM_PATH_TRAVELS) return this->travels[index];
	else return NULL;
}

int PathGroup::getNumAvailPathTravels(void)
{
	for(int i = 0; i < MAX_NUM_PATH_TRAVELS; i++)
	{
		if(NULL == travels[i])
		{
			return i;
		}
	}

	return MAX_NUM_PATH_TRAVELS;
}
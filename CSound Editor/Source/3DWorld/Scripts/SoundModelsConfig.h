#pragma once

struct MovingPlaneConfig
{
	float travelSpeed;
	float maxDistanceReach;
	float pauseBetweenScans;
	float tickInterval;
	float tickVolume;
};

struct SweepingPlaneConfig
{
	float fullScanTime;
	float maxDistanceReach;
	float pauseBetweenScans;
	float tickInterval;
	float tickVolume;
};

struct ExpandingSphereConfig
{
	float travelSpeed;
	float maxDistanceReach;
	float pauseBetweenScans;
	unsigned int startScanFXVolume;
};


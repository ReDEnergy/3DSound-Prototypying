#pragma once

struct MovingPlaneConfig
{
	float travelSpeed;
	float maxDistanceReach;
	float pauseBetweenScans;
};

struct ExpandingSphereConfig
{
	float travelSpeed;
	float maxDistanceReach;
	float pauseBetweenScans;
	unsigned int startScanFXVolume;
};
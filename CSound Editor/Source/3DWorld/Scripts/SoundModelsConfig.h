#pragma once

struct MovingPlaneConfig
{
	float travelSpeed;
	float maxDistanceReach;
	float pauseBetweenScans;
	float tickInterval;
	float tickVolume;
	float soundGain;
};

struct SweepingPlaneConfig
{
	float fullScanTime;
	float maxDistanceReach;
	float pauseBetweenScans;
	float tickInterval;
	float tickVolume;
	float soundGain;
};

struct ExpandingSphereConfig
{
	float travelSpeed;
	float maxDistanceReach;
	float pauseBetweenScans;
	unsigned int startScanFXVolume;
};

struct DepthPerceptionConfig
{
	float soundGain;
};

struct ImpactSoundConfig
{
	float soundGain;
};

struct GlobalCsoundControlChannel {
	string label;
	string name;
	float value;
};
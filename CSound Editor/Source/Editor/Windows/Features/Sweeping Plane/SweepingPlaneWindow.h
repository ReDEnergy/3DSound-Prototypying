#pragma once
#include <Editor/Windows/Interface/CustomWidget.h>

// Engine
#include <Event/EventListener.h>


class CSoundComponentProperty;
class CSound3DSource;
class HeadphoneTestGenerator;
class HeadphoneTestAnswerPanel;
class SimpleFloatInput;
class QtSortableInput;
class SimpleCheckBox;
class QKeyEvent;

struct SweepingPlaneConfig;

class SweepingPlaneWindow
	: public CustomWidget
	, public EventListener
{
	public:
		SweepingPlaneWindow();
		virtual ~SweepingPlaneWindow() {};

	private:
		void InitUI();
		void Start();
		void Stop();
		void ResetConfig();
		void OnEvent(const string& eventID, void *data);

	private:
		SweepingPlaneConfig *config;
		SimpleFloatInput *scanTime;
		SimpleFloatInput *maxDistanceInput;
		SimpleFloatInput *pauseBetweenScans;
		SimpleFloatInput *tickInterval;
		SimpleFloatInput *tickVolume;
		SimpleFloatInput *soundGain;
};
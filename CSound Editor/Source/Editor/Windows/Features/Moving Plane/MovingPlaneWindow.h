#pragma once
#include <Editor/Windows/Interface/CustomWidget.h>

class CSoundComponentProperty;
class CSound3DSource;
class HeadphoneTestGenerator;
class HeadphoneTestAnswerPanel;
class SimpleFloatInput;
class QtSortableInput;
class SimpleCheckBox;
class QKeyEvent;

struct MovingPlaneConfig;

class MovingPlaneWindow
	: public CustomWidget
{
	public:
		MovingPlaneWindow();
		virtual ~MovingPlaneWindow() {};

	private:
		void InitUI();
		void Start();
		void ToggleState();
		void ResetConfig();

	private:
		QPushButton *startStopButton;
		MovingPlaneConfig *config;
		SimpleFloatInput *maxDistanceInput;
		SimpleFloatInput *travelSpeedInput;
		SimpleFloatInput *pauseBetweenScans;
		SimpleFloatInput *tickInterval;
		SimpleFloatInput *soundGain;
};
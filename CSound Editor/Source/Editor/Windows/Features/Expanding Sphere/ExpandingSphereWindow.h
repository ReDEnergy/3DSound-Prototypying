#pragma once
#include <Editor/Windows/Interface/CustomWidget.h>

class CSoundComponentProperty;
class CSound3DSource;
class HeadphoneTestGenerator;
class HeadphoneTestAnswerPanel;
class SimpleFloatInput;
class QtSortableInput;
class SimpleCheckBox;

struct ExpandingSphereConfig;

class ExpandingSphereWindow
	: public CustomWidget
{
	public:
		ExpandingSphereWindow();
		virtual ~ExpandingSphereWindow() {};

	private:
		void InitUI();
		void Start();
		void ToggleState();
		void ResetConfig();

	private:
		QPushButton* startStopButton;
		ExpandingSphereConfig *config;
		SimpleFloatInput *maxDistanceInput;
		SimpleFloatInput *travelSpeedInput;
		SimpleFloatInput *pauseBetweenScans;
		SimpleFloatInput *tickInterval;
		SimpleFloatInput *startTickVolume;
};
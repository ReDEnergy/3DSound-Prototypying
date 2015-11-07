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

struct MovingPlaneConfig;

class MovingPlaneWindow
	: public CustomWidget
	, public EventListener
{
	public:
		MovingPlaneWindow();
		virtual ~MovingPlaneWindow() {};

	private:
		void InitUI();
		void Start();
		void Stop();
		void ResetConfig();
		void OnEvent(const string& eventID, void *data);

	private:
		MovingPlaneConfig *config;
		SimpleFloatInput *maxDistanceInput;
		SimpleFloatInput *travelSpeedInput;
		SimpleFloatInput *pauseBetweenScans;
		SimpleFloatInput *tickInterval;
};
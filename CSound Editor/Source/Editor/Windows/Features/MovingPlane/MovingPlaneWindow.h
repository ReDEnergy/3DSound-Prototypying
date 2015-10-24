#pragma once
#include <Editor/Windows/Interface/CustomWidget.h>
#include <3DWorld/Scripts/MovingPlaneConfig.h>

// Engine
#include <Event/EventListener.h>


class CSoundComponentProperty;
class CSound3DSource;
class HrtfTestGenerator;
class HrtfTestAnswerPanel;
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
		SimpleFloatInput *planeMaxDistance;
		SimpleFloatInput *planeTravelSpeed;
		SimpleFloatInput *pauseBetweenScans;
		SimpleFloatInput *tickInterval;
};
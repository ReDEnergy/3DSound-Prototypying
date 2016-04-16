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

struct DepthPerceptionConfig;

class DepthPerceptionWindow
	: public CustomWidget
{
	public:
		DepthPerceptionWindow();
		virtual ~DepthPerceptionWindow() {};

	private:
		void InitUI();
		void Start();
		void ToggleState();
		void ResetConfig();

	private:
		QPushButton *startStopButton;
		DepthPerceptionConfig *config;
		SimpleFloatInput *soundGain;
};
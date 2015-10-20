#pragma once
#include <Editor/Windows/Interface/CustomWidget.h>

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

class HrtfTest
	: public CustomWidget
	, public EventListener
{
	public:
		HrtfTest();
		virtual ~HrtfTest() {};

	private:
		void InitUI();
		void Start();
		void Stop();
		void ResetConfig();
		void OnEvent(const string& eventID, void *data);
		void keyPressEvent(QKeyEvent * event);
		void keyReleaseEvent(QKeyEvent * event);

	private:

		HrtfTestGenerator *sampleGenerator;
		HrtfTestAnswerPanel *answerPanel;

		CustomWidget *configArea;
		CustomWidget *advanceConfig;
		QLineEdit *testName;
		SimpleFloatInput *prepareTime;
		SimpleFloatInput *sampleDuration;
		SimpleFloatInput *sampleInterval;
		SimpleFloatInput *randomIterations;
		SimpleFloatInput *numberOfSamples;
		QtSortableInput *sortableElevation;
		QtSortableInput *sortableAzimuth;
		SimpleCheckBox *wait4Input;
		SimpleCheckBox *randomValues;
		QPushButton **buttons;
};
#pragma once
#include <Editor/Windows/Interface/CustomWidget.h>

// Engine
#include <Event/EventListener.h>

#include <unordered_map>

struct HeadphoneTestConfig;

class CSoundComponentProperty;
class CSound3DSource;
class HeadphoneTestGenerator;
class HeadphoneTestAnswerPanel;
class SimpleFloatInput;
class QtSortableInput;
class SimpleCheckBox;

class QKeyEvent;


class HeadphoneTestWindow
	: public CustomWidget
	, public EventListener
{
	public:
		HeadphoneTestWindow();
		virtual ~HeadphoneTestWindow() {};

	private:
		void InitUI();
		void Start();
		void Stop();
		void SetConfig(HeadphoneTestConfig *config);
		void ResetConfig();
		void OnEvent(const string& eventID, void *data);
		void keyPressEvent(QKeyEvent * event);
		void KeyResponseInfo() const;
		void ResetAnswerKeyOffset();
		void SendKeyboardAnswer() const;

	private:

		QPushButton* buttonStartStop;

		HeadphoneTestConfig *config;
		vector<HeadphoneTestConfig *> customTests;

		HeadphoneTestGenerator *sampleGenerator;
		HeadphoneTestAnswerPanel *answerPanel;

		SimpleFloatInput *azimuthAnswer;
		SimpleFloatInput *elevationAnswer;

		CustomWidget *sectionLeft;
		CustomWidget *configArea;
		CustomWidget *generalConfigPanel;
		CustomWidget *advanceConfigPanel;
		QLineEdit *testName;
		QLineEdit *indivHrtfFile;
		SimpleFloatInput *prepareTime;
		SimpleFloatInput *sampleDuration;
		SimpleFloatInput *sampleInterval;
		SimpleFloatInput *randomIterations;
		SimpleFloatInput *numberOfSamples;
		QtSortableInput *sortableElevation;
		QtSortableInput *sortableAzimuth;

		SimpleCheckBox *showAdvancedConfig;
		SimpleCheckBox *wait4Input;
		SimpleCheckBox *randomValues;
		unordered_map<string, SimpleCheckBox*> testTypes;

		int keyOffsetX = 0;
		int keyOffsetY = 0;
		bool testStarted;
};
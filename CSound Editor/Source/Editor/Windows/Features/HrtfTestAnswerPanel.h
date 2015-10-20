#pragma once
#include <vector>
#include <functional>
#include <Editor/Windows/Interface/CustomWidget.h>

using namespace std;

class QLineEdit;

class HrtfTestAnswerPanel
	: public CustomWidget
{
	public:
		HrtfTestAnswerPanel();
		virtual ~HrtfTestAnswerPanel() {};

		void SetupAnswerPanel(vector<float> azimuthSamples, vector<float> elevationSamples);

	private:
		void InitUI();
		void ButtonClick(unsigned int index);

	private:
		vector<QPushButton*> buttons;
		CustomWidget *generatedPanel;
		QLabel *answerInfo;
};

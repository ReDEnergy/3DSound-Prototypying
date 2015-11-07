#pragma once
#include <vector>
#include <functional>
#include <Editor/Windows/Interface/CustomWidget.h>

using namespace std;

class QLineEdit;

using CallbackFunc = function<void(float azimuth, float elevation)>;

class HeadphoneTestAnswerPanel
	: public CustomWidget
{
	public:
		HeadphoneTestAnswerPanel();
		virtual ~HeadphoneTestAnswerPanel() {};

		void SetupAnswerPanel(vector<float> azimuthSamples, vector<float> elevationSamples);
		void OnButtonClick(CallbackFunc onClick);

	private:
		void InitUI();
		void ButtonClick(unsigned int index);

	private:
		vector<QPushButton*> buttons;
		CustomWidget *generatedPanel;
		QLabel *answerInfo;
		list<CallbackFunc> callbacks;
};

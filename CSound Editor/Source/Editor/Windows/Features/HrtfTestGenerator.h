#pragma once
#include <vector>
#include <functional>
#include <Editor/Windows/Interface/CustomWidget.h>

using namespace std;

class QLineEdit;
class SimpleFloatInput;

class HrtfTestGenerator
	: public CustomWidget
{
	public:
		HrtfTestGenerator();
		virtual ~HrtfTestGenerator() {};

		void SetupGenerator(vector<float> azimuthSamples, vector<float> elevationSamples);
		unsigned int GetNumberOfSamples() const;
		const vector<float>& GetAzimuthValues() const;
		const vector<float>& GetElevationValues() const;

	private:
		void InitUI();
		void Clear();
		void ButtonClick(unsigned int index);

	private:
		vector<QPushButton*> buttons;
		CustomWidget *generatedPanel;
		QLineEdit *testsSamples;
		vector <float> azimuthTestValues;
		vector <float> elevationTestValues;

		SimpleFloatInput *numberOfSamples;
};

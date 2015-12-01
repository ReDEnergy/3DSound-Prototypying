#pragma once
#include <Editor/Windows/Interface/CustomWidget.h>

class SimpleDropDown;
class CSound3DSource;
class QPushButton;

class CSoundOptionsWindow
	: public CustomWidget
{
	public:
		CSoundOptionsWindow();
		virtual ~CSoundOptionsWindow() {};

	private:
		void InitUI();

	private:
		CSound3DSource *testSource;
		SimpleDropDown *dropdownDevice;
		SimpleDropDown *dropdownChannels;
		QPushButton *testAudioButton;
};
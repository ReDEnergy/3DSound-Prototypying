#pragma once
#include <Editor/Windows/Interface/CustomWidget.h>

class SimpleDropDown;

class CSoundOptionsWindow
	: public CustomWidget
{
	public:
		CSoundOptionsWindow();
		virtual ~CSoundOptionsWindow() {};

	private:
		void InitUI();

	private:
		SimpleDropDown *dropdownDevice;
		SimpleDropDown *dropdownChannels;
};
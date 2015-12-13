#pragma once
#include <Editor/Windows/Interface/DockWindow.h>
#include <Editor/Windows/Interface/QtInput.h>
#include <Editor/QT/Utils.h>

#include <Event/EventListener.h>

#include <unordered_map>

class CSound3DSource;
class CustomWidget;

class CSoundControlWindow
	: public DockWindow
	, public EventListener
{
	public:
		CSoundControlWindow();
		virtual ~CSoundControlWindow() {};

	private:
		void InitUI();
		SimpleFloatInput* CreateControl(const char* name, const char *channel);
		void OnEvent(EventType Event, void *data);

	private:
		CustomWidget *controls;
		SimpleFloatInput *elevationPanningBias;
		SimpleFloatInput *rearChannelGain;

		QLineEdit *newViewName;
		QLineEdit *newChannelName;

		unordered_map<string, SimpleFloatInput*> controlChannels;
};
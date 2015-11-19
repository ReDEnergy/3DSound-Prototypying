#pragma once
#include <Editor/Windows/Interface/DockWindow.h>
#include <Editor/Windows/Interface/QtInput.h>
#include <Editor/QT/Utils.h>

#include <Event/EventListener.h>

class CSound3DSource;

class CSoundControlWindow
	: public DockWindow
	, public EventListener
{
	public:
		CSoundControlWindow();
		virtual ~CSoundControlWindow() {};

	private:
		void InitUI();
		void OnEvent(EventType Event, void *data);

	private:
		SimpleFloatInput *elevationPanningBias;
		SimpleFloatInput *rearChannelGain;
};
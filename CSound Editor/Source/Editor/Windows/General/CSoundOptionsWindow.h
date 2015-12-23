#pragma once
#include <Editor/Windows/Interface/CustomWidget.h>

class SimpleDropDown;
class CSound3DSource;
class AudioDevice;

class CSoundOptionsWindow
	: public CustomWidget
{
	public:
		CSoundOptionsWindow();
		virtual ~CSoundOptionsWindow() {};

	private:
		void InitUI();
		void SetSourceChannelForMapping(uint channelID);
		void MapSourceChannelTo(uint channelID);
		void UpdateChannelButton(uint channelID, uint channelTarget);
		void LoadDeviceSettings(uint deviceID);
		void UpdateChannelDropdown(uint deviceID);

		void UpdateScene();
		void UpdateTestScore();
		void SaveDevicesConfiguration();

		void TestChannel(uint channelID);
		void hideEvent(QHideEvent * event);

	private:
		vector<CustomWidget*> panels;

		CSound3DSource *testSource;
		SimpleDropDown *dropdownDevice;
		SimpleDropDown *dropdownChannels;
		QPushButton *saveButton;
		
		// Config Panel
		AudioDevice *selectedDevice;
		int sourceChannelForMapping;
		CustomWidget *configPanel;
		CustomWidget *configPanelChannelsBox;
		QLabel *configPanelTitle;
		int activeTestChannel;
		vector<QPushButton*> testButtons;
		vector<QPushButton*> testButtonsMap;
		vector<QPushButton*> mappedChannelsButtons;
		vector<CustomWidget*> channelBoxes;
};
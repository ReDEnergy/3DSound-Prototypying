#include "CSoundControlWindow.h"

#include <functional>

#include <Editor/GUI.h>
#include <Editor/QT/Utils.h>
#include <Editor/Windows/Editors/ComponentEditor.h>
#include <3DWorld/Csound/CSound3DSource.h>
#include <3DWorld/Csound/CSoundScene.h>

#include <Csound/CSoundComponentProperty.h>
#include <Csound/CSoundManager.h>
#include <Csound/SoundManager.h>
#include <CSoundEditor.h>

// Engine
#include <include/glm.h>
#include <include/glm_utils.h>
#include <include/utils.h>
#include <Core/GameObject.h>
#include <Core/Camera/Camera.h>
#include <Component/Transform/Transform.h>
#include <Component/Renderer.h>
#include <Component/Mesh.h>
#include <Manager/Manager.h>
#include <Manager/SceneManager.h>
#include <Manager/ResourceManager.h>
#include <UI/ColorPicking/ColorPicking.h>

// QT
#include <QComboBox>
#include <QLayout>
#include <QPushButton>
#include <QStyledItemDelegate>
#include <QGroupBox>

CSoundControlWindow::CSoundControlWindow()
{
	LoadStyleSheet("csound-control-window.css");
	setWindowTitle("Sound Control");
	InitUI();

	SubscribeToEvent(EventType::FRAME_AFTER_RENDERING);
}

void CSoundControlWindow::InitUI()
{
	qtLayout->setSpacing(5);
	qtLayout->setContentsMargins(20, 10, 20, 10);

	{
		auto zone = new QLabel("CSound Simulation Control");
		zone->setAlignment(Qt::AlignCenter);
		zone->setMargin(5);
		zone->setFont(QFont("Arial", 10, QFont::Bold));
		qtLayout->addWidget(zone);
	}

	controls = new CustomWidget();
	controls->setObjectName("ControlsPanel");

	{
		elevationPanningBias = CreateControl("Elevation Bias", "kElBias");
		elevationPanningBias->OnUserEdit([this](float value) {
			if (value > 1.0f) {
				elevationPanningBias->SetValue(1);
				return;
			}
			if (value < -1.0f) {
				elevationPanningBias->SetValue(-1);
				return;
			}
		});
	}

	{
		rearChannelGain = CreateControl("Rear Gain", "kRearGain");
		rearChannelGain->AcceptNegativeValues(false);
		rearChannelGain->SetValue(1);
		rearChannelGain->OnUserEdit([this](float value) {
			if (value > 10.0f) {
				rearChannelGain->SetValue(10);
			}
		});
	}

	{
		auto multi8El = CreateControl("Multi8El", "kMulti8El");
		multi8El->AcceptNegativeValues(false);
		multi8El->SetUnit("degrees");
		multi8El->SetValue(120);
	}

	{
		auto input = CreateControl("kDragFront", "kDragFront");
		input->AcceptNegativeValues(false);
		input->SetValue(1);
	}

	{
		auto input = CreateControl("8ChanPower", "k8ChanPower");
		input->AcceptNegativeValues(false);
		input->SetValue(1.25);
	}

	{
		auto input = CreateControl("FrontAmp", "kFrontAmp");
		input->AcceptNegativeValues(false);
		input->SetValue(1.5);
	}

	auto separator = new QWidget();
	separator->setObjectName("Separator");

	auto definePanel = new QVBoxLayout();
	definePanel->setObjectName("DefinePanel");
	{
		{
			auto widget = new CustomWidget(QBoxLayout::Direction::LeftToRight);
			auto viewLabel = new QLabel();
			viewLabel->setText("View Name");
			viewLabel->setProperty("DefineLabel", 0);
			newViewName = new QLineEdit();
			widget->AddWidget(viewLabel);
			widget->AddWidget(newViewName);
			definePanel->addWidget(widget);
		}

		{
			auto widget = new CustomWidget(QBoxLayout::Direction::LeftToRight);
			auto nameLabel = new QLabel();
			nameLabel->setText("Csound Channel");
			nameLabel->setProperty("DefineLabel", 0);
			newChannelName = new QLineEdit();
			widget->AddWidget(nameLabel);
			widget->AddWidget(newChannelName);
			definePanel->addWidget(widget);
		}

		{
			auto button = new QPushButton();
			button->setText("Add");
			definePanel->addWidget(button);
			QObject::connect(button, &QPushButton::clicked, this, [&]() {
				auto name = newViewName->text().toStdString();
				auto chn = newChannelName->text().toStdString();
				if (name.length() && chn.length()) {
					CreateControl(name.c_str(), chn.c_str());
				}
			});
		}
	}

	auto groupBox = new QGroupBox();
	groupBox->setTitle("Create new control");
	groupBox->setLayout(definePanel);


	qtLayout->addWidget(controls);
	qtLayout->addWidget(separator);
	qtLayout->addWidget(groupBox);
}

SimpleFloatInput* CSoundControlWindow::CreateControl(const char * name, const char * channel)
{
	if (controlChannels.find(channel) == controlChannels.end())
	{
		auto control = new SimpleFloatInput(name, "", 2);
		control->SetValue(0);
		controls->AddWidget(control);
		controlChannels[channel] = control;
		return control;
	}

	return nullptr;
}

void CSoundControlWindow::OnEvent(EventType Event, void * data)
{
	if (Event == EventType::FRAME_AFTER_RENDERING)
	{
		for (auto &CC : controlChannels) {
			CSoundEditor::GetScene()->SetCSoundControl(CC.first.c_str(), CC.second->GetValue());
		}
	}
}

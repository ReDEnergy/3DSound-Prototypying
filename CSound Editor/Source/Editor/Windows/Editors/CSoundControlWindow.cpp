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

CSoundControlWindow::CSoundControlWindow()
{
	setWindowTitle("Sound Control");
	InitUI();

	SubscribeToEvent(EventType::FRAME_AFTER_RENDERING);
}

void CSoundControlWindow::InitUI()
{
	qtLayout->setSpacing(5);
	qtLayout->setContentsMargins(20, 10, 20, 10);
	qtLayout->setAlignment(Qt::AlignTop);

	{
		auto zone = new QLabel("CSound Simulation Control");
		zone->setAlignment(Qt::AlignCenter);
		zone->setMargin(5);
		zone->setFont(QFont("Arial", 10, QFont::Bold));
		qtLayout->addWidget(zone);
	}

	{
		elevationPanningBias = new SimpleFloatInput("Elevation Bias:", "", 2);
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
		qtLayout->addWidget(elevationPanningBias);
	}

	{
		rearChannelGain = new SimpleFloatInput("Rear Gain:", "", 2);
		rearChannelGain->AcceptNegativeValues(false);
		rearChannelGain->SetValue(1);
		rearChannelGain->OnUserEdit([this](float value) {
			if (value > 10.0f) {
				rearChannelGain->SetValue(10);
			}
		});
		qtLayout->addWidget(rearChannelGain);
	}
}

void CSoundControlWindow::OnEvent(EventType Event, void * data)
{
	if (Event == EventType::FRAME_AFTER_RENDERING) {
		CSoundEditor::GetScene()->SetCSoundControl("kRearGain", rearChannelGain->GetValue());
		CSoundEditor::GetScene()->SetCSoundControl("kElBias", elevationPanningBias->GetValue());
	}
}

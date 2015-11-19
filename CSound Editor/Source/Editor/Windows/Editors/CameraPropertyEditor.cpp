#include "CameraPropertyEditor.h"

#include <functional>

#include <Editor/GUI.h>
#include <Editor/QT/Utils.h>
#include <Editor/Windows/Editors/ComponentEditor.h>
#include <3DWorld/Csound/CSound3DSource.h>

#include <Csound/CSoundComponentProperty.h>
#include <Csound/CSoundManager.h>
#include <Csound/SoundManager.h>

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
#include <QLayout>

CameraPropertyEditor::CameraPropertyEditor()
{
	setWindowTitle("Camera Properties");
	InitUI();
	SubscribeToEvent(EventType::FRAME_AFTER_RENDERING);
}

void CameraPropertyEditor::InitUI()
{
	qtLayout->setSpacing(5);
	qtLayout->setContentsMargins(20, 10, 20, 10);
	qtLayout->setAlignment(Qt::AlignTop);

	{
		auto zone = new QLabel("Camera properties");
		zone->setAlignment(Qt::AlignCenter);
		zone->setMargin(5);
		zone->setFont(QFont("Arial", 10, QFont::Bold));
		qtLayout->addWidget(zone);
	}

	{
		cameraPosition = new GLMVecComponent<glm::vec3>("Position:", glm::vec3());
		cameraPosition->OnUserEdit([](glm::vec3 val) {
			Manager::GetScene()->GetActiveCamera()->transform->SetWorldPosition(val);
		});
		qtLayout->addWidget(cameraPosition);

		cameraRotation = new GLMVecComponent<glm::vec3>("Rotation:", glm::vec3());
		cameraRotation->OnUserEdit([](glm::vec3 val) {
			Manager::GetScene()->GetActiveCamera()->transform->SetWorldRotation(val);
		});
		qtLayout->addWidget(cameraRotation);

		camerFoV = new SimpleFloatInput("Camera FoV", "degrees", 0);
		camerFoV->AcceptNegativeValues(false);
		camerFoV->OnUserEdit([](float value) {
			auto cam = Manager::GetScene()->GetActiveCamera();
			auto PI = cam->GetProjectionInfo();
			PI.FoV = max(10, value) / PI.aspectRatio;
			cam->SetProjection(PI);
		});
		auto PI = Manager::GetScene()->GetActiveCamera()->GetProjectionInfo();
		camerFoV->SetValue(PI.FoV * PI.aspectRatio);
		qtLayout->addWidget(camerFoV);

		moveSpeed = new SimpleFloatInput("Move spped", "meters/second", 2);
		moveSpeed->AcceptNegativeValues(false);
		moveSpeed->OnUserEdit([](float value) {
			Manager::GetScene()->GetActiveCamera()->transform->SetMoveSpeed(value);
		});
		auto cam = Manager::GetScene()->GetActiveCamera();
		moveSpeed->SetValue(cam->transform->GetMoveSpeed());
		qtLayout->addWidget(moveSpeed);
	}
}

void CameraPropertyEditor::Update()
{
	auto cameraTransform = Manager::GetScene()->GetActiveCamera()->transform;
	bool cameraMotion = cameraTransform->GetMotionState();

	if (cameraMotion) {
		cameraPosition->SetValue(cameraTransform->GetWorldPosition());
		cameraRotation->SetValue(cameraTransform->GetRotationEuler360());
	}
}

void CameraPropertyEditor::OnEvent(EventType Event, void * data)
{
	if (Event == EventType::FRAME_AFTER_RENDERING) {
		Update();
	}
}

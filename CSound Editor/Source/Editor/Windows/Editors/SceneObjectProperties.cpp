#include "SceneObjectProperties.h"

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
#include <QComboBox>
#include <QLayout>
#include <QPushButton>
#include <QStyledItemDelegate>

static CSound3DSource* defaultObject;

SceneObjectProperties::SceneObjectProperties()
{
	setWindowTitle("Object Properties");
	forceUpdate = false;
	defaultObject = new CSound3DSource();
	gameObj = defaultObject;

	InitUI();

	SubscribeToEvent(EventType::EDITOR_OBJECT_SELECTED);
	SubscribeToEvent(EventType::EDITOR_NO_SELECTION);
}

void SceneObjectProperties::InitUI()
{
	qtLayout->setSpacing(5);
	qtLayout->setContentsMargins(20, 10, 20, 10);
	qtLayout->setAlignment(Qt::AlignTop);

	{
		auto label = new QLabel();
		label->setText("Object Type");
		label->setContentsMargins(0, 0, 0, 0);
		label->setFixedWidth(65);
		label->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);

		meshType = new QComboBox();
		
		QStyledItemDelegate* itemDelegate = new QStyledItemDelegate();
		meshType->setItemDelegate(itemDelegate);
		meshType->addItem("Box", QVariant("box"));
		meshType->addItem("Sphere", QVariant("sphere"));
		meshType->addItem("Plane", QVariant("plane"));

		void (QComboBox::* indexChangedSignal)(int index) = &QComboBox::currentIndexChanged;
		QObject::connect(meshType, indexChangedSignal, this, [&](int index) {
			auto value = meshType->itemData(index);
			auto type = value.toString().toStdString();
			auto mesh = Manager::GetResource()->GetMesh(type.c_str());
			// If plane disable backface culling
			auto culling = index == 2 ? OpenGL::CULL::NONE : OpenGL::CULL::BACK;
			gameObj->renderer->SetCulling(culling);
			gameObj->SetMesh(mesh);
		});

		auto widget = Wrap(label, meshType);
		widget->layout()->setSpacing(15);
		qtLayout->addWidget(widget);
	}

	{
		auto zone = new QLabel("Absolute properties");
		zone->setAlignment(Qt::AlignCenter);
		zone->setMargin(5);
		zone->setFont(QFont("Arial", 10, QFont::Bold));
		qtLayout->addWidget(zone);
	}

	{
		worldPosition = new GLMVecComponent<glm::vec3>("Position:", glm::vec3());
		worldPosition->OnUserEdit([this](glm::vec3 val) {
			gameObj->transform->SetWorldPosition(val);
		});
		qtLayout->addWidget(worldPosition);

		worldEuler = new GLMVecComponent<glm::vec3>("Rotation:", glm::vec3());
		worldEuler->OnUserEdit([this](glm::vec3 val) {
			gameObj->transform->SetWorldRotation(val);
		});
		qtLayout->addWidget(worldEuler);

		worldScale = new GLMVecComponent<glm::vec3>("Scale:", glm::vec3());
		worldScale->OnUserEdit([this](glm::vec3 val) {
			gameObj->transform->SetScale(val);
		});
		qtLayout->addWidget(worldScale);

		worldQuat = new GLMVecComponent<glm::quat>("Quat:", glm::quat());
		worldQuat->OnUserEdit([this](glm::quat val) {
			gameObj->transform->SetWorldRotation(val);
		});
		qtLayout->addWidget(worldQuat);
	}

	{
		auto zone = new QLabel("Relative to camera");
		zone->setAlignment(Qt::AlignCenter);
		zone->setMargin(5);
		zone->setFont(QFont("Arial", 10, QFont::Bold));
		qtLayout->addWidget(zone);
	}

	{
		cameraSpacePosition = new GLMVecComponent<glm::vec3>("Position:", glm::vec3());
		qtLayout->addWidget(cameraSpacePosition);
	}

	{
		soundVolume = new SimpleFloatInput("Volume:", "", 2, true);
		qtLayout->addWidget(soundVolume);

		soundIntensity = new SimpleFloatInput("Intensity:", "", 2, true);
		qtLayout->addWidget(soundIntensity);

		azimuthInput = new SimpleFloatInput("Azimuth:", "deg", 2, true);
		qtLayout->addWidget(azimuthInput);

		elevationInput = new SimpleFloatInput("Elevation:", "deg", 2, true);
		qtLayout->addWidget(elevationInput);

		azimuthPanningFactor = new SimpleFloatInput("Azimuth pan:", "", 2, true);
		qtLayout->addWidget(azimuthPanningFactor);

		elevationPanningFactor = new SimpleFloatInput("Elevation pan:", "", 2, true);
		qtLayout->addWidget(elevationPanningFactor);

		distanceToCameraInput = new SimpleFloatInput("Distance to:", "meters", 2, true);
		qtLayout->addWidget(distanceToCameraInput);

		surfaceAreaInput = new SimpleFloatInput("Surface area:", "pixels", 0, true);
		qtLayout->addWidget(surfaceAreaInput);

		surfaceCoverInput = new SimpleFloatInput("Surface cover:", "", 3, true);
		qtLayout->addWidget(surfaceCoverInput);
	}
}

void SceneObjectProperties::Update()
{
	bool selectedMotion = gameObj->transform->GetMotionState();
	auto cameraTransform = Manager::GetScene()->GetActiveCamera()->transform;
	bool cameraMotion = cameraTransform->GetMotionState();

	//if (selectedMotion || cameraMotion) {
		gameObj->ComputeControlProperties();
		cameraSpacePosition->SetValue(gameObj->GetCameraSpacePosition());
		surfaceAreaInput->SetValue(gameObj->GetSurfaceArea());
		surfaceCoverInput->SetValue(gameObj->GetSurfaceCover());
		distanceToCameraInput->SetValue(gameObj->GetDistanceToCamera());
		soundVolume->SetValue(gameObj->GetSoundVolume());
		azimuthInput->SetValue(gameObj->GetAzimuthToCamera());
		elevationInput->SetValue(gameObj->GetElevationToCamera());
		soundIntensity->SetValue(gameObj->GetSoundIntensity());
		azimuthPanningFactor->SetValue(gameObj->GetAzimuthPanningFactor());
		elevationPanningFactor->SetValue(gameObj->GetElevationPanningFactor());
	//}

	if (forceUpdate || selectedMotion) {
		gameObj->transform->GetWorldPosition();
		worldPosition->SetValue(gameObj->transform->GetWorldPosition());
		worldQuat->SetValue(gameObj->transform->GetWorldRotation());
		worldScale->SetValue(gameObj->transform->GetScale());
		worldEuler->SetValue(gameObj->transform->GetRotationEuler360());
	}
}

void SceneObjectProperties::ForceUpdate()
{
	if (forceUpdate == false)
	{
		forceUpdate = true;
		Update();

		auto meshID = gameObj->mesh->GetMeshID();
		auto typeIndex = meshType->findData(QVariant(meshID));
		if (typeIndex >= 0)
			meshType->setCurrentIndex(typeIndex);

		forceUpdate = false;
	}
}

void SceneObjectProperties::OnEvent(EventType Event, void * data)
{
	if (Event == EventType::EDITOR_OBJECT_SELECTED) {
		gameObj = dynamic_cast<CSound3DSource*>((GameObject*)data);
		if (!gameObj)
			gameObj = defaultObject;
		ForceUpdate();
	}
	if (Event == EventType::EDITOR_NO_SELECTION) {
		gameObj = defaultObject;
		ForceUpdate();
	}
}

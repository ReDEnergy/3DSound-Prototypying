#include "MovingPlane.h"

#include <3DWorld/Csound/CSound3DSource.h>
#include <3DWorld/Csound/CSoundScene.h>

#include <Csound/CSoundComponentProperty.h>
#include <Csound/CSoundManager.h>
#include <Csound/SoundManager.h>
#include <Csound/CSoundScore.h>
#include <Csound/CSoundInstrument.h>

#include <CSoundEditor.h>

// Engine
#include <include/glm.h>
#include <include/glm_utils.h>
#include <Core/Engine.h>
#include <Core/GameObject.h>
#include <Core/Camera/Camera.h>
#include <Component/Transform/Transform.h>
#include <Component/Renderer.h>
#include <Manager/Manager.h>
#include <Manager/SceneManager.h>
#include <Manager/ResourceManager.h>
#include <Manager/EventSystem.h>
#include <UI/ColorPicking/ColorPicking.h>

static TimerManager<string> *dynamicEvents;

MovingPlane::MovingPlane()
{
	planeSpeed = 5.0f;

	virtualPlane = Manager::GetResource()->GetGameObject("plane");
	visiblePlane = Manager::GetResource()->GetGameObject("plane");

	camera = Manager::GetScene()->GetActiveCamera();

	visiblePlane->SetSelectable(false);
	visiblePlane->renderer->SetOpacity(0.2f);
	visiblePlane->renderer->SetCulling(OpenGL::CULL::NONE);
	visiblePlane->transform->SetScale(glm::vec3(10));

	SubscribeToEvent("start-moving-plane");
	SubscribeToEvent("stop-moving-plane");
}

void MovingPlane::Init()
{
	camera->AddChild(virtualPlane);
	virtualPlane->transform->SetWorldRotation(camera->transform->GetWorldRotation());

	visiblePlane->transform->SetWorldRotation(camera->transform->GetWorldRotation());
	Manager::GetScene()->AddObject(visiblePlane);
}

void MovingPlane::Start()
{
	Init();
	Reset();
	CSoundEditor::GetScene()->Play();
	SubscribeToEvent(EventType::FRAME_UPDATE);
}

void MovingPlane::Stop()
{
	camera->RemoveChild(virtualPlane);
	Manager::GetScene()->RemoveObject(visiblePlane);
	CSoundEditor::GetScene()->Stop();
	UnsubscribeFrom(EventType::FRAME_UPDATE);
}

void MovingPlane::ClearEvents()
{
}

void MovingPlane::Update()
{
	planeSpeed += 0.25f;
	auto position = virtualPlane->transform->GetLocalPosition() + glm::vec3(0, 0, 1) * Engine::GetLastFrameTime() * planeSpeed;
	virtualPlane->transform->SetLocalPosition(position);
	visiblePlane->transform->SetWorldPosition(virtualPlane->transform->GetWorldPosition());
	visiblePlane->transform->SetWorldRotation(virtualPlane->transform->GetWorldRotation());

	// Intersect object from the scene
	auto entries = CSoundEditor::GetScene()->GetEntries();

	for (auto &source : entries) {
		if (source->GetSurfaceCover()) {
			glm::vec3 pos = glm::rotate(glm::inverse(camera->transform->GetWorldRotation()), source->transform->GetWorldPosition() - camera->transform->GetWorldPosition());
			float dist = min(abs(pos.z - position.z), 2.0f);
			unsigned int volume = unsigned int((2 - dist) * 50);
			source->SetVolume(volume);
		}
		else {
			source->SetVolume(0);
		}
	}

	if (position.z > 15) {
		Reset();
	}
}

void MovingPlane::Reset()
{
	virtualPlane->transform->SetLocalPosition(glm::vec3(0, 0, 0.5f));
	planeSpeed = 5;
}

void MovingPlane::OnEvent(EventType eventID, void * data)
{
	if (eventID == EventType::FRAME_UPDATE)
		Update();
}

void MovingPlane::OnEvent(const string & eventID, void * data)
{
	if (eventID.compare("start-moving-plane") == 0) {
		Start();
	}
	if (eventID.compare("stop-moving-plane") == 0) {
		Stop();
	}
}
#include "MovingPlaneScript.h"

#include <3DWorld/Csound/CSound3DSource.h>
#include <3DWorld/Csound/CSoundScene.h>

#include <3DWorld/Compute/SceneIntersection.h>
#include <3DWorld/Scripts/MovingPlaneConfig.h>

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

MovingPlaneScript::MovingPlaneScript()
{
	virtualPlane = Manager::GetResource()->GetGameObject("plane");
	visiblePlane = new GameObject(*virtualPlane);

	camera = Manager::GetScene()->GetActiveCamera();

	visiblePlane->SetSelectable(false);
	visiblePlane->renderer->SetOpacity(0.2f);
	visiblePlane->renderer->SetCulling(OpenGL::CULL::NONE);
	visiblePlane->transform->SetScale(glm::vec3(10));

	dynamicEvents = Manager::GetEvent()->GetDynamicTimers();
	scanPauseEvent = dynamicEvents->Create("Resume-Moving-Plane", 1);

	computeIntersection = new SceneIntersection();

	SubscribeToEvent("Start-Moving-Plane");
	SubscribeToEvent("Stop-Moving-Plane");
	SubscribeToEvent("Resume-Moving-Plane");
}

void MovingPlaneScript::Init()
{
	camera->AddChild(virtualPlane);
	virtualPlane->transform->SetWorldRotation(camera->transform->GetWorldRotation());

	visiblePlane->transform->SetWorldRotation(camera->transform->GetWorldRotation());
	Manager::GetScene()->AddObject(visiblePlane);
}

void MovingPlaneScript::Start()
{
	Init();
	Reset();

	scanPauseEvent->SetNewInterval(config->pauseBetweenScans);
	computeIntersection->Start();
	CSoundEditor::GetScene()->Play();
	SubscribeToEvent(EventType::FRAME_UPDATE);
}

void MovingPlaneScript::Stop()
{
	computeIntersection->Stop();
	camera->RemoveChild(virtualPlane);
	Manager::GetScene()->RemoveObject(visiblePlane);
	CSoundEditor::GetScene()->Stop();
	UnsubscribeFrom(EventType::FRAME_UPDATE);
	dynamicEvents->Remove(scanPauseEvent);
}

void MovingPlaneScript::ClearEvents()
{
}

void MovingPlaneScript::Update()
{
	// View offset represents the distance between the plane and the computational model
	// The computation is based on the camera view space distance provided by the rendering pipeline
	// Since the visible plane/sphere affects the view space (like a wall in the rendering target) 
	// we need to make the computation using a slighly less value from the visible one in order to
	// guarantee that the plane/spehere will not have an effect on the computation model

	float viewOffset = 0.1f;

	// In view/camera space forward is oriented towards the -Z axis
	auto forward = -glm::vec3(0, 0, 1);

	auto position = virtualPlane->transform->GetLocalPosition() + forward * Engine::GetLastFrameTime() * config->travelSpeed;
	virtualPlane->transform->SetLocalPosition(position);
	visiblePlane->transform->SetWorldPosition(virtualPlane->transform->GetWorldPosition());
	visiblePlane->transform->SetWorldRotation(virtualPlane->transform->GetWorldRotation());

	// -pozition.z is actually a positive number since camera space is in -Z area
	computeIntersection->SetSphereSize(-position.z - viewOffset);

	// Reset position when
	if ((-position.z + viewOffset) > config->maxDistanceReach)
	{
		computeIntersection->SetSphereSize(0.05f);
		Reset();
	}
}

void MovingPlaneScript::Reset()
{
	if (config->pauseBetweenScans)
	{
		UnsubscribeFrom(EventType::FRAME_UPDATE);
		Manager::GetScene()->RemoveObject(visiblePlane);
		dynamicEvents->Add(scanPauseEvent);

		visiblePlane->transform->SetWorldPosition(virtualPlane->transform->GetWorldPosition());
		visiblePlane->transform->SetWorldRotation(virtualPlane->transform->GetWorldRotation());
	}
	virtualPlane->transform->SetLocalPosition(glm::vec3(0, 0, 0.5f));
}

void MovingPlaneScript::OnEvent(EventType eventID, void * data)
{
	if (eventID == EventType::FRAME_UPDATE)
		Update();
}

void MovingPlaneScript::OnEvent(const string & eventID, void * data)
{
	if (eventID.compare("Start-Moving-Plane") == 0) {
		config = (MovingPlaneConfig*)data;
		Start();
	}
	if (eventID.compare("Resume-Moving-Plane") == 0) {
		SubscribeToEvent(EventType::FRAME_UPDATE);
		Manager::GetScene()->AddObject(visiblePlane);
		dynamicEvents->Remove(scanPauseEvent);
	}
	if (eventID.compare("Stop-Moving-Plane") == 0) {
		Stop();
	}
}
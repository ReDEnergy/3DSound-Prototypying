#include "GrowingSphere.h"

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
#include <Component/Mesh.h>
#include <Manager/Manager.h>
#include <Manager/SceneManager.h>
#include <Manager/ResourceManager.h>
#include <Manager/EventSystem.h>
#include <UI/ColorPicking/ColorPicking.h>

static TimerManager<string> *dynamicEvents;

GrowingSphere::GrowingSphere()
{
	camera = Manager::GetScene()->GetActiveCamera();

	expandingSpeed = 5.0f;
	visibleSphere = Manager::GetResource()->GetGameObject("sphere");
	visibleSphere->SetSelectable(false);
	visibleSphere->renderer->SetOpacity(0.2f);
	visibleSphere->renderer->SetCulling(OpenGL::CULL::NONE);

	SubscribeToEvent("start-expanding-sphere");
	SubscribeToEvent("stop-expanding-sphere");
}

void GrowingSphere::Init()
{
	Manager::GetScene()->AddObject(visibleSphere);
}

void GrowingSphere::Start()
{
	Init();
	Reset();
	CSoundEditor::GetScene()->Play();
	SubscribeToEvent(EventType::FRAME_UPDATE);
	visibleSphere->transform->SetWorldPosition(camera->transform->GetWorldPosition());
}

void GrowingSphere::Stop()
{
	Manager::GetScene()->RemoveObject(visibleSphere);
	CSoundEditor::GetScene()->Stop();
	UnsubscribeFrom(EventType::FRAME_UPDATE);
}

void GrowingSphere::ClearEvents()
{
}

void GrowingSphere::Update()
{
	expandingSpeed += 0.25f;
	auto scale = visibleSphere->transform->GetScale() + glm::vec3(1.0f) * Engine::GetLastFrameTime() * expandingSpeed;
	visibleSphere->transform->SetScale(scale);
	if (camera->transform->GetMotionState())
		visibleSphere->transform->SetWorldPosition(camera->transform->GetWorldPosition());

	// Intersect object from the scene
	auto entries = CSoundEditor::GetScene()->GetEntries();

	for (auto &source : entries) {
		if (source->GetSurfaceCover())
		{
			float dist = min(abs(scale.x/2 - source->DistTo(camera)), 2.0f);
			float volume = (2 - dist) * 50;
			source->SetVolume((unsigned int)volume);
		}
		else {
			source->SetVolume(0);
		}
	}

	if (scale.x/2 > 10) {
		Reset();
	}
}

void GrowingSphere::Reset()
{
	visibleSphere->transform->SetScale(glm::vec3(1.0f));
	expandingSpeed = 5.0f;
}

void GrowingSphere::OnEvent(EventType eventID, void * data)
{
	if (eventID == EventType::FRAME_UPDATE)
		Update();
}

void GrowingSphere::OnEvent(const string & eventID, void * data)
{
	if (eventID.compare("start-expanding-sphere") == 0) {
		Start();
	}
	if (eventID.compare("stop-expanding-sphere") == 0) {
		Stop();
	}
}
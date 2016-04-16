#include <pch.h>
#include "ExpandingSphereScript.h"

#include <3DWorld/Csound/CSound3DSource.h>
#include <3DWorld/Csound/CSoundScene.h>

#include <3DWorld/Compute/SceneIntersection.h>
#include <3DWorld/Scripts/SoundModelsConfig.h>

#include <Csound/CSoundComponentProperty.h>
#include <Csound/CSoundManager.h>
#include <Csound/SoundManager.h>
#include <Csound/CSoundSynthesizer.h>
#include <Csound/CSoundInstrument.h>

#include <CSoundEditor.h>

static TimerManager<string> *dynamicEvents;

ExpandingSphereScript::ExpandingSphereScript()
{
	camera = Manager::GetScene()->GetActiveCamera();

	visibleSphere = Manager::GetResource()->GetGameObject("sphere");
	visibleSphere->SetSelectable(false);
	visibleSphere->renderer->SetOpacity(0.2f);
	visibleSphere->renderer->SetCulling(OpenGL::CULL::NONE);

	dynamicEvents = Manager::GetEvent()->GetDynamicTimers();
	scanPauseEvent = dynamicEvents->Create("Resume-Expanding-Sphere", 1);

	computeIntersection = new SceneIntersection();

	computeIntersection->OnUpdate([](const vector<CSound3DSource*>& objects) {
		// ------------------------------------------------------------------------
		// Update Scene Information
		for (auto S3D : CSoundEditor::GetScene()->GetEntries())
		{
			S3D->SetVolume(0);
			S3D->UseVirtalPosition(true);
		}

		for (auto &S3D : objects)
		{
			S3D->SetVolume(100);
			S3D->ComputeControlProperties();
		}
	});

	// Start Tick
	startSound = (AudioSource*)Manager::GetAudio()->GetSoundEffect("tick2");

	SubscribeToEvent("Start-Expanding-Sphere");
	SubscribeToEvent("Stop-Expanding-Sphere");
	SubscribeToEvent("Resume-Expanding-Sphere");
}

void ExpandingSphereScript::Init()
{
	//Manager::GetScene()->AddObject(visibleSphere);
}

void ExpandingSphereScript::Start()
{
	Init();
	Reset();

	if (startSound)
		startSound->SetVolume((float)config->startScanFXVolume);
	scanPauseEvent->SetNewInterval(config->pauseBetweenScans);
	computeIntersection->Start();
	CSoundEditor::GetScene()->Play();
	SubscribeToEvent(EventType::FRAME_UPDATE);

	//sphereTransform->SetWorldPosition(camera->transform->GetWorldPosition());
}

void ExpandingSphereScript::Stop()
{
	computeIntersection->Stop();
	//Manager::GetScene()->RemoveObject(visibleSphere);
	CSoundEditor::GetScene()->Stop();
	UnsubscribeFrom(EventType::FRAME_UPDATE);
	dynamicEvents->Remove(*scanPauseEvent);

	// ------------------------------------------------------------------------
	// Update Scene Information
	for (auto S3D : CSoundEditor::GetScene()->GetEntries())
	{
		S3D->SetVolume(100);
		S3D->StopScore();
		S3D->UseVirtalPosition(false);
	}
}

void ExpandingSphereScript::ClearEvents()
{
}

void ExpandingSphereScript::Update()
{
	if (sphereScale == 0.05f && startSound)
		startSound->Play();

	// In view/camera space forward is oriented towards the -Z axis
	sphereScale += Engine::GetLastFrameTime() * config->travelSpeed;
	//visibleSphere->transform->SetScale(glm::vec3(sphereScale) * 2.0f);
	//sphereTransform->SetWorldPosition(camera->transform->GetWorldPosition());

	// View offset represents the distance between the plane and the computational model
	// The computation is based on the camera view space distance provided by the rendering pipeline
	// Since the visible plane/sphere affects the view space (like a wall in the rendering target) 
	// we need to make the computation using a slighly less value from the visible one in order to
	// guarantee that the plane/spehere will not have an effect on the computation model

	float viewOffset = max(sphereScale / 10.0f, 0.1f);
	float radius = sphereScale - viewOffset;

	computeIntersection->SetSphereSize(radius);

	// Reset position when
	if (radius > config->maxDistanceReach) {
		Reset();
	}
}

void ExpandingSphereScript::Reset()
{
	sphereScale = 0.05f;
	computeIntersection->SetSphereSize(sphereScale);
	//visibleSphere->transform->SetScale(glm::vec3(0.1f));

 	for (auto S3D : CSoundEditor::GetScene()->GetEntries()) {
		S3D->SetVolume(0);
	}

	if (config->pauseBetweenScans)
	{
		computeIntersection->Stop();
		UnsubscribeFrom(EventType::FRAME_UPDATE);
		//Manager::GetScene()->RemoveObject(visibleSphere);
		dynamicEvents->Add(*scanPauseEvent);
	}
}

void ExpandingSphereScript::OnEvent(EventType eventID, void * data)
{
	if (eventID == EventType::FRAME_UPDATE)
		Update();
}

void ExpandingSphereScript::OnEvent(const string & eventID, void * data)
{
	if (eventID.compare("Start-Expanding-Sphere") == 0) {
		config = (ExpandingSphereConfig*)data;
		Start();
	}
	if (eventID.compare("Resume-Expanding-Sphere") == 0) {
		computeIntersection->Start();
		SubscribeToEvent(EventType::FRAME_UPDATE);
		//Manager::GetScene()->AddObject(visibleSphere);
		dynamicEvents->Remove(*scanPauseEvent);
	}
	if (eventID.compare("Stop-Expanding-Sphere") == 0) {
		Stop();
	}
}
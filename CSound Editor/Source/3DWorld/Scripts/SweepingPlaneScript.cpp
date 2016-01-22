#include "SweepingPlaneScript.h"

#include <iostream>
#include <algorithm> 

using namespace std;

#include <3DWorld/Csound/CSound3DSource.h>
#include <3DWorld/Csound/CSoundScene.h>

#include <3DWorld/Compute/SceneIntersection.h>
#include <3DWorld/Scripts/SoundModelsConfig.h>
#include <3DWorld/Scripts/SoundTriggerList.h>

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
#include <Component/AudioSource.h>
#include <Component/Transform/Transform.h>
#include <Component/Renderer.h>
#include <Event/TimerEvent.h>
#include <Manager/Manager.h>
#include <Manager/AudioManager.h>
#include <Manager/SceneManager.h>
#include <Manager/ResourceManager.h>
#include <Manager/EventSystem.h>
#include <UI/ColorPicking/ColorPicking.h>

static TimerManager<string> *dynamicEvents;

SweepingPlaneScript::SweepingPlaneScript()
{
	camera = Manager::GetScene()->GetActiveCamera();

	virtualPlane = Manager::GetResource()->GetGameObject("sphere");
	visiblePlane = new GameObject(*virtualPlane);

	visiblePlane->SetSelectable(false);
	visiblePlane->renderer->SetOpacity(0.2f);
	visiblePlane->renderer->SetCulling(OpenGL::CULL::NONE);
	visiblePlane->transform->SetScale(glm::vec3(0.1f));

	dynamicEvents = Manager::GetEvent()->GetDynamicTimers();
	scanPauseEvent = dynamicEvents->Create("Resume-Sweeping-Plane", 1);

	triggerList = new SoundTriggerList();

	// Moving Tick
	feedbackTick = new CSound3DSource();
	auto score = SoundManager::GetCSManager()->GetScore("feedback-tick");
	if (score) {
		feedbackTick->SetSoundModel(score, false);
	}

	SubscribeToEvent("Start-Sweeping-Plane");
	SubscribeToEvent("Stop-Sweeping-Plane");
	SubscribeToEvent("Resume-Sweeping-Plane");
}

void SweepingPlaneScript::Init()
{
	camera->AddChild(virtualPlane);
	virtualPlane->transform->SetLocalPosition(glm::vec3(0, 0, -5));
	auto cameraRotation = camera->transform->GetWorldRotation();
	visiblePlane->transform->SetWorldRotation(camera->transform->GetWorldRotation());
	Manager::GetScene()->AddObject(visiblePlane);
}

void SweepingPlaneScript::Start(SweepingPlaneConfig *config)
{
	this->config = config;

	Init();
	Reset();

	nrFeedbackTicks = int((90 / config->tickInterval)) + 1;
	scanPauseEvent->SetNewInterval(config->pauseBetweenScans);

	CSoundEditor::GetScene()->Play();
	for (auto S3D : CSoundEditor::GetScene()->GetEntries())
	{
		S3D->PlayScore();
		S3D->SetVolume(0);
	}

	SubscribeToEvent(EventType::FRAME_UPDATE);
}

void SweepingPlaneScript::Stop()
{
	feedbackTick->StopScore();
	camera->RemoveChild(virtualPlane);
	Manager::GetScene()->RemoveObject(visiblePlane);
	CSoundEditor::GetScene()->Stop();
	UnsubscribeFrom(EventType::FRAME_UPDATE);
	dynamicEvents->Remove(*scanPauseEvent);
}

void SweepingPlaneScript::Update()
{
	auto vPlaneTransform = virtualPlane->transform;

	scanningAzimuth = (sweepingTime / config->fullScanTime) * 90 - 45;
	auto scanAzRad = RADIANS(scanningAzimuth);
	float oz = (float)cos(scanAzRad);
	float ox = (float)sin(scanAzRad);
	glm::vec3 localPosition = glm::vec3(ox, 0, -oz) * config->maxDistanceReach;
	vPlaneTransform->SetLocalPosition(localPosition);
	//vPlaneTransform->SetLocalRotation(glm::quat(glm::vec3(0, scanningAzimuth, 0)));
	visiblePlane->transform->SetWorldPosition(vPlaneTransform->GetWorldPosition());

	if ((scanningAzimuth + 45) / config->tickInterval >= nrTicksReleased) {
		TriggerFeedbackTick();
	}

	for (auto S3D : CSoundEditor::GetScene()->GetEntries())
	{
		if (S3D->GetSoundVolume() == 0)
		{
			if (S3D->GetDistanceToCamera() < config->maxDistanceReach && abs(scanningAzimuth - S3D->GetAzimuthToCamera()) < 1) {
				if (triggerList->IsTracked(S3D) == false)
					triggerList->Add(S3D);
			}
		}
	}

	triggerList->Update();

	// Reset position when
	if (sweepingTime > config->fullScanTime)
	{
		TriggerFeedbackTick();
		cout << "[Scan End] \t" << vPlaneTransform->GetRelativeRotation() << endl;
		Reset();
	}

	sweepingTime += Engine::GetLastFrameTime();
}

void SweepingPlaneScript::Resume()
{
	SubscribeToEvent(EventType::FRAME_UPDATE);
	Manager::GetScene()->AddObject(visiblePlane);
	for (auto S3D : CSoundEditor::GetScene()->GetEntries())
	{
		S3D->SetVolume(0);
	}
	dynamicEvents->Remove(*scanPauseEvent);
	TriggerFeedbackTick();
}

void SweepingPlaneScript::Reset()
{
	sweepingTime = 0;
	scanningAzimuth = -45.0f;
	if (config->pauseBetweenScans)
	{
		UnsubscribeFrom(EventType::FRAME_UPDATE);
		Manager::GetScene()->RemoveObject(visiblePlane);
		dynamicEvents->Add(*scanPauseEvent);
	}
	nrTicksReleased = 0;
}

void SweepingPlaneScript::TriggerFeedbackTick()
{
	if (nrTicksReleased >= nrFeedbackTicks)
		return;
	int half = (nrFeedbackTicks - 1) / 2;
	float dist = (float)abs(nrTicksReleased - half) / half;
	uint value = uint((1.0f - dist) * 50) + 50;
	float azimuth = nrTicksReleased * 90.0f / (nrFeedbackTicks - 1) - 45.0f;
	cout << nrTicksReleased << " Volume " << value << "\tAzimuth " << azimuth << endl;
	feedbackTick->SetControlChannel("kAzimuth", azimuth);
	feedbackTick->SetControlChannel("kTickVolume", value / 100.0f * config->tickVolume);
	feedbackTick->SetPlaybackTime(0);
	feedbackTick->PlayScore();
	nrTicksReleased++;
}

void SweepingPlaneScript::OnEvent(EventType eventID, void * data)
{
	if (eventID == EventType::FRAME_UPDATE)
		Update();
}

void SweepingPlaneScript::OnEvent(const string & eventID, void * data)
{
	if (eventID.compare("Start-Sweeping-Plane") == 0) {
		Start((SweepingPlaneConfig*)data);
		return;
	}
	if (eventID.compare("Resume-Sweeping-Plane") == 0) {
		Resume();
		return;
	}
	if (eventID.compare("Stop-Sweeping-Plane") == 0) {
		Stop();
		return;
	}
}
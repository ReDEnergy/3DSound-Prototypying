#include <pch.h>
#include "MovingPlaneScript.h"

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
#include <Csound/CSoundSynthesizer.h>
#include <Csound/CSoundInstrument.h>

#include <CSoundEditor.h>

static TimerManager<string> *dynamicEvents;

MovingPlaneScript::MovingPlaneScript()
{
	camera = Manager::GetScene()->GetActiveCamera();

	virtualPlane = Manager::GetResource()->GetGameObject("plane");
	visiblePlane = new GameObject(*virtualPlane);

	visiblePlane->SetSelectable(false);
	visiblePlane->renderer->SetOpacity(0.2f);
	visiblePlane->renderer->SetCulling(OpenGL::CULL::NONE);
	visiblePlane->transform->SetScale(glm::vec3(10));

	dynamicEvents = Manager::GetEvent()->GetDynamicTimers();
	scanPauseEvent = dynamicEvents->Create("Resume-Moving-Plane", 1);

	triggerList = new SoundTriggerList();
	triggerList->OnTrigger(bind(&MovingPlaneScript::OnTriggerSound, this, placeholders::_1));

	// Moving Tick
	feedbackTick = (AudioSource*)Manager::GetAudio()->GetSoundEffect("tick2");

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

	CSoundEditor::GetScene()->SetOutputModel(NULL);

	auto score = SoundManager::GetCSManager()->GetScore("moving-plane");
	for (auto S3D : CSoundEditor::GetScene()->GetEntries())
	{
		S3D->SetSoundModel(score);
	}

	CSoundEditor::GetScene()->SetDefaultScore(score);

	// Create Global Control
	GlobalCsoundControlChannel controlChannel;
	controlChannel.label = "Gain M/S plane";
	controlChannel.name = "kGain";
	controlChannel.value = config->soundGain;
	Manager::GetEvent()->EmitSync("create-global-csound-control", (void*)(&controlChannel));
}

void MovingPlaneScript::Start(MovingPlaneConfig *config)
{
	this->config = config;

	Init();
	Reset();

	nrFeedbackTicks = uint(config->maxDistanceReach / config->tickInterval) + 1;

	feedbackTick->SetVolume(100);
	scanPauseEvent->SetNewInterval(config->pauseBetweenScans);

	//computeIntersection->Start();
	CSoundEditor::GetScene()->Play();
	for (auto S3D : CSoundEditor::GetScene()->GetEntries())
	{
		S3D->PlayScore();
		S3D->SetVolume(0);
	}

	SubscribeToEvent(EventType::FRAME_UPDATE);
}

void MovingPlaneScript::Stop()
{
	//computeIntersection->Stop();
	camera->RemoveChild(virtualPlane);
	Manager::GetScene()->RemoveObject(visiblePlane);
	CSoundEditor::GetScene()->Stop();
	UnsubscribeFrom(EventType::FRAME_UPDATE);
	dynamicEvents->Remove(*scanPauseEvent);
}

void MovingPlaneScript::Resume()
{
	SubscribeToEvent(EventType::FRAME_UPDATE);
	Manager::GetScene()->AddObject(visiblePlane);
	for (auto S3D : CSoundEditor::GetScene()->GetEntries())
	{
		S3D->SetVolume(0);
	}
	dynamicEvents->Remove(*scanPauseEvent);
}

void MovingPlaneScript::Update()
{
	// In view/camera space forward is oriented towards the -Z axis
	auto forward = -glm::vec3(0, 0, 1);

	auto vPlaneTransform = virtualPlane->transform;
	auto localPosition = vPlaneTransform->GetLocalPosition();
	
	localPosition += forward * Engine::GetLastFrameTime() * config->travelSpeed;

	vPlaneTransform->SetLocalPosition(localPosition);

	visiblePlane->transform->SetWorldPosition(vPlaneTransform->GetWorldPosition());
	visiblePlane->transform->SetWorldRotation(vPlaneTransform->GetWorldRotation());

	if (-localPosition.z / config->tickInterval >= nrTicksReleased) {
		TriggerFeedbackTick();
	}

	for (auto S3D : CSoundEditor::GetScene()->GetEntries())
	{
		// get forward distance to the plane (created by the object) parralel to the camera view 
		// cos(elevation) = proj / radius => proj = cos(elevation) * radius
		// coz(azimuth) = forward / proj => forward = cos(azimuth) * proj
		// forward = cos(azimuth) * cos(elevation) * radius

		if (S3D->GetSoundVolume() == 0)
		{
			float relDist = float(cos(RADIANS(S3D->GetAzimuthToCamera())) * cos(RADIANS(S3D->GetElevationToCamera())) * S3D->GetDistanceToCamera());
			if (abs(localPosition.z + relDist) < 0.1) {
				if (triggerList->IsTracked(S3D) == false)
					triggerList->Add(S3D);
			}
		}
	}

	triggerList->Update();

	// View offset represents the distance between the plane and the computational model
	// The computation is based on the camera view space distance provided by the rendering pipeline
	// Since the visible plane/sphere affects the view space (like a wall in the rendering target) 
	// we need to make the computation using a slighly less value from the visible one in order to
	// guarantee that the plane/spehere will not have an effect on the computation model

	float viewOffset = 0.1f;

	// Reset position when
	if (abs(-localPosition.z + viewOffset) > config->maxDistanceReach)
	{
		cout << "[Scan End] \t" << localPosition.z << endl;
		Reset();
	}
}

void MovingPlaneScript::Reset()
{
	if (config->pauseBetweenScans)
	{
		UnsubscribeFrom(EventType::FRAME_UPDATE);
		Manager::GetScene()->RemoveObject(visiblePlane);
		dynamicEvents->Add(*scanPauseEvent);
		visiblePlane->transform->SetWorldPosition(virtualPlane->transform->GetWorldPosition());
	}
	nrTicksReleased = 0;
	virtualPlane->transform->SetLocalPosition(glm::vec3(0, 0, 0.5f));
}

void MovingPlaneScript::TriggerFeedbackTick()
{
	if (nrTicksReleased >= nrFeedbackTicks)
		return;
	float volume = 100 * float(nrFeedbackTicks - nrTicksReleased) / nrFeedbackTicks;
	cout << "Tick volume " << volume << endl;
	feedbackTick->SetVolume(volume);
	feedbackTick->Play();
	nrTicksReleased++;
}

void MovingPlaneScript::OnTriggerSound(CSound3DSource *source)
{
	float distance = source->GetDistanceToCamera();
	float a = 1 - (distance / config->maxDistanceReach);
	float pitch = lerp(8.01f, 8.10f, a);
	float duration = lerp(0.1f, 1, a);
	float az = source->GetAzimuthToCamera();
	float el = source->GetElevationToCamera();
	double p[4] = { 1, 0, duration, pitch };
	source->SetVolumeOvershoot(100, duration);
	source->SendCsoundEvent('i', 4, p);
}

void MovingPlaneScript::OnEvent(EventType eventID, void * data)
{
	if (eventID == EventType::FRAME_UPDATE)
		Update();
}

void MovingPlaneScript::OnEvent(const string & eventID, void * data)
{
	if (eventID.compare("Start-Moving-Plane") == 0) {
		Start((MovingPlaneConfig*)data);
		return;
	}
	if (eventID.compare("Resume-Moving-Plane") == 0) {
		Resume();
		return;
	}
	if (eventID.compare("Stop-Moving-Plane") == 0) {
		Stop();
		return;
	}
}
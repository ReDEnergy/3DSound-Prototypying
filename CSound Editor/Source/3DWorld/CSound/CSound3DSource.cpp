#include <pch.h>
#include "CSound3DSource.h"

#include <CSoundEditor.h>
#include <SoundManager.h>
#include <3DWorld/CSound/CSoundScene.h>
#include <3DWorld/Game.h>
#include <3DWorld/Compute/SurfaceArea.h>

// CSound lib headers
#include <CSound/CSoundPlayer.h>
#include <CSound/CSoundSynthesizer.h>

#define RESTRICT(value, min, max)  (value < min) ? min : (value > max) ? max : value

CSound3DSource::CSound3DSource()
	: GameObject("")
{
	SetName("sound-source");
	SetMesh(Manager::GetResource()->GetMesh("sphere"));
	transform->SetWorldPosition(glm::vec3(0, 1, 0));

	forceUpdateSoundParameters = false;
	useVirtualPosition = false;
	soundModel = nullptr;
	player = nullptr;
	Init();
}

CSound3DSource::CSound3DSource(const CSound3DSource & ref)
	: GameObject(ref)
{
	SetSoundModel(ref.soundModel);
	Init();
}

void CSound3DSource::Init()
{
	distanceToCamera = 0;
	surfaceArea = 0;
	surfaceCover = 0;
	azimuth = 0;
	elevation = 0;
	positionCameraSpace = glm::vec3(0);
	soundVolume = 100;
	soundFallOff = 20;
	elevationPanningFactor = 0;
	azimuthPanningFactor = 0;

	stopTimer = new SimpleTimer(1);
	stopTimer->OnExpire([this]() {
		StopScore();
	});

	volumeTimer = new SimpleTimer(1);
	volumeTimer->OnExpire([this]() {
		SetVolume(0);
	});

	SubscribeToEvent("model-changed");
}

CSound3DSource::~CSound3DSource()
{
	UnsubscribeFrom("model-changed");
	StopScore();
	if (soundModel)
		CSoundEditor::GetScene()->UnTrackScore(soundModel);
	cout << "DELETED SOURCE" << endl;
}

void CSound3DSource::SetSoundModel(CSoundSynthesizer * soundModel, bool trackScore)
{
	// TODO: Tracking should be done only by the scene manager
	if (trackScore) {
		if (this->soundModel) {
			CSoundEditor::GetScene()->UnTrackScore(this->soundModel);
		}
		if (soundModel) {
			CSoundEditor::GetScene()->TrackScore(soundModel);
		}
	}
	this->soundModel = soundModel;

	if (soundModel) {
		SAFE_FREE(player);
		player = new CSoundPlayer(soundModel);
		player->LoadCSD();
	}
}

void CSound3DSource::Update()
{
	Transform* cameraTransform = Manager::GetScene()->GetActiveCamera()->transform;
	bool motion = transform->GetMotionState() || cameraTransform->GetMotionState();
	if (motion) {
		ComputeControlProperties();
	}
	UpdateControlChannels(motion);
}

void CSound3DSource::SelectObject()
{
	Manager::GetPicker()->SetSelectedObject(this);
}

bool CSound3DSource::GetPlaybackState() const
{
	return player ? player->IsPlaying() : false;
}

bool CSound3DSource::TooglePlayback()
{
	if (player->IsPlaying()) {
		StopScore();
		return false;
	}
	PlayScore();
	return true;
}

void CSound3DSource::PlayScore()
{
	if (player && player->IsPlaying() == false) {
		soundVolume = 100;
		player->Play();
		ComputeControlProperties();
		UpdateControlChannels();
	}
}

void CSound3DSource::PlayScore(float deltaTime)
{
	if (player && player->IsPlaying() == false) {
		player->Play();
		ComputeControlProperties();
		UpdateControlChannels();
		stopTimer->SetDuration(deltaTime);
		stopTimer->Start();
	}
}

void CSound3DSource::StopScore()
{
	if (player && player->IsPlaying())
		player->Stop();
}

void CSound3DSource::ReloadScore()
{
	if (player)
		player->Reload();
}

void CSound3DSource::OnEvent(const string& eventID, void * data)
{
	if (eventID.compare("model-changed") == 0) {
		auto model = (CSoundSynthesizer*)data;
		if (soundModel != model) {
			return;
		}

		if (player) {
			bool state = player->Reload();
			if (state) {
				ComputeControlProperties();
				UpdateControlChannels();
			}
		}
		return;
	}
}

void CSound3DSource::ComputeControlProperties()
{
	Transform* cameraTransform = Manager::GetScene()->GetActiveCamera()->transform;

	if (useVirtualPosition) {
		positionCameraSpace = virtualCameraSpacePos;
	}
	else {
		auto cameraPos = cameraTransform->GetWorldPosition();
		auto worldPosition = transform->GetWorldPosition();
		positionCameraSpace = glm::rotate(glm::inverse(cameraTransform->GetWorldRotation()), worldPosition - cameraPos);
	}

	// Compute Distance to camera
	distanceToCamera = glm::length(positionCameraSpace);

	// ------------------------------------------------------------------------
	// Compute Sqare Attenuation

	soundIntensity = 0;
	//if (surfaceCover) {
		soundIntensity = 1 - distanceToCamera / soundFallOff;
		soundIntensity *= max(0, soundIntensity);
	//}

	// Camera Space
	if (distanceToCamera)
	{
		auto targetCameraSpace = glm::normalize(positionCameraSpace);
		auto targetProjection = glm::normalize(glm::vec3(targetCameraSpace.x, 0, targetCameraSpace.z));

		// ------------------------------------------------------------------------
		// Compute Azimuth
		float value = glm::dot(glm::vec3(0, 0, 1), targetProjection);
		value = RESTRICT(value, -1, 1);
		azimuth = 180 - acos(value) * TO_DEGREES;
		if (positionCameraSpace.x < 0)
			azimuth = -azimuth;

		// ------------------------------------------------------------------------
		// Compute Elevation

		value = glm::dot(targetProjection, targetCameraSpace);
		value = RESTRICT(value, -1, 1);
		elevation = acos(value) * TO_DEGREES;
		if (positionCameraSpace.y < 0)
			elevation = -elevation;
	}

	// Compute azimuth panning factor
	// 0 only left channel, 1 only right channel
	azimuthPanningFactor = azimuth / 180 + 0.5f;
	azimuthPanningFactor = min(max(azimuthPanningFactor, 0), 1);

	// Compute elevation panning factor
	// 0 only bottom, 1 only top
	elevationPanningFactor = elevation / 90 + 0.5f;
	elevationPanningFactor = min(max(elevationPanningFactor, 0), 1);
}

void CSound3DSource::UpdateControlChannels(bool motion) const
{
	if (!player || !player->IsPlaying()) return;

	// ------------------------------------------------------------------------
	// Update Control Channels
	player->SetControl("kOutput", (float)SoundManager::GetGlobalOutputModelIndex());
	player->SetControl("kAzPanFactor", azimuthPanningFactor);
	player->SetControl("kElPanFactor", elevationPanningFactor);
	player->SetControl("kSurfaceCover", surfaceCover);
	player->SetControl("kDistance", distanceToCamera);
	player->SetControl("kAzimuth", azimuth);
	player->SetControl("kElevation", elevation);
	player->SetControl("kAttenuation", soundIntensity);
	player->SetControl("kVolume", soundVolume / 100.0f);
}

const char * CSound3DSource::GetRender() const
{
	if (soundModel) {
		return soundModel->GetRender();
	}
	return nullptr;
}

CSoundSynthesizer* CSound3DSource::GetScore() const
{
	return soundModel;
}

float CSound3DSource::GetDistanceToCamera() const
{
	return distanceToCamera;
}

int CSound3DSource::GetSurfaceArea() const
{
	return surfaceArea;
}

float CSound3DSource::GetSurfaceCover() const
{
	return surfaceCover;
}

float CSound3DSource::GetElevationToCamera() const
{
	return elevation;
}

float CSound3DSource::GetAzimuthToCamera() const
{
	return azimuth;
}

float CSound3DSource::GetAzimuthPanningFactor() const
{
	return azimuthPanningFactor;
}

float CSound3DSource::GetElevationPanningFactor() const
{
	return elevationPanningFactor;
}

unsigned int CSound3DSource::GetSoundVolume() const
{
	return soundVolume;
}

float CSound3DSource::GetSoundIntensity() const
{
	return soundIntensity;
}

const glm::vec3 & CSound3DSource::GetCameraSpacePosition() const
{
	return positionCameraSpace;
}

void CSound3DSource::SetVirtualCameraSpacePosition(glm::vec3 cameraSpacePosition)
{
	virtualCameraSpacePos = cameraSpacePosition;
}

void CSound3DSource::SetSurfaceArea(unsigned int visibleAreaInPixels)
{
	surfaceArea = visibleAreaInPixels;
	auto res = Engine::Window->GetResolution();
	surfaceCover = float(surfaceArea) / (res.x * res.y);
}

void CSound3DSource::SetPlaybackTime(float time)
{
	player->SetPlaybackTime(time);
}

void CSound3DSource::SetVolume(unsigned int value)
{
	soundVolume = max(min(value, 100), 0);
}

void CSound3DSource::SetVolumeOvershoot(unsigned int value, float deltaTime)
{
	soundVolume = max(min(value, 100), 0);
	volumeTimer->SetDuration(deltaTime);
	volumeTimer->Start();
}

void CSound3DSource::SetControlChannel(const char * channel, float value, bool forceUpdate) const
{
	player->SetControl(channel, value, forceUpdate);
}

void CSound3DSource::SendCsoundEvent(char eventType, int nrParams, double * params) const
{
	if (player)
		player->SendEvent(eventType, nrParams, params);
}

void CSound3DSource::UseVirtalPosition(bool value)
{
	useVirtualPosition = value;
}
#include "CSound3DSource.h"
#include <iostream>

#include <CSoundEditor.h>
#include <3DWorld/CSound/CSoundScene.h>

// CSound lib headers
#include <CSound/CSoundPlayer.h>
#include <CSound/CSoundScore.h>

// Engine headers
#include <include/glm.h>
#include <include/glm_utils.h>
#include <include/utils.h>

#include <Core/Engine.h>
#include <Core/WindowObject.h>
#include <Core/Camera/Camera.h>

#include <Component/Transform/Transform.h>

#include <Manager/Manager.h>
#include <Manager/SceneManager.h>
#include <Manager/ResourceManager.h>
#include <UI/ColorPicking/ColorPicking.h>


CSound3DSource::CSound3DSource()
	: GameObject("")
{
	mesh = Manager::GetResource()->GetMesh("sphere");
	SetupAABB();
	transform->SetWorldPosition(glm::vec3(0, 1, 0));

	Manager::GetScene()->AddObject(this);
	soundModel = nullptr;
	player = nullptr;

	SubscribeToEvent("model-changed");
}

CSound3DSource::~CSound3DSource()
{
	CSoundEditor::GetScene()->UnTrackScore(this->soundModel);
	cout << "DELETED SOURCE" << endl;
	StopScore();
}

void CSound3DSource::SetSoundModel(CSoundScore * soundModel)
{
	// TODO: Tracking should be done only by the scene manager
	if (this->soundModel)
		CSoundEditor::GetScene()->UnTrackScore(this->soundModel);
	this->soundModel = soundModel;
 	CSoundEditor::GetScene()->TrackScore(soundModel);

	SAFE_FREE(player);
	player = new CSoundPlayer(soundModel);
	player->Init();
}

void CSound3DSource::SetSufaceArea(unsigned int surface)
{
	surfaceArea = surface;
	surfaceCover = float(surface) / (Engine::Window->resolution.x * Engine::Window->resolution.y);
}

void CSound3DSource::Update() {

	if (!player->IsPlaying())
		return;

	Transform* cameraTransform = Manager::GetScene()->GetActiveCamera()->transform;
	if (transform->GetMotionState() || cameraTransform->GetMotionState())
	{
		UpdateControlChannels();
	}
};

void CSound3DSource::SelectObject()
{
	Manager::GetPicker()->SetSelectedObject(this);
}

void CSound3DSource::PlayScore()
{
	if (player) {
		player->Play();
		UpdateControlChannels();
	}
}

void CSound3DSource::StopScore()
{
	if (player)
		player->Stop();
}

void CSound3DSource::ReloadScore()
{
	if (player)
		player->Reload();
}

void CSound3DSource::OnEvent(string eventID, void * data)
{
	if (eventID.compare("model-changed") == 0) {
		auto model = (CSoundScore*)data;
		if (soundModel != model) {
			return;
		}

		bool state = player->Reload();
		if (state)
			UpdateControlChannels();

		return;
	}
}

void CSound3DSource::UpdateControlChannels()
{
	Transform* cameraTransform = Manager::GetScene()->GetActiveCamera()->transform;

	glm::vec3 cameraPos = cameraTransform->GetWorldPosition();
	glm::vec3 targetPos = transform->GetWorldPosition();
	glm::vec3 toTarget = targetPos - cameraPos;
	glm::vec3 forward = cameraTransform->GetLocalOZVector();

	// ------------------------------------------------------------------------
	// Compute Sqare Attenuation

	float distance = glm::length(toTarget);
	float intensity = (100 - min(100, distance)) / 100.0f;
	intensity *= intensity;
	//cout << "dist: " << attenuation << " vol: " << attenuation << endl;

	// ------------------------------------------------------------------------
	// Compute Azimuth
	int sign = glm::GetSideOfPointToLine(cameraPos, cameraPos + forward, targetPos);

	// projection on OY plane
	glm::vec2 forward2D = glm::normalize(glm::vec2(forward.x, forward.z));
	glm::vec2 toTarget2D = glm::normalize(glm::vec2(toTarget.x, toTarget.z));

	float value = glm::dot(forward2D, toTarget2D);
	float azimuth = acos(value) * sign * TO_DEGREES;
	//cout << value << "\t" << azimuth << endl;

	// ------------------------------------------------------------------------
	// Compute Elevation

	float elevation = asin(toTarget.y / distance) * TO_DEGREES;

	// ------------------------------------------------------------------------
	// Update Control Channels

	player->SetControl("kDistance", distance);
	player->SetControl("kAzimuth", azimuth);
	player->SetControl("kElevation", elevation);
	player->SetControl("kAttenuation", intensity);
	player->SetControl("kSurfaceCover", surfaceCover);
}

const char * CSound3DSource::GetRender() const
{
	if (soundModel) {
		return soundModel->GetRender();
	}
	return nullptr;
}

CSoundScore* CSound3DSource::GetScore() const
{
	return soundModel;
}
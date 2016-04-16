#include <pch.h>
#include "ImpactSoundScript.h"

#include <iostream>
#include <algorithm> 

using namespace std;

#include <include/gl.h>

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

ImpactSoundScript::ImpactSoundScript()
{
	SubscribeToEvent("Start-Impact-Sound");
	SubscribeToEvent("Stop-Impact-Sound");
	SubscribeToEvent("Resume-Impact-Sound");
}

void ImpactSoundScript::Init()
{
	auto score = SoundManager::GetCSManager()->GetScore("impact-sound");
	score->Rebuild();
	for (auto S3D : CSoundEditor::GetScene()->GetEntries())
	{
		S3D->SetSoundModel(score);
	}

	CSoundEditor::GetScene()->SetDefaultScore(score);
}

void ImpactSoundScript::Start(ImpactSoundConfig *config)
{
	this->config = config;
	isRunning = true;

	Init();
	Reset();

	CSoundEditor::GetScene()->Play();
	for (auto S3D : CSoundEditor::GetScene()->GetEntries())
	{
		S3D->PlayScore();
		S3D->SetControlChannel("kFreq", 440);
	}

	//SubscribeToEvent(EventType::FRAME_UPDATE);
}

void ImpactSoundScript::Stop()
{
	isRunning = false;
	CSoundEditor::GetScene()->Stop();
	//UnsubscribeFrom(EventType::FRAME_UPDATE);
}

void ImpactSoundScript::Update()
{
	int nrSources = CSoundEditor::GetScene()->GetEntries().size();

	double params[3] = { 1.0, 0, 0.25 };
	for (auto S3D : CSoundEditor::GetScene()->GetEntries())
	{
		auto distance = S3D->GetDistanceToCamera();

		float frequency = 840 / max(distance, 0.25f);
		S3D->SetControlChannel("kFreq", frequency);

		float repetitionRate = 0.1f * (distance + nrSources - 1);
		params[2] = repetitionRate;
		S3D->SendCsoundEvent('i', 3, params);
	}
}

void ImpactSoundScript::Reset()
{
}

void ImpactSoundScript::OnKeyPress(int key, int mods)
{
	if (isRunning) {
		if (key == GLFW_KEY_T) {
			Update();
		}
	}
}

void ImpactSoundScript::OnEvent(EventType eventID, void * data)
{
	if (eventID == EventType::FRAME_UPDATE)
		Update();
}

void ImpactSoundScript::OnEvent(const string & eventID, void * data)
{
	if (eventID.compare("Start-Impact-Sound") == 0) {
		Start((ImpactSoundConfig*)data);
		return;
	}
	if (eventID.compare("Stop-Impact-Sound") == 0) {
		Stop();
		return;
	}
}
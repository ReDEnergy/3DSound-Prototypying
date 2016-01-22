#include "SoundTriggerList.h"

#include <iostream>
#include <algorithm> 

using namespace std;

#include <3DWorld/Csound/CSound3DSource.h>

// Engine
#include <Core/Engine.h>

SoundTriggerList::SoundTriggerList()
{
	activeSource = nullptr;
}

void SoundTriggerList::Add(CSound3DSource * obj)
{
	if (activeSource == nullptr) {
		TriggerAction(obj);
		return;
	}
	triggerList.push_back(obj);
}

bool SoundTriggerList::IsTracked(CSound3DSource * obj)
{
	if (activeSource == obj)
		return true;

	auto elem = std::find(triggerList.begin(), triggerList.end(), obj);
	if (elem != triggerList.end())
		return true;

	return false;
}

void SoundTriggerList::Update()
{
	if (activeSource && activeSource->GetSoundVolume() > 0)
		activeSource = nullptr;
	if (triggerList.size() == 0)
		return;
	if (Engine::GetElapsedTime() - lastTriggerTime > 0.2f) {
		auto obj = triggerList.front();
		triggerList.pop_front();
		TriggerAction(obj);
	}
}

void SoundTriggerList::TriggerAction(CSound3DSource *obj)
{
	//obj->PlayScore(max(obj->GetSurfaceCover(), 0.05f));
	obj->SetVolumeOvershoot(100, max(obj->GetSurfaceCover(), 0.1f));
	activeSource = obj;
	lastTriggerTime = (float)Engine::GetElapsedTime();
}

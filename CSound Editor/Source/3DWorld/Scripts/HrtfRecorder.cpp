#include "HrtfRecorder.h"

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
#include <Core/GameObject.h>
#include <Core/Camera/Camera.h>
#include <Component/Transform/Transform.h>
#include <Manager/Manager.h>
#include <Manager/SceneManager.h>
#include <Manager/EventSystem.h>
#include <UI/ColorPicking/ColorPicking.h>

static TimerManager<string> *dynamicEvents;

HrtfRecorder::HrtfRecorder()
{
	dynamicEvents = Manager::GetEvent()->GetDynamicTimers();

	waitEvent = dynamicEvents->Create("record-HRTF-wait", 0.5f);
	playbackEvent = dynamicEvents->Create("record-HRTF-update", 1.5f);

	SubscribeToEvent("record-HRTF-update");
	SubscribeToEvent("record-HRTF-wait");
}

void HrtfRecorder::Start()
{
	CSoundEditor::GetScene()->Clear();
	Manager::GetEvent()->EmitAsync("UI-clear-scene");

	auto camera = Manager::GetScene()->GetActiveCamera();
	camera->transform->SetWorldPosition(glm::vec3(0, 4, -10));
	camera->transform->SetWorldRotation(glm::vec3(0, 0, 0));

	gameObj = new CSound3DSource();
	gameObj->SetName("HRTF source");

	CSoundEditor::GetScene()->AddSource(gameObj);
	Manager::GetPicker()->SetSelectedObject(gameObj);
	Manager::GetEvent()->EmitAsync("UI-add-object", gameObj);

	positions.clear();
	positions.push_back(glm::vec3(0, 4, 0));
	positions.push_back(glm::vec3(0, 4, 0));
	positions.push_back(glm::vec3(-4, 7, 0));
	positions.push_back(glm::vec3(-4, 4, 0));
	positions.push_back(glm::vec3(4, 7, 0));
	positions.push_back(glm::vec3(4, 4, 0));
	positions.push_back(glm::vec3(4, 1, 0));
	positions.push_back(glm::vec3(0, 7, 0));
	positions.push_back(glm::vec3(0, 1, 0));
	positions.push_back(glm::vec3(-4, 1, 0));

	auto score = SoundManager::GetCSManager()->GetScore("record-hrtf");
	auto instr = score->GetEntries().front();

	instr->SetDuration((int)ceil(positions.size() * 2));
	score->Save();
	gameObj->SetSoundModel(score);

	timelinePos = 0;
	CSoundEditor::GetScene()->Play();
	dynamicEvents->Add(playbackEvent);
}

void HrtfRecorder::ClearEvents()
{
	if (playbackEvent)
		dynamicEvents->Remove(playbackEvent);
	if (waitEvent)
		dynamicEvents->Remove(playbackEvent);
}

void HrtfRecorder::Update()
{
}

void HrtfRecorder::Stop()
{
	Manager::GetEvent()->GetDynamicTimers()->Remove(playbackEvent);
}

void HrtfRecorder::OnEvent(const string& eventID, void * data)
{
	if (eventID.compare("record-HRTF-update") == 0) {
		cout << "wait for 0.5 sec" << endl;
		gameObj->transform->SetWorldPosition(glm::vec3(100, 100, 100));
		dynamicEvents->Remove(playbackEvent);
		dynamicEvents->Add(waitEvent);
	};
	if (eventID.compare("record-HRTF-wait") == 0) {
		timelinePos++;
		if (timelinePos < positions.size())
			gameObj->transform->SetWorldPosition(positions[timelinePos]);
		else {
			ClearEvents();
			return;
		}
		dynamicEvents->Add(playbackEvent);
		dynamicEvents->Remove(waitEvent);
	};
}
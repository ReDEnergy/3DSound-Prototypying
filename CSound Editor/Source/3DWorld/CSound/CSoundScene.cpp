﻿#include "CSoundScene.h"
#include <ctime>

#include <include/pugixml.h>

#include <3DWorld/CSound/CSound3DSource.h>
#include <3DWorld/Game.h>
#include <CSoundEditor.h>

#include <CSound/CSound.h>

// Engine library
#include <include/glm.h>
#include <include/glm_utils.h>
#include <Component/Mesh.h>
#include <Component/Transform/Transform.h>
#include <Manager/Manager.h>
#include <Manager/ColorManager.h>
#include <Manager/ResourceManager.h>
#include <Manager/SceneManager.h>
#include <Manager/SceneManager.h>
#include <Manager/EventSystem.h>
#include <UI/ColorPicking/ColorPicking.h>
#include <Utils/Serialization.h>


using namespace pugi;
using namespace std;


CSoundScene::CSoundScene()
{
	sceneFile = "";
	defaultScore = nullptr;
	playback = false;
	SubscribeToEvent(EventType::EDITOR_OBJECT_REMOVED);
	_3DSources = nullptr;
}

CSoundScene::~CSoundScene()
{
	SAFE_FREE(_3DSources);
}

void CSoundScene::Init()
{
	_3DSources = new EntityStorage<CSound3DSource>("3DSource ");
	SetDefaultScore(SoundManager::GetCSManager()->GetScore("simple"));
	Clear();
	auto obj = CreateSource();
	SetOutputModel("hrtf-output");
}

void CSoundScene::Update()
{
	//for (auto S3D : _3DSources->GetEntries()) {
	//}
}

void CSoundScene::Clear()
{
	playback = false;
	Manager::GetPicker()->ClearSelection();
	Manager::GetEvent()->EmitSync(EventType::EDITOR_NO_SELECTION);
	for (auto S3D : _3DSources->GetEntries()) {
		Manager::GetScene()->RemoveObject(S3D, true);
	}
	_3DSources->Clear();
}

void CSoundScene::Play()
{
	playback = true;
	for (auto source : _3DSources->GetEntries()) {
		source->PlayScore();
	}
}

void CSoundScene::Stop()
{
	playback = false;
	for (auto source : _3DSources->GetEntries()) {
		source->StopScore();
	}
}

void CSoundScene::AddSource(CSound3DSource * S3D)
{
	Manager::GetScene()->AddObject(S3D);
	_3DSources->Set(S3D->GetName(), S3D);
}

CSound3DSource * CSoundScene::CreateSource()
{
	auto key = _3DSources->Create();
	auto source = _3DSources->Get(key.c_str());
	source->SetName(key.c_str());
	source->SetSoundModel(defaultScore);
	if (playback)
		source->PlayScore();

	Manager::GetScene()->AddObject(source);
	return source;
}

CSoundScore * CSoundScene::GetDefaultScore()
{
	return defaultScore;
}

const vector<CSound3DSource*>& CSoundScene::GetEntries() const
{
	return _3DSources->GetEntries();
}

bool CSoundScene::Rename(CSound3DSource * S3D, const char * newName)
{
	if (_3DSources->Rename(S3D->GetName(), newName)) {
		S3D->SetName(newName);
		return true;
	}
	return false;
}

void CSoundScene::Remove(CSound3DSource * S3D)
{
	_3DSources->Remove(S3D->GetName());
	Manager::GetScene()->RemoveObject(S3D, true);
}

void CSoundScene::SetDefaultScore(CSoundScore * score)
{
	if (defaultScore && strcmp(defaultScore->GetName(), score->GetName()) == 0)
		return;
	defaultScore = new CSoundScore(*score);
}

void CSoundScene::SetOutputModel(const char * name)
{
	SoundManager::SetGlobalOutputModel(name);

	// Run Update() only once for each instrument and score
	for (auto SS : sceneScores.GetEntities()) {
		SS->PreventUpdate();
	}

	for (auto SI : sceneInstruments.GetEntities()) {
		SI->Update();
	}

	for (auto SS : sceneScores.GetEntities()) {
		SS->ResumeUpdate();
		SS->Update();
		SS->Save();
	}

	// Update player in scene
	for (auto S3D : _3DSources->GetEntries()) {
		S3D->ReloadScore();
	}
}

void CSoundScene::AutoSave()
{
	time_t now = time(0);
	char fileName[32];
	strftime(fileName, 32, "Saves/Scene %d-%b [%Hh%Mm].xml", std::localtime(&now));
	SaveSceneAs(fileName);
}

bool CSoundScene::SaveScene()
{
	if (sceneFile.length()) {
		SaveSceneAs(sceneFile.c_str());
		return true;
	}
	return false;
}

void CSoundScene::SaveSceneAs(const char* path)
{
	string fileName(path);

	// Verify if .xml extension is appened; If not add it
	int len = fileName.length();
	if (len < 4 || (path[len - 4] != '.' && path[len - 3] != 'x' && path[len - 3] != 'm' && path[len - 1] != 'l')) {
		fileName.append(".xml");
	}
	sceneFile = fileName;

	///////////////////////////////////////////////////////////////////////////
	// Save Scores

	vector<CSoundScore*> scores;
	for (auto S3D : _3DSources->GetEntries()) {
		scores.push_back(S3D->GetScore());
	}
	auto doc = CSoundSerialize::GetXML(scores);

	///////////////////////////////////////////////////////////////////////////
	// Save Scene

	xml_node nodeScene = doc->append_child("scene");

	for (auto S3D : _3DSources->GetEntries())
	{
		auto sourceNode = nodeScene.append_child("object");

		CreateNode("name", S3D->GetName(), sourceNode);
		if (S3D->mesh) {
			CreateNode("meshID", S3D->mesh->GetMeshID(), sourceNode);
		}
		CreateNode("scoreID", S3D->GetScore()->GetUID(), sourceNode);

		auto transformNode = sourceNode.append_child("transform");

		auto pos = glm::Serialize<glm::vec3>(S3D->transform->GetWorldPosition());
		auto quat = glm::Serialize<glm::quat>(S3D->transform->GetWorldRotation());
		auto scale = glm::Serialize<glm::vec3>(S3D->transform->GetScale());

		CreateNode("position", pos.c_str(), transformNode);
		CreateNode("quaternion", quat.c_str(), transformNode);
		CreateNode("scale", scale.c_str(), transformNode);
	}

	///////////////////////////////////////////////////////////////////////////
	// Save Document
	doc->save_file(fileName.c_str());
}

void CSoundScene::LoadScene(const char* fileName)
{
	Clear();
	auto soundModels = CSoundSerialize::Load(fileName);

	///////////////////////////////////////////////////////////////////////////
	// Open Document
	auto doc = LoadXML(fileName);

	///////////////////////////////////////////////////////////////////////////
	// Load Scene

	xml_node nodeScene = doc->child("scene");

	for (auto nodeObj : nodeScene.children())
	{
		auto name = nodeObj.child_value("name");
		auto meshID = nodeObj.child_value("meshID");
		auto scoreID = nodeObj.child("scoreID").text().as_uint();

		CSound3DSource *GO = new CSound3DSource();
		GO->SetName(name);
		GO->SetMesh(Manager::GetResource()->GetMesh(meshID));

		auto transformNode = nodeObj.child("transform");
		Serialization::ReadTransform(transformNode, *GO->transform);

		if (soundModels.find(scoreID) != soundModels.end()) {
			GO->SetSoundModel(soundModels[scoreID]);
			AddSource(GO);
		}
		else {
			cout << "[ERROR] SoundModel ID: " << scoreID << " could not be found!" << endl;
		}
	}
}

void CSoundScene::TrackScore(CSoundScore * score)
{
	sceneScores.Add(score);
	for (auto I : score->GetEntries()) {
		sceneInstruments.Add(I);
	}
}

void CSoundScene::UnTrackScore(CSoundScore * score)
{
	sceneScores.Remove(score);
	for (auto I : score->GetEntries()) {
		sceneInstruments.Remove(I);
	}
}

void CSoundScene::TrackInstrument(CSoundInstrument * instr)
{
	sceneInstruments.Add(instr);
}

void CSoundScene::UnTrackInstrument(CSoundInstrument * instr)
{
	sceneInstruments.Remove(instr);
}

void CSoundScene::OnEvent(EventType Event, void * data)
{
	if (Event == EventType::EDITOR_OBJECT_REMOVED) {
		auto obj = (GameObject*)data;
		_3DSources->Remove(obj->GetName());
	}
}

#include "CSoundScene.h"
#include <ctime>

#include <include/pugixml.h>

#include <3DWorld/CSound/CSound3DSource.h>
#include <3DWorld/Game.h>
#include <CSoundEditor.h>

#include <CSound/CSound.h>

// Engine library
#include <include/glm.h>
#include <include/glm_utils.h>
#include <Core/Camera/Camera.h>
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
	_3DSources = new EntityStorage<CSound3DSource>("3DSource ");

	SubscribeToEvent(EventType::EDITOR_OBJECT_REMOVED);
}

CSoundScene::~CSoundScene()
{
	SAFE_FREE(_3DSources);
}

void CSoundScene::Init()
{
	SetDefaultScore(SoundManager::GetCSManager()->GetScore("simple"));
	auto obj = CreateSource();
	SetOutputModel("global-output");
}

void CSoundScene::TriggerCSoundRebuild()
{
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
		SS->SaveToFile();
	}

	// Update player in scene
	for (auto S3D : _3DSources->GetEntries()) {
		S3D->ReloadScore();
	}
}

void CSoundScene::UpdateScores()
{
	for (auto SS : sceneScores.GetEntities()) {
		SS->Update();
		SS->SaveToFile();
	}

	// Update player in scene
	for (auto S3D : _3DSources->GetEntries()) {
		S3D->ReloadScore();
	}
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

void CSoundScene::SetCSoundControl(const char * channel, float value) const
{
	if (!playback) return;
	for (auto S3D : _3DSources->GetEntries()) {
		S3D->SetControlChannel(channel, value);
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
	TriggerCSoundRebuild();
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
	xml_node objects = nodeScene.append_child("objects");

	for (auto S3D : _3DSources->GetEntries())
	{
		auto sourceNode = objects.append_child("object");

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

	// Save Camera Information
	{
		xml_node cameraInfo = nodeScene.append_child("camera");

		auto transformNode = cameraInfo.append_child("transform");

		auto camera = Manager::GetScene()->GetActiveCamera();

		auto pos = glm::Serialize<glm::vec3>(camera->transform->GetWorldPosition());
		auto rot = glm::Serialize<glm::vec3>(camera->transform->GetRotationEuler360());

		CreateNode("position", pos.c_str(), transformNode);
		CreateNode("rotation", rot.c_str(), transformNode);

		auto projectionInfo = cameraInfo.append_child("projection");
		auto PI = camera->GetProjectionInfo();
		if (PI.isPerspective) {
			CreateNode("fov", PI.FoV, projectionInfo);
			CreateNode("aspect-ratio", PI.aspectRatio, projectionInfo);
		}
		else {
			CreateNode("width", PI.width, projectionInfo);
			CreateNode("height", PI.height, projectionInfo);
		}
		CreateNode("zNear", PI.zNear, projectionInfo);
		CreateNode("zFar", PI.zFar, projectionInfo);
		CreateNode("persepective", PI.isPerspective, projectionInfo);
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
	xml_node objects = nodeScene.child("objects");

	for (auto nodeObj : objects.children())
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

	// Read Camera Info
	{
		xml_node cameraInfo = nodeScene.child("camera");

		auto camera = Manager::GetScene()->GetActiveCamera();

		auto transformNode = cameraInfo.child("transform");
		Serialization::ReadTransform(transformNode, *camera->transform);

		auto PI = camera->GetProjectionInfo();

		auto projectionNode = cameraInfo.child("projection");
		PI.isPerspective = projectionNode.child("persepective").text().as_bool();
		if (PI.isPerspective) {
			PI.FoV = projectionNode.child("fov").text().as_float();
			PI.aspectRatio = projectionNode.child("aspect-ratio").text().as_float();
		}
		else {
			PI.width = projectionNode.child("width").text().as_float();
			PI.height = projectionNode.child("height").text().as_float();
		}
		PI.zFar = projectionNode.child("zFar").text().as_float();
		PI.zNear = projectionNode.child("zNear").text().as_float();

		camera->SetProjection(PI);
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

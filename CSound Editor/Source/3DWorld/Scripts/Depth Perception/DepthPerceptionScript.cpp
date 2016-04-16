#include <pch.h>
#include "DepthPerceptionScript.h"

#include <GPU/SSBO.h>

#include <include/utils.h>
#include <include/math.h>
#include <include/glm_utils.h>

#include <3DWorld/Game.h>
#include <3DWorld/CSound/CSound3DSource.h>
#include <3DWorld/CSound/CSoundScene.h>
#include <3DWorld/Scripts/SoundModelsConfig.h>

#include <CSoundEditor.h>
#include <Csound/SoundManager.h>
#include <Csound/CSoundManager.h>
#include <Csound/CSoundSynthesizer.h>
#include <Csound/CSoundInstrument.h>


DepthPerceptionScript::DepthPerceptionScript()
{
	isLoaded = false;
	SubscribeToEvent("Start-Depth-Perception");
	SubscribeToEvent("Stop-Depth-Perception");
}

void DepthPerceptionScript::Start()
{
	Init();
	if (shaderErrors) return;
	for (auto obj : objects) {
		obj->PlayScore();
	}
	SubscribeToEvent(EventType::FRAME_AFTER_RENDERING);
}

void DepthPerceptionScript::Stop()
{
	if (shaderErrors) return;
	for (auto obj : objects) {
		obj->StopScore();
	}
	UnsubscribeFrom(EventType::FRAME_AFTER_RENDERING);
}

void DepthPerceptionScript::OnUpdate(function<void(const vector<CSound3DSource*>&)> onUpdate)
{
	callbacks.push_back(onUpdate);
}

void DepthPerceptionScript::Init()
{
	if (isLoaded) return;
	isLoaded = true;

	gridSize = 5;
	gameFBO = CSoundEditor::GetGame()->FBO;;

	depthRangeShader = Manager::GetShader()->GetShader("DepthGrid");
	depthClearShader = Manager::GetShader()->GetShader("DepthGridClear");
	depthPerceptionShader = Manager::GetShader()->GetShader("DepthPerception");

	shaderErrors = true;
	if (depthRangeShader && depthClearShader && depthPerceptionShader)
	{
		shaderErrors = depthRangeShader->HasCompileErrors() && depthClearShader->HasCompileErrors() && depthPerceptionShader->HasCompileErrors();

		depthRangeShader->OnLoad([this]() {
			shaderErrors |= depthRangeShader->HasCompileErrors();
			loc_grid_size_DepthRange = depthRangeShader->GetUniformLocation("grid_size");
		});

		depthPerceptionShader->OnLoad([this]() {
			shaderErrors |= depthPerceptionShader->HasCompileErrors();
			loc_grid_size_DepthPerception = depthRangeShader->GetUniformLocation("grid_size");
		});
	}
	depthRangeShader->Reload();
	depthPerceptionShader->Reload();

	auto score = SoundManager::GetCSManager()->GetScore("simple");
	for (auto I : score->GetEntries()) {
		I->Update();
	}
	score->Update();
	score->SaveToFile();

	auto mesh = Manager::GetResource()->GetMesh("sphere");
	auto obj = new CSound3DSource();
	obj->SetMesh(mesh);
	obj->transform->SetScale(glm::vec3(0.1f));
	obj->SetSoundModel(score, false);

	uint size = gridSize * gridSize;
	for (uint i = 0; i < size; i++) {
		objects.push_back(new CSound3DSource(*obj));
	}
	ssbo = new SSBO<glm::ivec4>(size);
	depthRange = new SSBO<glm::ivec2>(size);
}

void DepthPerceptionScript::Update()
{
	auto camera = Manager::GetScene()->GetActiveCamera();
	auto rezolution = gameFBO->GetResolution();

	// Custom Clear for Buffers
	depthClearShader->Use();
	depthRange->BindBuffer(0);
	ssbo->BindBuffer(1);
	OpenGL::DispatchCompute(ssbo->GetSize(), 1, 1, 32);

	// Find Cell Depth Range
	depthRangeShader->Use();
	depthRange->BindBuffer(0);
	gameFBO->SendResolution(depthRangeShader);
	camera->BindPosition(depthPerceptionShader->loc_eye_pos);
	CheckOpenGLError();
	glUniform2i(loc_grid_size_DepthRange, gridSize, gridSize);
	CheckOpenGLError();
	glBindImageTexture(0, gameFBO->GetTextureID(1), 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
	OpenGL::DispatchCompute(rezolution.x, rezolution.y, 1, 32);

	depthRange->ReadBuffer();
	auto vals = depthRange->GetBuffer();

	// Get Positions
	depthPerceptionShader->Use();
	depthRange->BindBuffer(0);
	ssbo->BindBuffer(1);
	gameFBO->SendResolution(depthPerceptionShader);
	camera->BindPosition(depthPerceptionShader->loc_eye_pos);
	glUniform2i(loc_grid_size_DepthPerception, gridSize, gridSize);
	glBindImageTexture(0, gameFBO->GetTextureID(1), 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
	OpenGL::DispatchCompute(rezolution.x, rezolution.y, 1, 32);
	ssbo->ReadBuffer();

	// Compute Virtual 3D position - average of 3D points intersected by the sphere/plane for an object
	auto size = ssbo->GetSize();
	auto values = ssbo->GetBuffer();

	int nrActiveSources = 1;
	for (uint i = 0; i < size; i++) {
		glm::vec4 pos(0);
		objects[i]->SetVolume(0);

		if (values[i].w) {
			pos = glm::vec4(values[i]) / (values[i].w * 1000.0f);
			objects[i]->SetVolume(100);
			nrActiveSources++;
		}

		float distance = glm::distance(camera->transform->GetWorldPosition(), glm::vec3(pos));
		objects[i]->transform->SetScale(glm::vec3(0.02f * distance));
		objects[i]->transform->SetWorldPosition(glm::vec3(pos));
	}

	for (auto &obj: objects) {
		if (obj->GetSoundVolume() > 0)
			obj->SetVolume( uint(config->soundGain * 100.0f) / (nrActiveSources + 1));
		obj->Update();
	}

	// ------------------------------------------------------------------------
	// Get Objects and asign virtual camera space center position
	for (auto callback : callbacks) {
		callback(objects);
	}

	RenderPoints();
}

void DepthPerceptionScript::RenderPoints()
{
	auto camera = Manager::GetScene()->GetActiveCamera();

	Shader *shader = Manager::GetShader()->GetShader("simple");
	shader->Use();
	camera->BindPosition(shader->loc_eye_pos);
	camera->BindViewMatrix(shader->loc_view_matrix);
	camera->BindProjectionMatrix(shader->loc_projection_matrix);
	glUniform4f(shader->loc_debug_color, 1, 1, 0, 0);

	glDepthMask(GL_FALSE);
	glDisable(GL_DEPTH_TEST);

	gameFBO->Bind(false);

	for (auto &obj : objects) {
		if (obj->transform->GetWorldPosition() != glm::vec3(0))
			obj->Render(shader);
	}

	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);
}

void DepthPerceptionScript::OnEvent(EventType eventID, void * data)
{
	if (eventID == EventType::FRAME_AFTER_RENDERING)
		Update();
}

void DepthPerceptionScript::OnEvent(const string & eventID, void * data)
{
	if (eventID.compare("Start-Depth-Perception") == 0) {
		config = (DepthPerceptionConfig*)data;
		Start();
	}
	if (eventID.compare("Stop-Depth-Perception") == 0) {
		Stop();
	}
}
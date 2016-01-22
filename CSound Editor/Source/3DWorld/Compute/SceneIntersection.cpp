#include "SceneIntersection.h"

#include <include/utils.h>
#include <include/math.h>
#include <include/glm_utils.h>

#include <3DWorld/Game.h>
#include <3DWorld/CSound/CSound3DSource.h>
#include <3DWorld/CSound/CSoundScene.h>
#include <CSoundEditor.h>

#include <Core/Engine.h>
#include <Core/GameObject.h>
#include <Core/Camera/Camera.h>
#include <Core/WindowObject.h>
#include <Component/Transform/Transform.h>

#include <Manager/Manager.h>
#include <Manager/SceneManager.h>
#include <Manager/ShaderManager.h>
#include <Manager/ColorManager.h>
#include <Debugging/TextureDebugger.h>
#include <UI/ColorPicking/ColorPicking.h>

#include <GPU/Shader.h>
#include <GPU/SSBO.h>
#include <GPU/Texture.h>
#include <GPU/FrameBuffer.h>

#include <Utils/OpenGL.h>

SceneIntersection::SceneIntersection()
{
	gameFBO = CSoundEditor::GetGame()->FBO;;

	computeShader = Manager::GetShader()->GetShader("SceneIntersection");
	if (!computeShader) {
		return;
	}

	sphere_size = computeShader->GetUniformLocation("sphere_size");
	plane_direction = computeShader->GetUniformLocation("plane_direction");

	computeShader->OnLoad([this]()
	{
		sphere_size = computeShader->GetUniformLocation("sphere_size");
		plane_direction = computeShader->GetUniformLocation("plane_direction");
		auto offset_loc = computeShader->GetUniformLocation("colorID_step");
		auto ecodeSize = Manager::GetColor()->GetChannelsEncodeSize();
		glUniform3iv(offset_loc, 1, glm::value_ptr(ecodeSize));
	});
	computeShader->Reload();

	sphereRadius = 0;
	auto rezolution = gameFBO->GetResolution();

	visualization = new Texture();
	visualization->Create2DTexture((unsigned char*)nullptr, rezolution.x, rezolution.y, 1);

	Manager::GetTextureDebugger()->SetChannelIndex(3, 1, visualization);

	ssbo = new SSBO<glm::ivec4>(2048);

}

void SceneIntersection::Start()
{
	if (!computeShader)
		return;
	SubscribeToEvent(EventType::FRAME_AFTER_RENDERING);
}

void SceneIntersection::Stop()
{
	if (!computeShader)
		return;
	SetSphereSize(0.05f);
	UnsubscribeFrom(EventType::FRAME_AFTER_RENDERING);
}

void SceneIntersection::SetSphereSize(float sphereRadius)
{
	this->sphereRadius = max(sphereRadius, 0.05f);
}

void SceneIntersection::OnUpdate(function<void(const vector<CSound3DSource*>&)> onUpdate)
{
	callbacks.push_back(onUpdate);
}

void SceneIntersection::Update()
{
	ssbo->ClearBuffer();

	computeShader->Use();

	auto rezolution = gameFBO->GetResolution();
	auto camera = Manager::GetScene()->GetActiveCamera();

	auto cameraPos = camera->transform->GetWorldPosition();
	auto planePos = cameraPos - camera->transform->GetLocalOZVector() * 3.0f;
	auto direction = -camera->transform->GetLocalOZVector();
	glUniform3f(plane_direction, direction.x, direction.y, direction.z);
	glUniform1f(sphere_size, sphereRadius);
	gameFBO->SendResolution(computeShader);
	camera->BindPosition(computeShader->loc_eye_pos);

	ssbo->BindBuffer(0);
	glBindImageTexture(0, visualization->GetTextureID(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R8UI);
	glBindImageTexture(1, gameFBO->GetTextureID(3), 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
	glBindImageTexture(2, Manager::GetPicker()->FBO->GetTextureID(0), 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
	glBindImageTexture(3, gameFBO->GetTextureID(0), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
	OpenGL::DispatchCompute(rezolution.x, rezolution.y, 1, 32);
	ssbo->ReadBuffer();

	// Compute Virtual 3D position - average of 3D points intersected by the sphere/plane for an object
	auto size = ssbo->GetSize();
	auto values = ssbo->GetBuffer();

	// Used for computing world position from view position
	//auto invView = glm::inverse(camera->GetViewMatrix());
	centerPoints.clear();
	objects.clear();

	for (uint i = 0; i < size; i++) {
		if (values[i].w) {
			glm::vec4 averageViewSpacePos = glm::vec4(values[i]) / (float(1000 * values[i].w));

			// Compute world position from view position
			// the 4th component needs to be set to 1 otherwise the mutiplication will yield a wrong answer
			//averageViewSpacePos.w = 1;
			//glm::vec4 worldPos = invView * averageViewSpacePos;
			//worldPos.w = float(i);

			averageViewSpacePos.w = float(i);
			centerPoints.push_back(averageViewSpacePos);
		}
	}

	// ------------------------------------------------------------------------
	// Get Objects and asign virtual camera space center position

	if (centerPoints.size()) {
		objects.reserve(centerPoints.size());

		for (auto const &pos : centerPoints)
		{
			auto obj = Manager::GetColor()->GetObjectByID((unsigned int)pos.w);
			auto source = dynamic_cast<CSound3DSource*>(obj);
			if (source)
			{
				source->SetVirtualCameraSpacePosition(glm::vec3(pos));
				objects.push_back(source);
			}
		}
	}

	for (auto callback : callbacks) {
		callback(objects);
	}
}

void SceneIntersection::OnEvent(EventType eventID, void * data)
{
	if (eventID == EventType::FRAME_AFTER_RENDERING)
		Update();
}

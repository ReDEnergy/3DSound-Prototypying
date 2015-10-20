#include "PlaneIntersection.h"

#include <include/utils.h>
#include <include/math.h>

#include <3DWorld/Game.h>

#include <Core/Engine.h>
#include <Core/Camera/Camera.h>
#include <Core/WindowObject.h>
#include <Component/Transform/Transform.h>

#include <Manager/Manager.h>
#include <Manager/SceneManager.h>
#include <Manager/ShaderManager.h>
#include <Debugging/TextureDebugger.h>
#include <UI/ColorPicking/ColorPicking.h>

#include <GPU/Shader.h>
#include <GPU/SSBO.h>
#include <GPU/Texture.h>
#include <GPU/FrameBuffer.h>

#include <CSoundEditor.h>

PlaneIntersection::PlaneIntersection()
{
	computeShader = Manager::GetShader()->GetShader("PlaneIntersection");
	plane_origin = computeShader->GetUniformLocation("plane_origin");
	plane_direction = computeShader->GetUniformLocation("plane_direction");

	gameFBO = CSoundEditor::GetGame()->FBO;

	auto rezolution = gameFBO->GetResolution();

	visualization = new Texture();
	visualization->Create2DTexture((unsigned char*)nullptr, rezolution.x, rezolution.y, 1);

	Manager::GetTextureDebugger()->SetChannelIndex(3, 0, visualization);

	ssbo = new SSBO<glm::vec3>(rezolution.x * rezolution.y);

	SubscribeToEvent(EventType::FRAME_AFTER_RENDERING);
}

void PlaneIntersection::SetPlaneTransform(const Transform * const planeTransform)
{
	this->planeTransform = planeTransform;
}

void PlaneIntersection::Update()
{
	int WORK_GROUP_SIZE = 32;
	computeShader->Use();

	auto rezolution = gameFBO->GetResolution();
	auto camera = Manager::GetScene()->GetActiveCamera();

	auto pos = planeTransform->GetWorldPosition();
	auto direction = planeTransform->GetLocalOZVector();
	pos = camera->transform->GetWorldPosition();
	direction = camera->transform->GetLocalOZVector();
	glUniform3f(plane_direction, direction.x, direction.y, direction.z);
	glUniform3f(plane_origin, pos.x, pos.y, pos.z);
	gameFBO->SendResolution(computeShader);

	ssbo->BindBuffer(0);
	//glBindImageTexture(0, gameFBO->GetTextureID(0), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
	glBindImageTexture(0, visualization->GetTextureID(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R8UI);
	glBindImageTexture(1, gameFBO->GetTextureID(1), 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
	glDispatchCompute(GLuint(UPPER_BOUND(rezolution.x, WORK_GROUP_SIZE)), GLuint(UPPER_BOUND(rezolution.y, WORK_GROUP_SIZE)), 1);
	glFinish();
	glMemoryBarrier(GL_ALL_BARRIER_BITS);

	ssbo->ReadBuffer();
	auto x = ssbo->GetBuffer();
}

void PlaneIntersection::OnEvent(EventType eventID, void * data)
{
	if (eventID == EventType::FRAME_AFTER_RENDERING)
		Update();
}

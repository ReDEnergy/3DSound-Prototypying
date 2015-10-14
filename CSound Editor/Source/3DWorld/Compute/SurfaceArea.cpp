#include "SurfaceArea.h"

#include <include/utils.h>
#include <include/math.h>

#include <Core/Engine.h>
#include <Core/Camera/Camera.h>
#include <Core/WindowObject.h>
#include <Component/Transform/Transform.h>

#include <Manager/Manager.h>
#include <Manager/ShaderManager.h>
#include <UI/ColorPicking/ColorPicking.h>

#include <GPU/Shader.h>
#include <GPU/SSBO.h>
#include <GPU/Texture.h>
#include <GPU/FrameBuffer.h>

SurfaceArea::SurfaceArea()
{
	ssbo = new SSBO<unsigned int>(2048);
}

void SurfaceArea::Update(const Camera* const camera)
{
	if (camera->transform->GetMotionState() == false) return;

	Shader *S = Manager::GetShader()->GetShader("ColorSurface");
	S->Use();

	ssbo->ClearBuffer();
	ssbo->BindBuffer(0);
	Manager::GetPicker()->FBO->BindTexture(0, GL_TEXTURE0);
	int WORK_GROUP_SIZE = 16;
	glDispatchCompute(GLuint(UPPER_BOUND(Engine::Window->resolution.x, WORK_GROUP_SIZE)), GLuint(UPPER_BOUND(Engine::Window->resolution.y, WORK_GROUP_SIZE)), 1);
	glFinish();
	glMemoryBarrier(GL_ALL_BARRIER_BITS);

	ReadData();
}

unsigned int SurfaceArea::GetValue(unsigned int ID) const
{
	return counter[ID];
}

void SurfaceArea::ReadData()
{
	ssbo->ReadBuffer();
	counter = ssbo->GetBuffer();
}
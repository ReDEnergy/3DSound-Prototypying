#include "SurfaceArea.h"

#include <include/utils.h>
#include <include/math.h>

#include <Core/Engine.h>
#include <Core/Camera/Camera.h>
#include <Core/WindowObject.h>
#include <Component/Transform/Transform.h>

#include <Manager/Manager.h>
#include <Manager/ShaderManager.h>
#include <Manager/ColorManager.h>
#include <UI/ColorPicking/ColorPicking.h>

#include <GPU/Shader.h>
#include <GPU/SSBO.h>
#include <GPU/Texture.h>
#include <GPU/FrameBuffer.h>

#include <3DWorld/Game.h>
#include <3DWorld/CSound/CSoundScene.h>
#include <3DWorld/CSound/CSound3DSource.h>
#include <CSoundEditor.h>

SurfaceArea::SurfaceArea()
{
	ssbo = new SSBO<unsigned int>(2048);
	counter = ssbo->GetBuffer();

	gameCamera = CSoundEditor::GetGame()->gameCamera;

	computeShader = Manager::GetShader()->GetShader("ColorSurface");
	if (computeShader) {
		SubscribeToEvent(EventType::FRAME_UPDATE);
	}
}

void SurfaceArea::Update()
{
	if (gameCamera->transform->GetMotionState() == false) return;

	computeShader->Use();
	int WORK_GROUP_SIZE = 32;

	ssbo->ClearBuffer();
	ssbo->BindBuffer(0);
	glBindImageTexture(0, Manager::GetPicker()->FBO->GetTextureID(0), 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
	glDispatchCompute(GLuint(UPPER_BOUND(Engine::Window->resolution.x, WORK_GROUP_SIZE)), GLuint(UPPER_BOUND(Engine::Window->resolution.y, WORK_GROUP_SIZE)), 1);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);
	glFinish();

	ReadData();


	// ------------------------------------------------------------------------
	// Update Scene Information
	for (auto S3D : CSoundEditor::GetScene()->GetEntries())
	{
		auto ID = Manager::GetColor()->GetUKeyFromColor(S3D->GetColorID());
		if (counter[ID])
			S3D->SetSurfaceArea(counter[ID]);
	}
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

void SurfaceArea::OnEvent(EventType Event, void * data)
{
	if (Event == EventType::FRAME_UPDATE)
		Update();
}

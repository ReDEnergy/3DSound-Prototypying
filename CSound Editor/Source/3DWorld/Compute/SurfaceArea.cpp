#include "SurfaceArea.h"

#include <iostream>

#include <include/utils.h>
#include <include/math.h>
#include <include/glm_utils.h>

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

#include <Utils/OpenGL.h>

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

	if (!computeShader) {
		return;
	}

	computeShader->OnLoad([this]() {
		auto offset_loc = computeShader->GetUniformLocation("colorID_step");
		auto ecodeSize = Manager::GetColor()->GetChannelsEncodeSize();
		glUniform3iv(offset_loc, 1, glm::value_ptr(ecodeSize));
	});
	computeShader->Reload();

	SubscribeToEvent(EventType::FRAME_UPDATE);
}

void SurfaceArea::Update()
{
	bool motion = false;
	motion |= gameCamera->transform->GetMotionState();
	if (motion == false) {
		auto selectedObject = Manager::GetPicker()->GetSelectedObject();
		if (selectedObject) {
			motion |= selectedObject->transform->GetMotionState();
		}
		if (!motion) return;
	}

	ssbo->ClearBuffer();

	computeShader->Use();
	ssbo->BindBuffer(0);
	glBindImageTexture(0, Manager::GetPicker()->FBO->GetTextureID(0), 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
	OpenGL::DispatchCompute(Engine::Window->resolution.x, Engine::Window->resolution.y, 1, 32);

	ReadData();


	// ------------------------------------------------------------------------
	// Update Scene Information
	for (auto S3D : CSoundEditor::GetScene()->GetEntries())
	{
		auto ID = Manager::GetColor()->GetUKeyFromColor(S3D->GetColorID());
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

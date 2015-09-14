#include "GameWindow.h"

#include <Editor/Windows/OpenGL/OpenGLWindow.h>
#include <Editor/GUI.h>
#include <3DWorld/Game.h>
#include <3DWorld/CSound/CSoundScene.h>

#include <CSoundEditor.h>

#include <QOpenGLContext>
#include <QWGLNativeContext>
#include <QLayout>

// Engine library
#include <Engine.h>


static GPUBuffers *buffer;
static Transform debugTransform;

void RenderMesh(GLuint VAO)
{
	glBindVertexArray(VAO);
	glDrawElementsBaseVertex(4, 6, GL_UNSIGNED_SHORT, 0, 0);
	glBindVertexArray(0);
	CheckOpenGLError();
}

GameWindow::GameWindow()
{
	setWindowTitle("3D Scene");
	useContext = false;

	InitUI();
	GUI::Set(QT_INSTACE::_3D_SCENE_WINDOW, (void*) this);

	debugTransform.SetScale(glm::vec3(0.8));
	debugTransform.SetWorldPosition(glm::vec3(0));
}

void GameWindow::InitUI()
{
	qtOpenGLWindow = new OpenGLWindow();
	qtOpenGLWindow->Init();
	qtLayout->addWidget(qtOpenGLWindow->container);
}

void GameWindow::DockedEvent(bool state)
{
	if (state == true) {
		setFixedSize(Engine::Window->resolution.x, Engine::Window->resolution.y);
	}
	else {
		setMinimumHeight(Engine::Window->resolution.y + 22);
	}
}

void GameWindow::Init()
{
	game = CSoundEditor::GetGame();
	InitRendering();
}

void GameWindow::Update()
{
	CSoundEditor::GetScene()->Update();
}

void GameWindow::Render()
{
	auto value = qtOpenGLWindow->SetAsCurrentContext();
	auto activeCamera = Manager::GetScene()->GetActiveCamera();

	// No need to clear because we write a texture to the entire screen
	//glClearColor(1, 1, 1, 1);
	FrameBuffer::Unbind();
	glDepthMask(GL_FALSE);
	glDisable(GL_DEPTH_TEST);

	{
		Shader *Composition = Manager::GetShader()->GetShader("composition");
		Composition->Use();
		glUniform2f(Composition->loc_resolution, (float)Engine::Window->resolution.x, (float)Engine::Window->resolution.y);
		glUniform1i(Composition->active_ssao, false);
		glUniform1i(Composition->active_selection, Manager::GetPicker()->HasActiveSelection());
		glUniform1i(Composition->loc_debug_view, Manager::GetDebug()->GetActiveState());
		activeCamera->BindProjectionDistances(Composition);

		game->FBO->BindTexture(0, GL_TEXTURE0);
		game->FBO_Light->BindTexture(0, GL_TEXTURE1);
		game->ShadowMap->Bind(GL_TEXTURE2);
		game->FBO->BindDepthTexture(GL_TEXTURE4);
		Manager::GetDebug()->FBO->BindTexture(0, GL_TEXTURE5);
		Manager::GetDebug()->FBO->BindDepthTexture(GL_TEXTURE6);
		Manager::GetPicker()->FBO_Gizmo->BindTexture(0, GL_TEXTURE7);

		RenderMesh(buffer->VAO);
	}

	// --- Debug View --- //
	if (Manager::GetRenderSys()->Is(RenderState::DEBUG))
	{
		Shader *Debug = Manager::GetShader()->GetShader("debug");
		Debug->Use();
		glUniform1i(Debug->loc_debug_id, Manager::GetRenderSys()->debugParam);
		activeCamera->BindProjectionDistances(Debug);

		game->FBO->BindAllTextures();
		game->FBO_Light->BindTexture(0, GL_TEXTURE5);
		game->FBO->BindDepthTexture(GL_TEXTURE6);
		Manager::GetPicker()->FBO_Gizmo->BindTexture(0, GL_TEXTURE7);
		Manager::GetPicker()->FBO->BindTexture(0, GL_TEXTURE8);
		game->Sun->FBO->BindTexture(0, GL_TEXTURE10);
		game->sboArea->colorAreaTexture->Bind(GL_TEXTURE11);

		glUniformMatrix4fv(Debug->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(debugTransform.GetModel()));
		RenderMesh(buffer->VAO);
	}

	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);

	qtOpenGLWindow->EndFrame();
	CheckOpenGLError();
}

void GameWindow::InitRendering()
{
	auto value = qtOpenGLWindow->SetAsCurrentContext();
	auto mesh = game->ScreenQuad->mesh;
	buffer = UtilsGPU::UploadData(mesh->positions, mesh->normals, mesh->texCoords, mesh->indices);

	auto nativeHandle = qtOpenGLWindow->GetContext()->nativeHandle();
	auto windowsNative = nativeHandle.value<QWGLNativeContext>();

	auto sctx = qtOpenGLWindow->GetContext()->shareContext();

	useContext = true;
}
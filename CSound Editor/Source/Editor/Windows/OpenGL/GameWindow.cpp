#include <pch.h>
#include "GameWindow.h"

#include <include/gl.h>
#include <Utils/GPU.h>

#include <Editor/Windows/OpenGL/OpenGLWindow.h>
#include <Editor/GUI.h>
#include <3DWorld/Game.h>
#include <3DWorld/CSound/CSoundScene.h>

#include <CSoundEditor.h>

#include <QOpenGLContext>
#include <QWGLNativeContext>
#include <QLayout>

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
	qtOpenGLWindow->Init(WindowManager::GetDefaultWindow());
	qtLayout->addWidget(qtOpenGLWindow->container);
}

void GameWindow::DockedEvent(bool state)
{
	auto res = WindowManager::GetDefaultWindow()->GetResolution();
	if (state == true) {
		setFixedSize(res.x, res.y);
	}
	else {
		setMinimumHeight(res.y + 22);
	}
}

void GameWindow::Init()
{
	game = CSoundEditor::GetGame();
	InitRendering();
	Manager::GetTextureDebugger()->SetRenderingVAO(buffer->VAO);
}

void GameWindow::Update()
{
}

void GameWindow::Render()
{
	auto value = qtOpenGLWindow->SetAsCurrentContext();
	auto activeCamera = Manager::GetScene()->GetActiveCamera();

	// No need to clear because we write a texture to the entire screen
	auto window = WindowManager::GetDefaultWindow();

	FrameBuffer::Unbind(window);
	FrameBuffer::Clear();

	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);

	{
		Shader *Composition = Manager::GetShader()->GetShader("composition");
		Composition->Use();

		glUniform2f(Composition->loc_resolution, (float)window->props.resolution.x, (float)window->props.resolution.y);
		glUniform1i(Composition->active_selection, Manager::GetPicker()->HasActiveSelection());
		glUniform1i(Composition->loc_debug_view, Manager::GetDebug()->GetActiveState());
		activeCamera->BindProjectionDistances(Composition);

		game->FBO->BindTexture(0, GL_TEXTURE0);
		game->FBO->BindDepthTexture(GL_TEXTURE4);
		Manager::GetDebug()->FBO->BindTexture(0, GL_TEXTURE5);
		Manager::GetDebug()->FBO->BindDepthTexture(GL_TEXTURE6);
		Manager::GetPicker()->FBO_Gizmo->BindTexture(0, GL_TEXTURE7);

		RenderMesh(buffer->VAO);
	}

	Manager::GetTextureDebugger()->Render();

	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);

	qtOpenGLWindow->EndFrame();
	CheckOpenGLError();
}

// TODO - now the engine supports recreating a VAO for a different thread

void GameWindow::InitRendering()
{
	auto value = qtOpenGLWindow->SetAsCurrentContext();
	auto mesh = Manager::GetResource()->GetMesh("screen-quad");
	buffer = new GPUBuffers();
	*buffer = UtilsGPU::UploadData(mesh->positions, mesh->normals, mesh->texCoords, mesh->indices);

	auto nativeHandle = qtOpenGLWindow->GetContext()->nativeHandle();
	auto windowsNative = nativeHandle.value<QWGLNativeContext>();

	auto sctx = qtOpenGLWindow->GetContext()->shareContext();

	useContext = true;
}	
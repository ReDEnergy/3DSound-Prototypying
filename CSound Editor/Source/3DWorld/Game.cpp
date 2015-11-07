#include "Game.h"

#include <3DWorld/Input/GameInput.h>

#include <3DWorld/Compute/SurfaceArea.h>

#include <Editor/Windows/OpenGL/GameWindow.h>
#include <Editor/GUI.h>
#include <Editor/EditorMainWindow.h>

#include <include/Engine.h>
#include <templates/singleton.h>

#include <3DWorld/Scripts/MovingPlaneScript.h>
#include <3DWorld/Scripts/ExpandingSphereScript.h>

Game::Game() {
}

Game::~Game() {
}

void Game::Init()
{
	// Game resolution
	glm::ivec2 resolution = Engine::Window->resolution;
	float aspectRation = float(resolution.x) / resolution.y;

	// Cameras
	gameCamera = new Camera();
	gameCamera->SetPerspective(40, aspectRation, 0.1f, 250);
	gameCamera->SetPosition(glm::vec3(0, 3, -7));
	gameCamera->SplitFrustum(5);
	gameCamera->transform->SetMoveSpeed(1.4f);

	freeCamera = new Camera();
	freeCamera->SetPerspective(40, aspectRation, 0.1f, 250);
	freeCamera->SetPosition(glm::vec3(0.0f, 10.0f, 10.0f));
	freeCamera->Update();

	Manager::GetScene()->SetActiveCamera(gameCamera);

	// Lights
	Sun = new DirectionalLight();
	Sun->SetPosition(glm::vec3(0.0f, 50.0f, 0.0f));
	Sun->RotateOX(-460);
	Sun->RotateOY(360);
	Sun->SetOrthgraphic(40, 40, 0.1f, 200);
	Sun->Update();

	// --- Create FBO for rendering to texture --- //
	FBO = new FrameBuffer();
	FBO->Generate(resolution.x, resolution.y, 5);

	// Texture Debugger
	auto TXDB = Manager::GetTextureDebugger();
	TXDB->SetChannel(0, FBO);

	// Listens to Events and Input
	SubscribeToEvent(EventType::SWITCH_CAMERA);

	cameraInput = new CameraInput(Manager::GetScene()->GetActiveCamera());
	cameraDebugInput = new CameraDebugInput(gameCamera);
	ObjectInput *DI = new DebugInput();
	ObjectInput *EI = new EditorInput();
	ObjectInput *GI = new GameInput(this);
	InputRules::PushRule(InputRule::R_GAMEPLAY);

	InitSceneCameras();

	auto ground = Manager::GetScene()->GetGameObject("ground", 1);
	if (ground) ground->SetSelectable(false);
	Manager::GetScene()->Update();

	new MovingPlaneScript();
	new ExpandingSphereScript();
	new SurfaceArea();

	wglSwapIntervalEXT(1);
}

void Game::FrameStart()
{
	Engine::Window->SetContext();
	Manager::GetEvent()->EmitSync(EventType::FRAME_START);
}

void Game::Update(float elapsedTime, float deltaTime)
{
	Camera *activeCamera = Manager::GetScene()->GetActiveCamera();

	///////////////////////////////////////////////////////////////////////////
	// Update Scene

	InputSystem::UpdateObservers(deltaTime);

	Manager::GetAudio()->Update(activeCamera);
	Manager::GetEvent()->Update();
	Manager::GetScene()->Update();
	Manager::GetScene()->LightSpaceCulling(gameCamera, Sun);
	Manager::GetPicker()->Update(activeCamera);
	Manager::GetPicker()->DrawSceneForPicking();
	Manager::GetDebug()->Update(activeCamera);
	Manager::GetEvent()->EmitSync(EventType::FRAME_UPDATE);

	///////////////////////////////////////////////////////////////////////////
	// Scene Rendering

	if (Manager::GetRenderSys()->Is(RenderState::FORWARD)) {
		FrameBuffer::Unbind();
		FrameBuffer::Clear();
	}
	else {
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		FBO->Bind();
	}

	Manager::GetScene()->Render(activeCamera);
	Manager::GetMenu()->RenderMenu();
	Manager::GetEvent()->EmitSync(EventType::FRAME_AFTER_RENDERING);
}

void Game::FrameEnd()
{
//	Manager::GetTextureDebugger()->Render();
	Singleton<EditorMainWindow>::Instance()->Update();
	Manager::GetScene()->FrameEnded();
	Manager::GetEvent()->EmitSync(EventType::FRAME_END);
}

void Game::OnEvent(EventType Event, void *data)
{
	switch (Event)
	{
	case EventType::SWITCH_CAMERA:

		Manager::GetScene()->GetActiveCamera()->SetDebugView(true);

		activeSceneCamera++;
		if (activeSceneCamera == sceneCameras.size()) {
			activeSceneCamera = 0;
		}

		Manager::GetScene()->SetActiveCamera(sceneCameras[activeSceneCamera]);
		Manager::GetScene()->GetActiveCamera()->SetDebugView(false);
		cameraInput->camera = Manager::GetScene()->GetActiveCamera();

	default:
		break;
	}
}

void Game::InitSceneCameras()
{
	activeSceneCamera = 0;
	sceneCameras.push_back(gameCamera);
	sceneCameras.push_back(freeCamera);
	Manager::GetScene()->GetActiveCamera()->SetDebugView(false);
}
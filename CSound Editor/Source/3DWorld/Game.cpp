#include "Game.h"

#include <3DWorld/Input/GameInput.h>
#include <Editor/Windows/OpenGL/GameWindow.h>
#include <Editor/GUI.h>
#include <Engine.h>

//#ifdef GLEW_ARB_shader_storage_buffer_object
//#undef GLEW_ARB_shader_storage_buffer_object
//#endif

Game::Game() {
}

Game::~Game() {
}

void Game::Init()
{
	sboArea = new SSBOArea();

	// Game resolution
	glm::ivec2 resolution = Engine::Window->resolution;
	float aspectRation = float(resolution.x) / resolution.y;

	// Cameras
	gameCamera = new Camera();
	gameCamera->SetPerspective(40, aspectRation, 0.1f, 150);
	gameCamera->SetPosition(glm::vec3(0, 5, 5));
	gameCamera->SplitFrustum(5);
	//gameCamera->Update();

	freeCamera = new Camera();
	freeCamera->SetOrthgraphic(40, 40, 0.1f, 500);
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

	ShadowMap = new Texture();
	ShadowMap->Create2DTextureFloat(NULL, resolution.x, resolution.y, 4, 32);

	// --- Create FBO for rendering to texture --- //
	FBO = new FrameBuffer();
	FBO->Generate(resolution.x, resolution.y, 5);

	FBO_Light = new FrameBuffer();
	FBO_Light->Generate(resolution.x, resolution.y, 1);

	ScreenQuad = Manager::GetResource()->GetGameObject("render-quad");
	ScreenQuad->Update();

	// --- Debugging --- //
	DebugPanel = Manager::GetResource()->GetGameObject("render-quad");
	DebugPanel->UseShader(Manager::GetShader()->GetShader("debug"));
	DebugPanel->transform->SetScale(glm::vec3(0.5));
	DebugPanel->transform->SetWorldPosition(glm::vec3(0.5));

	// Listens to Events and Input

	SubscribeToEvent(EventType::SWITCH_CAMERA);
	SubscribeToEvent(EventType::CLOSE_MENU);
	SubscribeToEvent(EventType::OPEN_GAME_MENU);

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

	wglSwapIntervalEXT(1);
}

void Game::FrameStart()
{
	Engine::Window->SetContext();
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
	Manager::GetDebug()->Update(activeCamera);
	Manager::GetPicker()->Update(activeCamera);
	Manager::GetPicker()->DrawSceneForPicking();
	Manager::GetScene()->LightSpaceCulling(gameCamera, Sun);

	///////////////////////////////////////////////////////////////////////////
	// Scene Rendering

	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);

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

	///////////////////////////////////////////////////////////////////////
	// Compute ColorID Area 

	#ifdef GLEW_ARB_shader_storage_buffer_object
	{
		// -- COMPUTE SHADER
		int WORK_GROUP_SIZE = 16;

		Shader *S = Manager::GetShader()->GetShader("ColorSurface");
		S->Use();

		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, sboArea->ssbo);
		glClearBufferData(GL_SHADER_STORAGE_BUFFER, GL_R32UI, GL_RED, GL_UNSIGNED_INT, NULL);
		CheckOpenGLError();

		Manager::GetPicker()->FBO->BindTexture(0, GL_TEXTURE0);
		glBindImageTexture(1, sboArea->colorAreaTexture->GetTextureID(), 0, GL_FALSE, 0, GL_READ_WRITE, GL_R16F);
		glDispatchCompute(GLuint(UPPER_BOUND(Engine::Window->resolution.x, WORK_GROUP_SIZE)), GLuint(UPPER_BOUND(Engine::Window->resolution.y, WORK_GROUP_SIZE)), 1);

		glFinish();
		glMemoryBarrier(GL_ALL_BARRIER_BITS);

		sboArea->ReadData();
	}
	#endif

	Manager::GetScene()->FrameEnded();
}

void Game::FrameEnd()
{
	auto SceneWindow = GUI::Get<GameWindow>(QT_INSTACE::_3D_SCENE_WINDOW);
	if (SceneWindow) {
		SceneWindow->Update();
		SceneWindow->Render();
	}
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
	sceneCameras.push_back(Sun);

	Manager::GetScene()->GetActiveCamera()->SetDebugView(false);
}

SSBOArea::SSBOArea()
{
	colorAreaTexture = new Texture();
	colorAreaTexture->Create2DTextureFloat(NULL, 64, 64, 1);

	counter = new uint[2048];
	memset(counter, 0, 4 * 2048);

	#ifdef GLEW_ARB_shader_storage_buffer_object
	{
		glGenBuffers(1, &ssbo);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
		glBufferData(GL_SHADER_STORAGE_BUFFER, 4 * 2048, &counter, GL_DYNAMIC_READ);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
		CheckOpenGLError();
	}
	#endif
}

void SSBOArea::ReadData()
{
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
	GLvoid* p = glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
	memcpy(counter, p, 4 * 2048);
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

	vector<int> view;
	for (auto i = 0; i < 2048; i++) {
		if (counter[i])
			view.push_back(counter[i]);
	}
}

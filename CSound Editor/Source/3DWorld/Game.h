#pragma once
#include <vector>
#include <EngineForward.h>

using namespace std;

#include <Core/World.h>
#include <Event/EventListener.h>

class SSBOArea
{
	public:
		SSBOArea();
		~SSBOArea() {};
		void ReadData();

	public:
		unsigned int *counter;
		Texture *colorAreaTexture;
		unsigned int ssbo;
};

class Game : public World,
			public EventListener
{
	public:
		Game();
		~Game();
		void Init();
		void FrameStart();
		void Update(float elapsedTime, float deltaTime);
		void FrameEnd();

	private:
		void OnEvent(EventType Event, Object *data);
		void OnEvent(const char* eventID, Object *data);
		void InitSceneCameras();

	public:
		Camera				*freeCamera;
		Camera				*gameCamera;
		CameraInput			*cameraInput;
		CameraDebugInput	*cameraDebugInput;

		DirectionalLight	*Sun;
		SpotLight			*Spot;

		FrameBuffer			*FBO;
		FrameBuffer			*FBO_Light;

		GameObject			*ScreenQuad;
		GameObject			*DebugPanel;

		SSAO				*ssao;
		Texture				*ShadowMap;

		ColorPicking		*colorPicking;

		vector<Camera*>		sceneCameras;
		unsigned int		activeSceneCamera;

		SSBOArea *sboArea;
};
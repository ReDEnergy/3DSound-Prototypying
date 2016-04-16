#pragma once
#include <vector>
#include <EngineForward.h>

using namespace std;

#include <Core/World.h>
#include <Event/EventListener.h>
#include <Component/ObjectInput.h>

class SurfaceArea;

class Game :
	public World,
	public EventListener,
	public ObjectInput
{
	public:
		Game();
		~Game();

		void Init();
		void FrameStart();
		void Update(float elapsedTime);
		void FrameEnd();

	private:
		void OnEvent(EventType Event, void *data);
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

		std::vector<Camera*>	sceneCameras;
		unsigned int			activeSceneCamera;
};
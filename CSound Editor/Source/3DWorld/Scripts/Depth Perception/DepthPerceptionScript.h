#pragma once
#include <vector>
#include <functional>
#include <list>

class CSound3DSource;

struct DepthPerceptionConfig;
class Transform;
class Shader;
class FrameBuffer;
class Texture;
template <class T>
class SSBO;

#include <include/glm.h>
#include <Event/EventListener.h>

class DepthPerceptionScript
	: public EventListener
{
	public:
		DepthPerceptionScript();

		void Start();
		void Stop();
		void OnEvent(const string & eventID, void * data);
		void OnUpdate(function<void(const vector<CSound3DSource*>&)> onUpdate);

	private:
		void Init();
		void Update();
		void RenderPoints();
		void OnEvent(EventType eventID, void *data);

	private:

		DepthPerceptionConfig *config;

		SSBO<glm::ivec4> *ssbo;
		SSBO<glm::ivec2> *depthRange;
		vector<glm::vec4> centerPoints;
		vector<CSound3DSource*> objects;

		bool isLoaded;
		int gridSize;
		int loc_grid_size_DepthRange;
		int loc_grid_size_DepthPerception;
		bool shaderErrors;
		Shader *depthPerceptionShader;
		Shader *depthRangeShader;
		Shader *depthClearShader;
		const FrameBuffer *gameFBO;
		list<function<void(const vector<CSound3DSource*>&)>> callbacks;
};
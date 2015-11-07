#pragma once
#include <vector>
#include <functional>
#include <list>

class CSound3DSource;

class Transform;
class Shader;
class FrameBuffer;
class Texture;
template <class T>
class SSBO;

#include <include/glm.h>
#include <Event/EventListener.h>

class SceneIntersection
	: public EventListener
{
	public:
		SceneIntersection();
		~SceneIntersection() {};

		void Start();
		void Stop();
		void SetSphereSize(float sphereRadius);
		void OnUpdate(function<void(const vector<CSound3DSource*>&)> onUpdate);

	private:
		void Update();
		void OnEvent(EventType eventID, void *data);

	private:
		SSBO<glm::ivec4> *ssbo;
		vector<glm::vec4> centerPoints;
		vector<CSound3DSource*> objects;

		const Transform* planeTransform;
		float sphereRadius;

		// Shader uniforms
		unsigned int sphere_size;
		unsigned int plane_direction;
		Shader *computeShader;

		const FrameBuffer *gameFBO;
		Texture* visualization;

		list<function<void(const vector<CSound3DSource*>&)>> callbacks;
};
#pragma once
#include <Editor/Windows/Interface/DockWindow.h>
#include <Editor/Windows/Interface/QtInput.h>
#include <Editor/QT/Utils.h>

#include <include/glm.h>
#include <Event/EventListener.h>


class CSoundComponentProperty;
class CSound3DSource;

#include <QLayout>
#include <QLineEdit>
#include <QLabel>

class SceneObjectProperties
	: public DockWindow
	, public EventListener
{
	public:
		SceneObjectProperties();
		virtual ~SceneObjectProperties() {};

	private:
		void InitUI();
		void Update();
		void ForceUpdate();
		void OnEvent(EventType Event, void *data);

	private:
		QComboBox* meshType;
		GLMVecComponent<glm::vec3> *worldPosition;
		GLMVecComponent<glm::vec3> *cameraSpacePosition;
		GLMVecComponent<glm::vec3> *worldEuler;
		GLMVecComponent<glm::vec3> *worldScale;
		GLMVecComponent<glm::quat> *worldQuat;
		
		SimpleFloatInput *soundVolume;
		SimpleFloatInput *soundIntensity;
		SimpleFloatInput *azimuthInput;
		SimpleFloatInput *elevationInput;
		SimpleFloatInput *distanceToCameraInput;
		SimpleFloatInput *surfaceCoverInput;
		SimpleFloatInput *surfaceAreaInput;


		CSound3DSource *gameObj;

		bool forceUpdate;
};
#pragma once
#include <Editor/Windows/Interface/CustomWidget.h>
#include <Editor/Windows/Interface/QtInput.h>
#include <Editor/QT/Utils.h>

#include <include/glm.h>
#include <Event/EventListener.h>


class CSoundComponentProperty;
class CSound3DSource;
class HrtfRecorder;

#include <QLayout>
#include <QLineEdit>
#include <QLabel>

class HrtfTest
	: public CustomWidget
	, public EventListener
{
	public:
		HrtfTest();
		virtual ~HrtfTest() {};

	private:
		void InitUI();
		void Update();
		void OnEvent(EventType Event, void *data);

	private:
		GLMVecComponent<glm::vec3> *worldPosition;
		GLMVecComponent<glm::vec3> *cameraSpacePosition;
		GLMVecComponent<glm::vec3> *worldEuler;
		GLMVecComponent<glm::vec3> *worldScale;
		GLMVecComponent<glm::quat> *worldQuat;
		SimpleFloatInput *azimuthInput;
		SimpleFloatInput *elevationInput;
		SimpleFloatInput *distanceToCameraInput;
		SimpleFloatInput *surfaceCoverInput;
		SimpleFloatInput *surfaceAreaInput;

		CSound3DSource *gameObj;
		HrtfRecorder *recorder;
		bool forceUpdate;
};
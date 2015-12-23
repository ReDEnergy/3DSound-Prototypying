#pragma once

#include <include/glm.h>
#include <Core/GameObject.h>
#include <Event/EventListener.h>

#include <CSound/CSoundForward.h>

class SimpleTimer;

class CSound3DSource
	: public GameObject
	, public EventListener
{
	public:
		CSound3DSource();
		CSound3DSource(const CSound3DSource& ref);
		~CSound3DSource();

		void SetSoundModel(CSoundScore* soundModel, bool trackScore = true);
		void Update();
		void ComputeControlProperties();

		const char* GetRender() const;
		CSoundScore* GetScore() const;
		int GetSurfaceArea() const;
		float GetSurfaceCover() const;
		float GetDistanceToCamera() const;
		float GetElevationToCamera() const;
		float GetAzimuthToCamera() const;
		float GetAzimuthPanningFactor() const;
		float GetElevationPanningFactor() const;
		float GetSoundIntensity() const;
		unsigned int GetSoundVolume() const;
		const glm::vec3 & GetCameraSpacePosition() const;

		void SetVolume(unsigned int value = 100);
		void SetControlChannel(const char* channel, float value, bool forceUpdate = false) const;
		void UseVirtalPosition(bool value);
		void SetVirtualCameraSpacePosition(glm::vec3 cameraSpacePosition);

		void SetSurfaceArea(unsigned int visibleAreaInPixels);

		void SelectObject();

		bool GetPlaybackState() const;
		bool TooglePlayback();
		void PlayScore();
		void PlayScore(float deltaTime);
		void StopScore();
		void ReloadScore();

	private:
		void Init();
		void OnEvent(const string& eventID, void * data);
		void UpdateControlChannels(bool motion = true) const;
		CSoundScore *soundModel;
		CSoundPlayer *player;
		SimpleTimer *timer;

		bool forceUpdate;
		float distanceToCamera;
		unsigned int surfaceArea;
		float surfaceCover;
		float elevation;
		float azimuthPanningFactor;
		float elevationPanningFactor;
		float azimuth;
		float soundFallOff;
		float soundIntensity;
		unsigned int soundVolume;

		bool forceUpdateSoundParameters;
		bool useVirtualPosition;
		glm::vec3 positionCameraSpace;
		glm::vec3 virtualCameraSpacePos;
};
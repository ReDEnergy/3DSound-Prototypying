#pragma once

#include <Core/GameObject.h>
#include <Event/EventListener.h>

#include <CSound/CSoundForward.h>

class CSound3DSource
	: public GameObject
	, public EventListener
{
	public:
		CSound3DSource();
		~CSound3DSource();

		void SetSoundModel(CSoundScore* soundModel);
		void SetSufaceArea(unsigned int surface);
		void Update();

		const char* GetRender() const;
		CSoundScore* GetScore() const;

		void SelectObject();
		void PlayScore();
		void StopScore();
		void ReloadScore();

	private:
		void OnEvent(string eventID, void * data);
		void UpdateControlChannels();
		CSoundScore *soundModel;
		CSoundPlayer *player;
		unsigned int surfaceArea;
		float surfaceCover;
};
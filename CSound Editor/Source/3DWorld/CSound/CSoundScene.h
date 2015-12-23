#pragma once
#include <unordered_map>
#include <string>

#include <Event/EventListener.h>

#include <CSound/CSoundForward.h>
#include <templates/ReferenceCounter.h>

class CSoundScene
	: public EventListener
{
	public:
		CSoundScene();
		~CSoundScene();

	public:
		void Init();

		// Scene Update
		void UpdateScores();
		void TriggerCSoundRebuild();

		// Save/Load/Clear
		void Clear();
		void AutoSave();
		void LoadScene(const char* fileName);
		bool SaveScene();
		void SaveSceneAs(const char* fileName);

		// Playback audio
		void Play();
		void Stop();
		void SetCSoundControl(const char* channel, float value) const;

		// Scene Management
		void AddSource(CSound3DSource * S3D);
		bool Rename(CSound3DSource* S3D, const char* newName);
		void Remove(CSound3DSource* S3D);
		CSound3DSource *CreateSource();
		const vector<CSound3DSource*>& GetEntries() const;

		// Audio settings
		CSoundScore *GetDefaultScore();
		void SetDefaultScore(CSoundScore *score);
		void SetOutputModel(const char* name);

		// Tracking
		void TrackScore(CSoundScore* score);
		void TrackInstrument(CSoundInstrument * instr);
		void UnTrackScore(CSoundScore* score);

		void UnTrackInstrument(CSoundInstrument * instr);

	private:
		void OnEvent(EventType Event, void *data);

	private:
		EntityStorage<CSound3DSource> *_3DSources;

	public:
		ReferenceCounter<CSoundScore> sceneScores;
		ReferenceCounter<CSoundInstrument> sceneInstruments;

	private:
		bool playback;
		CSoundScore *defaultScore;
		string sceneFile;
};
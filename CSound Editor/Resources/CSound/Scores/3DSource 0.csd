<?xml version="1.0"?>
<CsoundSynthesizer>
	<CsOptions>
+rtaudio=alsa -o dac
-B 64
-b 2048
	</CsOptions>
	<CsInstruments>
ksmps = 32 		
nchnls = 2

instr 1

	kPitch chnget "kPitch"
	kAzimuth chnget "kAzimuth"
	kAttenuation chnget "kAttenuation"
	kTrigger chnget "kTrigger"
	kVolume chnget "kVolume"
	kElevation chnget "kElevation"

	 ain soundin "Audio/noise.wav"
	 aout, krec sndloop ain , kPitch, kTrigger , 0.5, 0
	 aout = aout * kVolume * kAttenuation
	 aleft, aright hrtfmove2 aout , kAzimuth, kElevation , "CSound/hrtf/hrtf-44100-left.dat", "CSound/hrtf/hrtf-44100-right.dat"
	 outs aleft, aright
endin

	</CsInstruments>
	<CsScore>
i1 0 300000
	</CsScore>
</CsoundSynthesizer>

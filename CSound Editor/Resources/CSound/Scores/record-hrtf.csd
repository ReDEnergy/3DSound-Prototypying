<?xml version="1.0"?>
<CsoundSynthesizer>
<CsOptions>
-o dac0
-+rtaudio=Portaudio
-B 64
-b 2048
</CsOptions>
<CsInstruments>
sr = 44100
ksmps = 7
nchnls_i = 1
nchnls = 2

instr 1

	kPitch chnget "kPitch"
	kAzimuth chnget "kAzimuth"
	kAttenuation chnget "kAttenuation"
	kTrigger chnget "kTrigger"
	kVolume chnget "kVolume"
	kElevation chnget "kElevation"

	ain soundin "../../Audio/noise.wav"
	aout, krec sndloop ain , kPitch, kTrigger , 5, 0
	aout = aout * kVolume * kAttenuation
	aleft, aright hrtfmove2 aout , kAzimuth, kElevation , "../hrtf/hrtf-44100-left.dat", "../hrtf/hrtf-44100-right.dat"
	outs aleft, aright
	fout "Audio/1.wav" , 14, aleft, aright
endin

	</CsInstruments>
<CsScore>
i1 0 1000 
	</CsScore>
</CsoundSynthesizer>

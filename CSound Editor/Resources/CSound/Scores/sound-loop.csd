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
	kTrigger chnget "kTrigger"

	ain soundin "../../Audio/noise.wav"
	aout, krec sndloop ain , kPitch, kTrigger , 5, 0
endin

	</CsInstruments>
<CsScore>
i1 0 1000 
	</CsScore>
</CsoundSynthesizer>

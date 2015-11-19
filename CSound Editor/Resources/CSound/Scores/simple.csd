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
	kElBias chnget "kElBias"
	kAzimuth chnget "kAzimuth"
	kAttenuation chnget "kAttenuation"
	kTrigger chnget "kTrigger"
	kVolume chnget "kVolume"
	kElevation chnget "kElevation"
	kAzPanFactor chnget "kAzPanFactor"
	kElPanFactor chnget "kElPanFactor"
	kOutput chnget "kOutput"
	kRearGain chnget "kRearGain"

	ain soundin "../../Audio/noise.wav"
	aout, krec sndloop ain , kPitch, kTrigger , 5, 0
	aout = aout * kVolume * kAttenuation
	
	if (kOutput == 0) then
		aleft = aout
		aright = aout
	elseif ((kOutput & 1) != 0) then
		aleft, aright hrtfmove2 aout, kAzimuth, kElevation , "../hrtf/hrtf-44100-left.dat", "../hrtf/hrtf-44100-right.dat"
	elseif ((kOutput & 2) != 0) then
		aleft = aout * cos(kAzPanFactor * $M_PI_2)
		aright = aout * sin(kAzPanFactor * $M_PI_2)
	elseif ((kOutput & 4) != 0) then
		aleft, aright hrtfmove2 aout , kAzimuth, kElevation, "../ind_hrtf/hrtf-44100-left.dat", "../ind_hrtf/hrtf-44100-right.dat" 
	elseif ((kOutput & 8) != 0) then
		aleft, aright pan2 aout, kAzPanFactor, 1
	elseif ((kOutput & 16) != 0) then
		aleft, aright hrtfmove2 aout, kAzimuth, 0 , "../hrtf/hrtf-44100-left.dat", "../hrtf/hrtf-44100-right.dat"
		kTopGain = kElPanFactor * (1 + kElBias)
		kBottomGain = (1 - kElPanFactor) * (1 - kElBias) * kRearGain
		atopleft = aleft * kTopGain
		abottomleft = aleft * kBottomGain
		atopright = aright * kTopGain
		abottomright = aright * kBottomGain
		outch 1, atopleft, 2, atopright, 5, abottomleft, 6, abottomright
	endif
	if ((kOutput & 16) == 0) then
		outs aleft, aright
	endif
			
endin

	</CsInstruments>
<CsScore>
i1 0 1000 
	</CsScore>
</CsoundSynthesizer>

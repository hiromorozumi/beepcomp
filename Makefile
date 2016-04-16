all: normal

normal:
	g++ -o beepcomp_dev MData.cpp DData.cpp Astro.cpp LFO.cpp OSC.cpp NOSC.cpp DelayLine.cpp MML.cpp MPlayer.cpp Dialog.cpp Button.cpp Knob.cpp GUI.cpp main.cpp beepcomp.res -I./include -L./lib -lsfml-graphics -lsfml-window -lsfml-system -lsfml-main -lmp3lame lib/libsndfile-1.lib lib/portaudio_x86.lib
	
pipe:
	g++ -o beepcomp_dev MData.cpp DData.cpp Astro.cpp LFO.cpp OSC.cpp NOSC.cpp DelayLine.cpp MML.cpp MPlayer.cpp Dialog.cpp Button.cpp Knob.cpp GUI.cpp main.cpp beepcomp.res -I./include -L./lib -lsfml-graphics -lsfml-window -lsfml-system -lsfml-main -lmp3lame lib/libsndfile-1.lib lib/portaudio_x86.lib &> err.txt

pipewarning:
	g++ -o beepcomp_dev MData.cpp DData.cpp Astro.cpp LFO.cpp OSC.cpp NOSC.cpp DelayLine.cpp MML.cpp MPlayer.cpp Dialog.cpp Button.cpp Knob.cpp GUI.cpp main.cpp beepcomp.res -I./include -L./lib -lsfml-graphics -lsfml-window -lsfml-system -lsfml-main -lmp3lame lib/libsndfile-1.lib lib/portaudio_x86.lib -Wall &> err.txt
	
release:
	g++ -o beepcomp MData.cpp DData.cpp Astro.cpp LFO.cpp OSC.cpp NOSC.cpp DelayLine.cpp MML.cpp MPlayer.cpp Dialog.cpp Button.cpp Knob.cpp GUI.cpp main.cpp beepcomp.res -I./include -L./lib -lsfml-graphics -lsfml-window -lsfml-system -lsfml-main -lmp3lame lib/libsndfile-1.lib lib/portaudio_x86.lib -mwindows
	
debug:
	g++ -o beepcompd -g MData.cpp DData.cpp Astro.cpp LFO.cpp OSC.cpp NOSC.cpp DelayLine.cpp MML.cpp MPlayer.cpp Dialog.cpp Button.cpp Knob.cpp GUI.cpp main.cpp beepcomp.res -I./include -L./lib -lsfml-graphics -lsfml-window -lsfml-system -lsfml-main -lmp3lame lib/libsndfile-1.lib lib/portaudio_x86.lib
all: normal

normal:
	g++ -o beepcomp_dev Config.cpp MData.cpp DData.cpp Astro.cpp LFO.cpp Fall.cpp OSC.cpp NOSC.cpp DelayLine.cpp MML.cpp MPlayer.cpp Dialog.cpp Button.cpp Knob.cpp GUI.cpp main.cpp beepcomp.res -I./include -L./lib -lsfml-graphics -lsfml-window -lsfml-system -lsfml-main -lmp3lame lib/libsndfile-1.lib lib/portaudio_x86.lib

short:
	g++ -o beepcomp_dev Config.cpp MData.o DData.o Astro.o LFO.o Fall.o OSC.o NOSC.o DelayLine.o MML.cpp MPlayer.cpp Dialog.cpp Button.cpp Knob.cpp GUI.cpp main.cpp beepcomp.res -I./include -L./lib -lsfml-graphics -lsfml-window -lsfml-system -lsfml-main -lmp3lame lib/libsndfile-1.lib lib/portaudio_x86.lib
	
pipe:
	g++ -o beepcomp_dev Config.cpp MData.cpp DData.cpp Astro.cpp LFO.cpp Fall.cpp OSC.cpp NOSC.cpp DelayLine.cpp MML.cpp MPlayer.cpp Dialog.cpp Button.cpp Knob.cpp GUI.cpp main.cpp beepcomp.res -I./include -L./lib -lsfml-graphics -lsfml-window -lsfml-system -lsfml-main -lmp3lame lib/libsndfile-1.lib lib/portaudio_x86.lib &> err.txt

pipewarning:
	g++ -o beepcomp_dev Config.cpp MData.cpp DData.cpp Astro.cpp LFO.cpp Fall.cpp OSC.cpp NOSC.cpp DelayLine.cpp MML.cpp MPlayer.cpp Dialog.cpp Button.cpp Knob.cpp GUI.cpp main.cpp beepcomp.res -I./include -L./lib -lsfml-graphics -lsfml-window -lsfml-system -lsfml-main -lmp3lame lib/libsndfile-1.lib lib/portaudio_x86.lib -Wall &> err.txt
	
release:
	g++ -o beepcomp Config.cpp MData.cpp DData.cpp Astro.cpp LFO.cpp Fall.cpp OSC.cpp NOSC.cpp DelayLine.cpp MML.cpp MPlayer.cpp Dialog.cpp Button.cpp Knob.cpp GUI.cpp main.cpp beepcomp.res -I./include -L./lib -lsfml-graphics -lsfml-window -lsfml-system -lsfml-main -lmp3lame lib/libsndfile-1.lib lib/portaudio_x86.lib -mwindows

portable2:
	g++ -o beepcomp_portable Config.cpp MData.cpp DData.cpp Astro.cpp LFO.cpp Fall.cpp OSC.cpp NOSC.cpp DelayLine.cpp MML.cpp MPlayer.cpp Dialog.cpp Button.cpp Knob.cpp GUI.cpp main.cpp beepcomp.res -I./include -L./lib -lsfml-graphics -lsfml-window -lsfml-system -lsfml-main -lmp3lame lib/libsndfile-1.lib lib/portaudio_x86.lib -mwindows
	
debug:
	g++ -o beepcompd -g Config.cpp MData.cpp DData.cpp Astro.cpp LFO.cpp Fall.cpp OSC.cpp NOSC.cpp DelayLine.cpp MML.cpp MPlayer.cpp Dialog.cpp Button.cpp Knob.cpp GUI.cpp main.cpp beepcomp.res -I./include -L./lib -lsfml-graphics -lsfml-window -lsfml-system -lsfml-main -lmp3lame lib/libsndfile-1.lib lib/portaudio_x86.lib
	
synthModules:
	g++ -c MData.cpp -I./include -L./lib;
	g++ -c DData.cpp -I./include -L./lib;
	g++ -c Astro.cpp -I./include -L./lib;
	g++ -c LFO.cpp -I./include -L./lib;
	g++ -c Fall.cpp -I./include -L./lib;
	g++ -c OSC.cpp -I./include -L./lib;
	g++ -c NOSC.cpp -I./include -L./lib;
	g++ -c DelayLine.cpp -I./include -L./lib;
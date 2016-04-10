
## Delay Effect ##

There is a nice stereo delay effect that is added to your track by default when it first plays. You can turn off the delay by pushing the DELAY button or pressing F7.

You can set the delay parameters in your source. Your commands will go in the Global section. For instance:

    @G
    DELAY=ON
    DELAYTIME=200
    DELAYLEVEL=50
    
    @1
    CDEFG

This gives you a pretty prominent delay effect with a short timing. Play around with `DELAYTIME=` (in milliseconds) and `DELAYLEVEL=` (scale of 100). You can turn off delay for the whole track by saying `DELAY=OFF`.


## Volume Envelope ##

For each channel, you can shape your sound using the volume envelope. The chart below shows the parameters you can set and how they will shape the volume changes in time.

![Volume Envelope](http://hiromorozumi.com/dev/BeepComp/documentation/beepcomp_volume_envelope.png)

* **Attack Time** ... Defined with `ATTACKTIME=n` (n=0 to 9999). The time, in milliseconds, that takes to get to the initial peak stage. A small or no attack time results in a sharp sound. A large attack time gets a soft sound.
* **Peak Time** ... Defined with `PEAKTIME=n` (n=0 to 9999). The time, in milliseconds, that the sound stays in its peak.
* **Peak Level** ... Defined with `PEAKLEVEL=n` (n=0 to 100). How loud the sound is in its peak.
* **Decay Time** ... Definied with `DECAYTIME=n` (n=0 to 9999). Determines how fast it goes downs to the plateau of the sustain stage.
* **Sustain Level** ... Defined with `SUSTAINLEVEL=n` (n=0 to 100). How loud the sound stays in its last plateau stage until the note turns off.
* **Release Time** .... Defined with `RELEASETIME=n` (n=0 to 9999). If the channel rests after a note is turned off, you get into the release stage where the note gradually fades away.

Let's experiment with these parameters.

    @1
    ATTACKTIME=1000
    L1 CDE

Each note starts very slowly.

    @1
    ATTACKTIME=0
    DECAYTIME=1000
    SUSTAINLEVEL=0
    L1 CDE

Now you get a hard, bell-like tone. The volume will decay all the way to 0 because the level of the sustain stage is set to 0.

Note that if you don't set one of these parameters, it'll come in its default value. The default set values are:

    ATTACKTIME=22
    PEAKTIME=18
    PEAKLEVEL=90
    DECAYTIME=250
    SUSTAINLEVEL=50
    RELEASETIME=40

I invite you to play around with different settings for each parameter. By the way, you could get unprocessed square wave tone like this:

    ATTACKTIME=0
    PEAKTIME=0
    PEAKLEVEL=0
    DECAYTIME=0
    SUSTAINLEVEL=100
    RELEASETIME=0


## LFO Effect ##

You can use this pitch LFO effect for each channel to give it a nice vibrato. Here is an example to try:

    @1
    LFO=ON
    LFOSPEED=6
    LFORANGE=30
    LFOWAIT=300
    L2 CDEFG

`LFO=` turns on the pitch LFO effect. It's turned off by default. `LFOSPEED=` sets the number of cycles for the pitch change. `LFORANGE=` determines how big the pitch change should occur, specified by the cent of semitones (so 100 will set it to one semitone). `LFOWAIT=` sets the time to wait to start the LFO effect, specified in milliseconds.

You could even use this LFO effect to get really wacky results. Try setting `LFOSPEED=1` and `LFORANGE=2400` :)


## Astro Effect ##

This is a very fun effect you can add to your music. The name comes from a game titled _Astro Wars_, which I used to play on a little TV game console. (search with "Astro Wars SCV" on YouTube :)) What this effect does is to add a square wave LFO to the pitches. The resulting sound is a out-of-world and funky wobbly sound. Try:

    @1
    ASTRO=14
    L4 CDEFG

The number you specify after `ASTRO=` is the number of times that the wobble happens. Between 10 and 20 is good to start with, but try different values to see which value works for you.


## Wave Tables ##

(Not yet fully implemented)

Right now, only the square wave and sine wave are available. But I plan to add capabilities to choose different wave tables so you can have more sounds to choose from.

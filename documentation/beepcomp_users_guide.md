<hr>
<hr>
# BeepComp User's Guide #
<hr>
<hr>
<br />

## Contents ##

* [About BeepComp](#about_beepcomp)
* [Getting Started](#getting_started)
* [Tutorials](#tutorials)
* [Basics](#basics)
	* [Main Editor](#basics_main_editor)
	* [Writing Your First Track](#basics_writing_your_first_track)
	* [Saving and Loading Your Work](#basics_saving_and_loading_your_work)
	* [Copying and Pasting Source](#basics_copying_and_pasting_source)
	* [Exporting Your Work](#basics_exporting_your_work)
* [Key Commands](#key_commands)
* [Global Section in Detail](#global_section_in_detail)
* [Music Section in Detail](#music_section_in_detail)
	* [Musical Notes](#music_section_musical_notes)
	* [Note Length](#music_section_note_length)
	* [Rests](#music_section_rests)
	* [Octave changes](#music_section_octave_changes)
	* [Ties](#music_section_ties)
	* [Tuplets](#music_section_tuplets)
	* [Volume Changes](#music_section_volume_changes)
	* [Repeats](#music_section_repeats)
	* [Fall Effect](#fall_effect)
	* [Rise Effect](#rise_effect)
	* [Waveform Selection](#waveform_selection)
	* [Preset Selection](#preset_selection)
	* [Volume Envelope Commands](#volume_envelope_commands)
	* [LFO Commands](#lfo_commands)
	* [Astro Command](#astro_command)
* [Drum Section in Detail](#drum_section_in_detail)
	* [Drum Hits](#drum_section_drum_hits)
	* [Quieter Drum Hits](#drum_section_quieter_drum_hits)
	* [Note Length](#drum_section_note_length)
	* [Rests](#drum_section_rests)
	* [Ties](#drum_section_ties)
	* [Tuplets](#drum_section_tuplets)
* [Delay Effect](#delay_effect)
* [Volume Envelope](#volume_envelope)
* [LFO Effect](#lfo_effect)
* [Astro Effect](#astro_effect)
* [Waveforms](#waveforms)
* [Tone Presets](#tone_presets)
* [Bookmarking Your Start Position](#bookmarking_your_start_position)
* [Auto Saving](#auto_saving)
* [Syntax Reference](#syntax_reference)
	* [General](#syntax_reference_general)
	* [Global Section](#syntax_reference_global_section)
	* [Music Channel](#syntax_reference_music_channel)
	* [Drum Channel](#syntax_reference_drum_channel)
* [Uninstalling BeepComp](#uninstalling_beepcomp)
* [Staying Updated](#staying_updated)
* [If you found issues/bugs...](#if_you_found_issues_bugs)
* [BeepComp on the Web](#beepcomp_on_the_web)
* [Contact Me!](#contact_me)

<div id="about_beepcomp" />
## About BeepComp ##

[BeepComp](http://hiromorozumi.com/beepcomp) lets you compose chiptune tracks with text files. It features a nice retro-sounding synthesizer engine with 9 music channels and 1 drum channel.

BeepComp is an open-source project. This means that you're welcome to modify the source code however you want. BeepComp is a also shareware that anyone can use freely. It is distributed under the MIT license.

<div id="getting_started" />
## Getting Started ##

Use the latest installer package available on the [Sourceforge top page](https://sourceforge.net/projects/beepcomp/). Run this set up file, and you'll be all set for your journey!

#### Windows ####

Launch the MSI installer file, **beepcomp_vx-x-x.msi**, (x-x-x fills the version number) which you just downloaded. Your program will be installed in the folder:

- **C:\Program Files\BeepComp\\** on 32-bit Windows
- **C:\Program Files (x86)\BeepComp\\** on 64-bit Windows

After installation, BeepComp will be on the list of installed programs. On Windows XP & 7, go to Start->All Programs->BeepComp. You can also click the shortcut that has been created on your Desktop.

#### Mac / Linux ####

I'll be working on these versions soon :)

<div id="tutorials" />
## Tutorials ##

The BeepComp homepage has section named [Your First Tutorial](http://hiromorozumi.com/beepcomp/first_tutorial_1.html), which will be your great starting point. The later section [Writing Your First Track](#basics_writing_your_first_track) on this documentation has a similar content.

After exploring those tutorials, exploring the tutorial files included in the **userdata** folder (the program's default save location) will be helpful, also. Simply load one of these files named **\_tutorial\_---.txt**, read and play to see writing what produces what result. I recommend starting from **\_tutorial\_basic\_1.txt**.

These files are located in:

- **C:\Documents and Settings\\{USERNAME}\My Documents\BeepComp\userdata** on XP
- **C:\Users\\{USERNAME}\\Documents\BeepComp\userdata** on Vista and later

<div id="basics" />
## Basics ##

<div id="basics_main_editor" />
### Main Editor ###

![Main Editor Screen](beepcomp_screen_explain.png)

* **Editor Area** ... You edit your music source text here.
* **Control Panel** ... Play/pause/save/load/export the music you write and control other things.
* **Cursor** ... Your point of control over the text.
* **Your Text** ... This single text file is your whole track - simple, eh?
* **Gain Meter** ... For visualizing each channel's activity in real time.
* **Progress Bar** ... Keeps track your song's progress in time.
* **Master Volume** ... Controls how loud the master output is. (Each channel has a separate volume control, too) 

<div id="basics_writing_your_first_track" />
### Writing Your First Track ###

Let's write a very basic "Hello BeepComp" source. Start up BeepComp, press the NEW button, choose "Yes", and type the folloing into the text editor:

    // My first BeepComp track! :)

	@G
	TEMPO=100
	MASTERVOLUME=80

	@1
	CDEFG

And press the play button (with a triangle). You heard an endless series of "do-re-mi-fa-sol...," right? Let me explain the source.

Two forward slashes `//` adds a comment. Following two slashes, you can write anything to make a note to yourself or others. The comment ends at the end of the line.

`@` is used to declare the beginning of each "section." `@G` starts the "global section" where you can set parameters that affect your entire track. Your section declaration stays effective until you declare another section (`@1` in this case).

`TEMPO=` sets the tempo of the track. `MASTERVOLUME=` sets the master output gain. The volume knob will change accordingly to the master volume when you play your track from the beginning.

`@1` declares the start of the first music channel section. Everything you write thereafter will be written to the first channel until you declare another section.

Then we have the musical notes `CDEFG`. Note that you have to write musical pitches in UPPERCASE.

You actually heard `CDEFG` over and over because BeepComp defaults the player to looping mode. (More about looping later...)

Easy? Let's expand this a little:

    // My first BeepComp track! :)

    @G
    TEMPO=100
    MASTERVOLUME=80

    @1
    O5L4 CDEFG~

    @2
    O4L4 EFGAB~

    @D
    L4 KKKSSS

Now play this. You now hear a mix of three channels.

`O` followed by a single digit number sets the octave. `O4` is the default octave where the middle C is, so `O5` is just one above the default octave. `L` followed by a number ranging from 1 to 64 changes the note length. `L4` sets up the note length to 1/4 of the measure length (quarter notes).

`~` is quite useful. It elongates a note by the currently set note length. You can actually chain multiple `~`s, for instance, `B~~~`.

Now we have `@2` so commands after that goes to the second music channel. You can play up to 9 music channels simultaneously, so you can declare sections up to `@9`.

And you can use the drum channel by declaring `@D`. Write `K` for kick, `S` for snare, `H` for high-hat. In fact, you can use lower case `k`, `s`, `h` which will play a little quieter.

Note that you see some white spaces in above examples. You can insert any amount of white space between commands because they're ignored by the interpreter.

<div id="basics_saving_and_loading_your_work" />
### Saving and Loading Your Work ###

Now let's save your work. Click the SAVE button (or F11), type in **hellobeep.txt**, and click the Save button. Now open NotePad or any text editor of your choice, choose to open a file, navigate to the **userdata** folder, the default user folder, and open up the **hellobeep.txt** file we just created.

(Note that default **userdata** folder will be at following locations:)

* **C:\Documents and Settings\\{USERNAME}\My Documents\BeepComp\userdata** on XP
* **C:\Users\\{USERNAME}\\Documents\BeepComp\userdata** on Vista and later

See? Your BeepComp source is simply a text file. And you could even continue to work on your track in your text editor if you want.

By the way, you can use the ALT + O shortcut from the main editor to open up your **userdata** folder anytime.

<div id="basics_copying_and_pasting_source" />
### Copying and Pasting Source ###

A nice thing about working with text is that you can easily copy and paste between BeepComp and other sources!

Try this - Start a new file (NEW button or F9), Go to [this page](http://codrspace.com/hiromorozumi/beepcomp-source-ys-1-field-music/), copy all the code inside the grey box, and press CTRL + V in BeepComp, and play.

I hope you see how easy it is to share work with each other on the web :)

Of course, going the other way is easy, too. You can easily copy your work from BeepComp by pressing CTRL + A (select all) then CTRL + C (copy), and paste what you just copied into your external applications like your Outlook to email your music to a friend, for instance.

<div id="basics_exporting_your_work" />
### Exporting Your Work

You can create audio files of your work by pressing the EXPORT button(or F12). Currently supported formats are: WAV, MP3 and OGG files. After you export your work, you can add fades and other finishing touches on audio editors like Audacity.

Your music won't be looped in your exported file.

Here is a tip if you want the exported data to be looped (so you can fade out after the loop point etc.) - Enclose the entire content of each channel with `{}`. Your exported file will come with your entire music repeated once.

<div id="key_commands" />
## Key Commands ##

###Mouse Input###

* **Mouse position** ... Navigates cursor position
* **Mouse Wheel** ... Helps scrolling text up/down
* **Left Button** ... Presses button on screen / selects text
* **Right Button** ... Clears text selection

###Key Commands: Main Editor###

* **F1** ... Play from beginning
* **F2** ... Pause / Unpause
* **F3** ... Rewind
* **F4** ... Fast Forward
* **F5** ... Show key commands
* **F6** ... Show Documentation
* **F7** ... Turn delay on/off
* **F9** ... Start a new file
* **F10** ... Load from a file
* **F11** ... Save to a file
* **F12** ... Export to an audio file
* **ARROW** ... Move cursor
* **ESCAPE** ... Quit
* **BACKSPACE** ... Backspacing, delete selection
* **SHIFT** ... Select text
* **SHIFT + ARROW** ... Expand text selection
* **ALT** ... Clear text selection
* **CTRL + Z** ... Undo
* **CTRL + C** ... Copy
* **CTRL + V** ... Paste
* **CTRL + A** ... Select all
* **CTRL + S** ... Save
* **ALT + S** ... Quick-save to last saved file
* **ALT + V** ... Open the system volume control
* **ALT + D** ... Open the audio device control
* **ALT + I** ... Initialize the audio device
* **ALT + O** ... Open "userdata" folder 
* **HOME** ... Go to the very top
* **END** ... Go to the very bottom
* **Page Up** ... Go up by one screen height
* **Page Down** ... Go down by one screen height

###Key Commands: File Dialog###

* **ESCAPE** ... Exits dialog
* **HOME** ... Go back to the default starting folder
* **END** ... Go back to previous folder
* **F1** ... Toggle file types to display
* **F12** ... Load/Save/Export (as indicated)
* **UP/DOWN** ... Navigate through the list of files in the current directory
* **RIGHT** ... Selects a file name, Go to child folder (when over <<<)
* **LEFT** ... Go to parent folder (when over <<<)
* **BACKSPACE** ... Backspacing in the file name input
* **ALT + O** ... Open the currently selected directory in Explorer
* **ALT + D** ... Go to the 'Desktop'

<div id="global_section_in_detail" />
## Global Section in Detail ##

The global section, defined by `@G`, is where you write commands that influence the entire track. The elements you can control in the global section are:

* Master volume ... `MASTERVOLUME=n`
* Initial channel volume ... `V1=n`, `V2=n`, `VD=n` etc...
* Tempo ... `TEMPO=n`
* Delay ... `DELAY=ON` / `OFF`, `DELAYTIME=n`, `DELAYLEVEL=n`
* Looping ... `LOOP=ON` / `OFF`

The master volume is also manually controllable with the volume knob in the control panel. Note that at the start of track, the master volume gets initialized to the declared value by `MASTERVOLUME=` (or default value 80 if none specified) so the volume knob position gets reset.

The tempo, measured by beats per measure (BPM), determines how fast your track is played. The `TEMPO=` command takes a value between 30 and 300.

The delay effect is applied to the whole track (not to inidividual track). See the Delay Effect section for more. 

By default, your track is set to play in an infinite loop. You can turn off track looping by `LOOP=OFF`. If you want to loop your track a certain number of times, you could first set `LOOP=OFF` in your global section and then enclose the entire real time data with `{}` in each channel.

Here is an example of using the global section to set up the volume, tempo and looping for your track:

    @G
    MASTERVOLUME=50 // not too loud
    TEMPO=80 // kind of slow
    LOOP=OFF

    @1
    GFEDC // this won't be looped

<div id="music_section_in_detail" />
## Music Section in Detail ##

Each section for a music is defined with `@n`. Your first channel section will start with `@1`, your second with `@2` and so forth.

<div id="music_section_musical_notes" />
###Musical Notes###

Musical notes are written with note names `C`, `D`, `E`, `F`, `G`, `A` and `B` in UPPERCASE. The following example plays an ascending C major scale:

    @1
    CDEFGAB

A note with a sharp is written with `#` following its pitch name - `C#`, for example. A note with a flat is written with `b`, for example, `Ab`. A chromatic scale can be played this way:

    @1
    CC#DD#EFF#GG#AA#BBbAAbGGbFEEbDDbC

<div id="music_section_note_length" />
###Note Length###

To specify the current note length, use the `L` command. The letter `L` should be followed by a number ranging from 1 to 64, the factor which should divide the measure length. `L4` sets the 1/4 of the measure length, so you get the quarter note length. `L8` gives you the eighth note length. Your length definition stays effective until the next `L` command.

The below example plays a few long half notes followed by a series of fast 32nd notes:

    @1
    L2 CDE L32 GABAGFED

<div id="music_section_rests" />
###Rests###

Rests can be easily inserted with a symbol `:`. The length of the rest is determined by the current note length.

    @1
    L4 C:CDE:EFG:G:E:::

The above example should show you how rests can be inserted intuitively.

<div id="music_section_octave_changes" />
###Octave Changes###

To specify the current octave range, use the `O` command followed by a number. The middle octave where the middle C sits is `O4`, and it's the default starting octave of each channel. The example below plays some notes in a high octave range `O6` then plays low notes in the octave `O2`.

    @1
    O5 GG:G:DE:C::: O2 C:::

Another way to navigate through octave ranges is to use the `<` and `>` symbols. `<` means go down by one octave, and `>` means go up by one octave. The example below sweeps up and down the entire possible octave ranges:

    @1
    O1 C>C>C>C>C>C>C>C<C<C<C<C<C<C<C

<div id="music_section_ties" />
###Ties###

You can elongate a note quite easily by the multiple of the current note length. To do that, use the `~` symbol after the note you want to elongate. For example, try this example:

    @1
    L8 C~~DE~~CE~C~E~~~

You can be pretty liberal with chaining `~` symbols if you want, for example try `L8C~~~~~~~` :)

<div id="music_section_tuplets" />
###Tuplets###

Tuplets are useful when you have to insert triplets or any other notes in non-standard subdivisions. Tuplets can be entered using the `[]` symbols. You simply enclose the notes you want to group in the space of the current note length:

    @1
    L4 C [EDC] D [FED] E [GFE]

You should have heard the notes enclosed in brackets as triplets in the space of a quarter note.

You can specify this grouping space, also. For example, if you want to write a quintuplets in a quarter note space in the midst of 8th notes:

    @1
    L8 C~~~ [4CDbC<B>C] >C<BbAbGFE

<div id="music_section_volume_changes" />
###Volume changes###

You can specify the volume of a channel at any point by using the `V` command. Write `V` followed by a value from 0 to 10, like `V10` or `V4`.

    @1
    V10 CDEFG V5 CDEFG

The above source will play the first phrase in full volume and the second half as loud.

If you want to make many gradual changes in your channel, symbols `^` and `_` will come in handy. `^` increases the volume by 10%, and `_` decreases by 10%. Try this:

    @1
    V0 L32
    AEFD ^ AEFD ^ AEFD ^ AEFD ^ AEFD ^ AEFD ^ AEFD ^ AEFD ^ AEFD  ^
    AEFD _ AEFD _ AEFD _ AEFD _ AEFD _ AEFD _ AEFD _ AEFD _ AEFD

<div id="music_section_repeats" />
###Repeats###

If you want to repeat a phrase or a section of your music, you can use the `{}` symbols. Anything enclosed in these symbols are repeated.

    @1
    {EbDD:} EbDD: Bb~~~

You can actually nest a repeat inside another repeat. For example:

    @1
    {CDEF{GFEF}ED} {{{C~}}}

Note that nesting makes the total number of repeats grow exponentially. The latter part of the above example is repeated 2 x 2 x 2 = 8 times.

You can also specify a certain number of times which a phrase should repeat by adding a single digit number just after the left curly brace like this:

     @1
     {5CDE}{3FGA}

<div id="fall_effect" />
###Fall Effect###

If a musical note is followed by comma `,`, the Fall effect is added, causing it to decrease its pitch in time. Here is an example where you can observe how this effect works:

    @1
    FALLSPEED=1200
    FALLWAIT=500
    L1
    C,D,E,

`FALLSPEED` is how many 100ths of a semitone the pitch decreases in a second. 1200 means a range of one octave per second. `FALLWAIT=` specifies the time in milliseconds before the effect starts.

The Fall point can be set in the middle of a tie series. This makes rhythmical timing of the effect easy. Try this example:

    @1
    FALLSPEED=1200
    FALLWAIT=0
    L16 O5
    FECD~~~~ ~,~~~ <A>CDC
    FECD~~ ~,~ <A~G~F~E~ ::::

<div id="rise_effect" />
###Rise Effect###

Placing an asterisk `*` is before a note causes that note to scoop up to its pitch, producing a similar effect of a synthesizer pitch-bend wheel. `RISESPEED=` sets the speed for the rise in 100ths of a semitone per second. `RISERANGE=` determines how wide the pitch change should span. This effect can nicely add liveliness to your musical lines:

    @G TEMPO=160
    @1
    RISESPEED=2400
    RISERANGE=200
    L8 O4 *EGA> *CCC *CCC *CC *C <AGD#EC<AG>
     *CCC *CCC *CC ::::::::

<div id="waveform_selection" />
###Waveform Selection###

You can choose a waveform other than the default square wave for each channel by declaring `WAVEFORM=n`.
See [Waveforms](#waveforms).

<div id="preset_selection" />
###Preset Selection###

You may choose a preset to for a quick tone shaping by declaring `PRESET=(presetname)`. See [Tone Presets](#tone_presets).

<div id="volume_envelope_commands" />
###Volume Envelope Commands###

Each channel has a volume envelope filter to help shape a unique sound. See [Volume Envelope](#volume_envelope).

<div id="lfo_commands" />
###LFO Commands###

The pitch LFO filter adds a vibrato effect. See [LFO Effect](#lfo_effect).

<div id="astro_command" />
###Astro Command###

The "Astro" effect is a unique 80's-ish wobble effect. See [Astro Effect](#astro_effect).

<div id="drum_section_in_detail" />
## Drum Section in Detail ##

Writing `@D` starts the drum section.

<div id="drum_section_drum_hits" />
###Drum Hits###

You can write a snare hit with `S`, and a kick with `K`. These normal hits come in UPPERCASE alphabets. With `:` giving a rest, a basic kick-and-snare groove can be written:

    @D
    K:S: KKS:

The hi-hat is written with `H`. A disco groove pattern can be played like this:

    @D
    K:H: S:H: K:H: S:HH

<div id="drum_section_quieter_drum_hits" />
###Quieter Drum Hits###

If you write the drum hits in lower case, you get quieter drum hits. These can be added to give rich dynamics to your groove:

    @D
    K::k S::k :kK: S:HH K:hh S:kk :shh hsSS  

<div id="drum_section_note_length" />
###Note Length###

Just as we did for a music channel, we can specify the note length as you go in the drum channel as well. Use the `L` command with a number ranging from 1 to 64, with the number being the subdivision factor of the measure. For example:

    @D
    L2 S L4 S L8 S L16 SS L32 SSSS L64 SSSSSSSS   

But you might want to set to the smallest subdivision once at the beginning and rely more on using rests and ties. You'll find this way much easier especially when working with groove-based music :)

<div id="drum_section_rests" />
###Rests###

A rest is indicated with a `:` as illustrated above. The length of a rest is determined by the currently set note length by the `L` command.

<div id="drum_section_ties" />
###Ties###

Instead of using a rest to fill a gap, you can use a tie symbol `~` to elongate a hit. The difference is that the tie ensures that the note gets played through the elongated space. When you hit and rest, if the note length is set very short, the abrupt cut-off of the hit might become obvious. Try this example to hear the difference:

    @D
    L64
    S::: S::: S::: S:::
    S~~~ S~~~ S~~~ S~~~

<div id="drum_section_tuplets" />
###Tuplets###

Tuplets make it possible to express quite complicated timings. Just as you saw in tuplets used in a music channel, you have two ways of expressing tuplets. The first way is to enclose hits simply with `[]` where the total length of grouped hits is the currently set note length. The second way is specifying the total length of grouped hits in the format `[n(hits)]`.

Using the both methods, let's illustrate some interesting things you can do to your groove with tuplets:

    @D
    L16
    K::[KK] S~~~ K::: [4SSS]
    K[KK]K[KK] KK[8HHHH] ss[8sss][8SSSS][8SSSSSSSS]

<div id="delay_effect" />
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

<div id="volume_envelope" />
## Volume Envelope ##

For each channel, you can shape your sound using the volume envelope. The chart below shows the parameters you can set and how they will shape the volume changes in time.

![Volume Envelope](beepcomp_volume_envelope.png)

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

I invite you to play around with different settings for each parameter. By the way, you could get a "pure PC beep" tone with an unprocessed square waveform like this:

    ATTACKTIME=0
    PEAKTIME=0
    PEAKLEVEL=70
    DECAYTIME=0
    SUSTAINLEVEL=70
    RELEASETIME=0

<div id="lfo_effect" />
## LFO Effect ##

You can use this pitch LFO effect for each channel to give it a nice vibrato. Here is an example to try:

    @1
    LFO=ON
    LFOSPEED=6
    LFORANGE=30
    LFOWAIT=300
    L2 CDEFG

`LFO=` turns on the pitch LFO effect. It's turned off by default. `LFOSPEED=` sets the number of cycles per second for the pitch change. `LFORANGE=` determines how big the pitch change should occur, specified by the cent of semitones (so 100 will set it to one semitone). `LFOWAIT=` sets the time to wait to start the LFO effect, specified in milliseconds.

You could even use this LFO effect to get really wacky results. Try setting `LFOSPEED=1` and `LFORANGE=2400` :)

<div id="astro_effect" />
## Astro Effect ##

This is a very fun effect you can add to your music. The name comes from a game titled _Astro Wars_, which I used to play on a little TV game console. (search with "Astro Wars SCV" on YouTube :)) What this effect does is to add a square wave LFO to the pitches. The resulting sound is an out-of-world, funky and wobbly sound. Try:

    @1
    ASTRO=14
    L4 CDEFG

The number you specify after `ASTRO=` is the number of times that the wobble happens. Between 10 and 20 is good to start with, but try different values to see which value works for you.

<div id="waveforms" />
## Waveforms ##

Each music channel is by default set to play a square wave tone. But you can change to different a wave form to work with, by declaring `WAVEFORM=` in each channel. The currently available waveforms and their assigned number are: sine(0), square(1, default), sawtooth(3), hybrid sine A(4), hybrid sine B(5). The hybrid sine waves combine a basic sign wave with a few added upper harmonics.

Here is an example where a different waveform is selected:

    @1
    DEFAULTTONE // resets the volume envelope settings to plain beep
    WAVEFORM=2  // sets up a saw tooth wave
	F~:G~:Ab~:>C~:<Ab:G:F~:Eb~:F~~~~~::::

<div id="tone_presets" />
## Tone Presets ##

With one line of the command `PRESET=`, you can set your channel to a set of predetermined tone parameters. You are welcome to try out the currently registered presets by declaring one of the following at the top of a channel:

* `PRESET=BEEP` ... (Square wave) Straight PC Beep tone.
* `PRESET=POPPY` ... (Square wave) Sustained tone with a strong attack.
* `PRESET=POPPYVIB` ... (Square wave) Preset POPPY with vibrato.
* `PRESET=BELL` ... (Square wave) Hard tone that decays in time.

<div id="bookmarking_your_start_position" />
## Bookmarking Your Start Position ##

When you compose a long song, you can take advantage of the bookmarking feature. Placing two percent signs `%%`in your source forces the player to start from where those percent signs are. Here is an example:

     @1 L8
     // verse
     {E~G~G~A~G~G~E~~~}
     %%
     // chorus
     F~G~A~F~>C~C~<G~~~

Now that you have finished working on the verse, you can just focus on the chorus part without having to go back to the top every time you press the play button.

Note that the bookmark will not be effective when you export to an audio file.

<div id="auto_saving" />
## Auto Saving ##

Your work gets automatically saved to a hidden file named "\_\_AUTOSAVED\_\_.txt" in your **userdata** folder every 5 minutes and also when you quit the application. If something unexpected ever happens, you can load this file to retrieve your work.

<div id="syntax_reference" />
## Syntax Reference ##

<div id="syntax_reference_general" />
### General ###

* `//` ... Commenting
* `@G` ... Declares the global section
* `@n` ... Declares a music channel section (n=1,2,3...9)
* `@D` ... Declares the drum channel section
* `%%` ... Bookmarks the player's starting point (Note: not effective when exporting)

<div id="syntax_reference_global_section" />
### Global Section ###

* `MASTERVOLUME=n` ... Sets the master output gain (n=1 to 100)
* `TEMPO=n` ... Sets the track tempo (n=40 to 300)
* `Vn=v` ... Sets the volume (v=0 to 10) for channel n (n=1 to 9, m=0 to 100)
* `VD=v` ... Sets the volume (v=0 to 10) for the drum channel
* `LOOP=?` ... Turns track looping on or off (?=ON or OFF)
* `DELAY=?` ... Turns the delay effect on or off (?=ON or OFF)
* `DELAYLEVEL=n` ... Sets the delay output level (n=0 to 100)
* `DELAYTIME=n` ... Sets the delay timing in milliseconds (n=0 to 999)

<div id="syntax_reference_music_channel" />
### Music channel ###


* `ATTACKTIME=n` ... Sets the envelope attack time in milliseconds (n=1 to 9999)
* `PEAKTIME=n` ... Sets the envelope peak time in milliseconds (n=1 to 9999)
* `PEAKLEVEL=n` ... Sets the envelope peak level (n=0 to 100)
* `DECAYTIME=n` ... Sets the envelope decay time in milliseconds (n=1 to 9999)
* `SUSTAINLEVEL=n` ... Sets the envelope sustain level (n=0 to 100)
* `RELEASETIME=n` ... Sets the envelope release time in milliseconds (n=1 to 9999)
* `LFO=?` ... Turns the pitch LFO effect on or off (?=ON or OFF) 
* `LFORANGE=n` ... Sets the LFO range to cents of a semitone (n=1 to 2400)
* `LFOSPEED=n` ... Sets the LFO speed by cycles per second (n=0 to 100)
* `LFOWAIT=n` ... Sets the time in milliseconds before the LFO effect starts (n=0 to 9999)
* `ASTRO=n` ... Turns on the Astro effect and sets the number of wobbles per second (n=0 to 100, 0 for OFF)
* `ASTRO=OFF` ... Turns off the Astro effect
* `PRESET=?` ... Sets up at once for a predetermined set of tone parameters (?=BEEP available currently)
* `DEFAULTTONE` ... Sets up a straight square wave tone
* `WAVEFORM=n` ... Chooses the waveform that the channel uses (n=0 to 5)
* `FALLSPEED=n` ... Speed of the Fall effect in 100ths of a semitone per second (n=0 to 9600)
* `FALLWAIT=n` ... Wait time in milliseconds before the Fall effect starts (n=0 to 9999)
* `RISESPEED=n` ... Speed of the Rise effect in 100ths of a semitone per second (n=0 to 9600)
* `RISERANGE=n` ... Pitch width for the Rise effect in 100ths of a semitone (n=0 to 9600)  
* `Vn` ... Sets channel volume (n=0,1,2...10)
* `Ln` ... Sets the current note length (n=1 to 64)
* `C` ... Musical note
* `D` ... Musical note
* `E` ... Musical note
* `F` ... Musical note
* `G` ... Musical note
* `A` ... Musical note
* `B` ... Musical note
* `#` ... Sharp
* `b` ... Flat
* `~` ... Elongates note by current note length
* `:` ... Rest for the current note length
* `[---]` ... Tuplet - groups notes --- in the space of current note length
* `[n---]` ... Tuplet - groups notes --- in the space of note length specified by n
* `{---}` ... Repeats the enclosed snippet ---
* `{n---}` ... Repeats the enclosed snippet --- `n` times
* `^` ... Increase channel volume by 10%
* `_` ... Decrease channel volume by 10%
* `,` ... Fall effect, works on the pitch commanded just before
* `*` ... Rise effect, works on the pitch that comes right after

<div id="syntax_reference_drum_channel" />
### Drum Channel ###

* `Vn` ... Sets channel volume (n=0,1,2...10)
* `Ln` ... Sets the current note length (n=1 to 64)
* `K` ... Kick
* `k` ... Kick, quieter
* `S` ... Snare
* `s` ... Snare, quieter
* `H` ... Hi-hat
* `h` ... Hi-hat, quieter
* `~` ... Elongates note by current note length
* `:` ... Rest for the current note length
* `[---]` ... Tuplet - groups notes --- in the space of current note length
* `[n---]` ... Tuplet - groups notes --- in the space of note length specified by n
* `{---}` ... Repeats the enclosed snippet ---
* `{n---}` ... Repeats the enclosed snippet --- `n` times
* `^` ... Increase channel volume by 10%
* `_` ... Decrease channel volume by 10%

<div id="uninstalling_beepcomp" />
## Uninstalling BeepComp ##

You can uninstall the application by using the "Programs and Features" under the Control Panel / Programs menu to uninstall BeepComp. Typing in "appwiz.cpl" in the command prompt or in the search box will direct you to this dialog, also.

Please note that your "userdata" folder inside your "Documents" (or "My Documents" on XP) folder will not be removed, just in case you still have some of your work left in this folder. If you no longer need your "userdata" folder, please delete it along with its parent folder named BeepComp.

<div id="staying_updated" />
## Staying Updated ##

New updates of BeepComp come out often. I ask every user to stay updated since updated versions fix and improve the bugs or issues found on the previous versions. I announce new versions through [the SourceForge Blog page](https://sourceforge.net/p/beepcomp/blog/). I also upload [patches](https://sourceforge.net/projects/beepcomp/files/patches/) whenever there is a critical issue.

Alternatively, you can scroll down the [SourceForge project top page](http://beepcomp.sourceforge.net) and press the button that says "Follow" to receive automatic updates, but be alerted that you will receive third-party junk mail as well. I will find a better newsgroup format that's clean. If anyone knows of a good service, let me know! :)

You can find out the version of BeepComp you're using by clicking the logo in the main editor screen.

When you update to a new release, you will have to first [uninstall](#uninstalling_beepcomp) your current version.

<div id="if_you_found_issues_bugs" />
## If you found issues/bugs...

BeepComp is still in its development stage... so I really appreciate you letting me know about any issues you experience. You're welcome to write in our [forum page](http://beepcomp.freeforums.net/) or email me at beepcomp-at-hiromorozumi-dot-com. I really want to make a program that is reliable and enjoyable to anyone :)

I am also documenting the issues as they get identified on the [Known Issues page](https://sourceforge.net/p/beepcomp/wiki/known_issues/) on sourceForge. 

<div id="beepcomp_on_the_web" />
## BeepComp on the Web ##

The home page for BeepComp is at:
[http://hiromorozumi.com/beepcomp](http://hiromorozumi.com/beepcomp).

Also, there is a [forum page](http://beepcomp.freeforums.net/) where you can join our discussion. I have also set up a [Facebook](https://www.facebook.com/beepcomp) page.

The source code is publicly available at the [SourceForge](http://sourceforge.net) repository. I will also welcome patches if you spot issues. The address of the repository is:
[https://sourceforge.net/p/beepcomp/code/ci/master/tree/](https://sourceforge.net/p/beepcomp/code/ci/master/tree/).

<div id="contact_me" />
## Contact Me! ##

I welcome anyone using BeepComp and also anyone interested in getting involved in this project to getting in touch. Feature suggestions are great. Bug reports are welcome. If you are a coder interested in working together to make this a better application, that's great, too!  Just letting me hear the tracks you create with BeepComp will make my day :) You can contact me by emailing me at beepcomp-at-hiromorozumi-dot-com.

<hr>
<hr>
# BeepComp User's Guide #
<hr>
<hr>
<br />

## Contents ##

## About BeepComp ##

BeepComp lets you compose chiptune tracks with text files. It features a nice retro-sounding synthesizer engine with 9 music channels and 1 drum channel.

## If you found issues/bugs...

BeepComp is still in development stage... so I really appreciate you letting me know about any issues you experience. I really want to make a program that is reliable and enjoyable to anyone :)

## Getting Started ##

Use the latest installer package available on the [Sourceforge top page](https://sourceforge.net/projects/beepcomp/) to install.

#### Windows ####

Launch the MSI installer file you download. Your program will be installed in the folder:

- **C:\Program Files\BeepComp\\** on 32-bit Windows
- **C:\Program Files(x86)\BeepComp\\** on 64-bit Windows

After installation, BeepComp will be on the list of installed programs. On Windows XP & 7, go to Start->All Programs->BeepComp.

#### Mac / Linux ####

I'll be working on these versions soon :)

## Tutorials ##

Some tutorial files are included in the **/userdata** folder inside your installed directory to get you started. Simply load one of these files named **\_tutorial\_---.txt**, read and play to see what you can write into BeepComp and what results you get. I recommend starting from **\_tutorial\_basic\_1.txt**.

## Basics ##

### Main Editor ###

![](beepcomp_screen_explain.png)

* **Editor Area** ... You edit your music source text here.
* **Control Panel** ... Play/pause/save/load/export the music you write and control other things.
* **Cursor** ... Your point of control over the text.
* **Your Text** ... This single text file is your whole track - simple, eh?
* **Gain Meter** ... For visualizing each channel's activity in real time.
* **Master Volume** ... Controls how loud the master output is. (Each channel has a separate volume control, too) 

### Writing Your Music ###

Let's write a very basic, "Hello BeepComp" source. Start up BeepComp, press the NEW button, choose "Yes", and type the folloing into the text editor:

    // My first BeepComp track! :)

	@G
	TEMPO=100
	MASTERVOLUME=80

	@1
	CDEFG

And press the play button (with a triangle). You heard an endless series of "do-re-mi-fa-sol...," right? Let me explain the source.

Two forward slashes `//` adds a comment. Following two slashes, you can write anything to make a note to yourself or others in the source.

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

`~` is quite useful. It elongates a note by the currently set note length.

Now we have `@2` so commands after that goes to the second music channel. You can play up to 9 music channels simultaneously, so you can declare sections up to `@9`.

And you can use the drum channel by declaring `@D`. You can use `K` for kick, `S` for snare, `H` for high-hat. In fact, you can use lower case `k`, `s`, `h` which will play a little quieter.

### Saving/Loading Your Work ###

Now let's save your work. Click the SAVE button (or F11), type in **hellobeep.txt**, and click the Save button. Now open NotePad or any text editor of your choice, choose to open a file, navigate to the **/userdata** in your installation folder, and open up the **hellobeep.txt** file we just created.

See? Your BeepComp source is simply a text file. And you could even continue to work on your track in your text editor if you want.

### Copying/Pasting Source ###

A nice thing about working with text is that you can easily copy and paste between BeepComp and other sources!

Try this - Start a new file (NEW button or F9), Go to [this page](http://codrspace.com/hiromorozumi/beepcomp-source-ys-1-field-music/), copy all the code inside the grey box, and press CTRL + V in BeepComp, and play. 

I hope you see how easy it is to share work with each other on the web :)

Of course, you can easily copy your work from BeepComp by pressing CTRL + A (select all) then CTRL + C (copy), to email your music to a friend, for instance.

### Exporting Your Work

You can create audio files of your work by pressing the EXPORT button(or F12). Currently supported formats are: WAV, MP3 and OGG files. After you export your work, you can work on the audio file further on audio editors like Audacity.

Your music won't be looped in your exported file.

Here is a tip if you want the exported data to be looped (so you can fade out after the loop point etc.) - Enclose the entire content of each channel with `{}`. Your exported file will come with your entire music repeated once.

## Key Commands ##

###Mouse Input###

* **Mouse position** ... Navigates cursor position
* **Mouse Wheel** ... Helps scrolling text up/down
* **Left Button** ... Presses button on screen / selects text
* **Right Button** ... Clears text selection

###Key Commands: Main Editor###

* **F1** ... Play from beginning
* **F2** ... Pause / Unpause
* **F3** ... Rewind (Will be implemented soon)
* **F4** ... Fast Forward (Will be implemented soon)
* **F5** ... Show key commands
* **F6** ... Documentation (Will be implemented soon)
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
* **CTRL + Z** ... Undo
* **CTRL + C** ... Copy
* **CTRL + V** ... Paste
* **CTRL + A** ... Select all
* **CTRL + S** ... Save
* **HOME** ... Go to the very top
* **END** ... Go to the very bottom

###Key Commands: File Dialog###

* **ESCAPE** ... Exits dialog
* **END** ... Go back to previous folder
* **F1** ... Toggle file types to display
* **F12** ... Load/Save/Export (as indicated)
* **UP/DOWN** ... Navigate through the list of files in the current directory
* **RIGHT** ... Selects a file name, Go to child folder (when over <<<)
* **LEFT** ... Go to parent folder (when over <<<)
* **BACKSPACE** ... Backspacing in the file name input

## In Detail: Global Section ##

## In Detail: Music Section ##

## In Detail: Drum Section ##

## Delay Effect ##

## Volume Envelope ##

## LFO Effect ##

## Astro Effect ##

## Wave Tables ##

## Syntax Reference ##

### General ###

* `//` ... Commenting
* `@n` ... Declares a section (n=1,2,3...9)
* `@D` ... Declares a drum channel section

### Global Section ###

* `MASTERVOLUME=n` ... Sets the master output gain (n=1 to 100)
* `TEMPO=n` ... Sets the track tempo (n=30 to 300)
* `Vn=m` ... Sets the volume for channel n (n=1 to 9, m=0 to 100)
* `LOOP=?` ... Turns track looping on or off (?=ON or OFF)
* `DELAY=?` ... Turns the delay effect on or off (?=ON or OFF)
* `DELAYLEVEL=n` ... Sets the delay output level (n=0 to 100)
* `DELAYTIME=n` ... Sets the delay timing in milliseconds (n=0 to 999)

### Music channel ###

##### Commands processed before track starts playing #####

* `ATTACKTIME=` ... Sets the envelope attack time in milliseconds (n=1 to 9999)
* `PEAKTIME=` ... Sets the envelope peak time in milliseconds (n=1 to 9999)
* `PEAKLEVEL=` ... Sets the envelope peak level (n=0 to 100)
* `DECAYTIME=` ... Sets the envelope decay time in milliseconds (n=1 to 9999)
* `SUSTAINLEVEL=` ... Sets the envelope sustain level (n=0 to 100)
* `RELEASETIME=` ... Sets the envelope release time in milliseconds (n=1 to 9999)
* `LFO=?` ... Turns the pitch LFO effect on or off (?=ON or OFF) 
* `LFORANGE=n` ... Sets the LFO range to cents of a semitone (n=1 to 2400)
* `LFOSPEED=n` ... Sets the LFO speed by cycles per second (n=0 to 100)
* `ASTRO=n` ... Turns on the astro effect and sets the number of wobbles per second (n=1 to 100)

##### Commands processed as the track progresses #####

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
* `{}` ... Repeats the enclosed snipet
* `^` ... Increase channel volume by 10%
* `_` ... Decrease channel volume by 10%

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
* `{}` ... Repeats the enclosed snipet
* `^` ... Increase channel volume by 10%
* `_` ... Decrease channel volume by 10%

## Contact Me! ##

I welcome anyone using BeepComp and also anyone interested in getting involved to getting in touch. Feature suggestions are great. Bug reports are welcome. If you are a coder interested in working together to make this a better application, that's great, too!  Just letting me hear the tracks you create with BeepComp will make my day :) To e-mail me, just write through the [SourceForge contact page](https://sourceforge.net/u/smokinhiro/profile/send_message).

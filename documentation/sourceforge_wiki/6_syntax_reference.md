
## Syntax Reference ##


### General ###

* `//` ... Commenting
* `@G` ... Declares the global section
* `@n` ... Declares a music channel section (n=1,2,3...9)
* `@D` ... Declares the drum channel section


### Global Section ###

* `MASTERVOLUME=n` ... Sets the master output gain (n=1 to 100)
* `TEMPO=n` ... Sets the track tempo (n=30 to 300)
* `Vn=v` ... Sets the volume (v=0 to 10) for channel n (n=1 to 9, m=0 to 100)
* `VD=v` ... Sets the volume (v=0 to 10) for the drum channel
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
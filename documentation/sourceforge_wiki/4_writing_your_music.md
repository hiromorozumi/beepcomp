
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


## Music Section in Detail ##

Each section for a music is defined with `@n`. Your first channel section will start with `@1`, your second with `@2` and so forth.

It's important to understand that there are two types of commands:

1. Commands that are processed before the track starts playing
2. Commands that are processed in real time as your track progresses

Let's break the types of commands into these two groups:

**Commands processed before the track starts playing:**

* Volume envelope commands
* LFO commands
* Astro command

Note that these will only be processed at the beginning of the track. Therefore, you cannot change the LFO speed in the middle of a song, for instance.

**Commands processed in real time:**

* Octave command
* Octave increment/decrement command
* Note length command
* Musical notes, ties and rests
* Tuplets
* Volume changes
* Repeats

###Volume Envelope###

See [Volume Envelope](#volume_envelope)

###LFO Commands###

See [LFO Effect](#lfo_effect)

###Astro Command###

See [Astro Effect](#astro_effect)


###Musical Notes###

Musical notes are written with note names `C`, `D`, `E`, `F`, `G`, `A` and `B` in UPPERCASE. The following example plays an ascending C major scale:

    @1
    CDEFGAB

A note with a sharp is written with `#` following its note name, `C#`, for example. A note with a flat is written with `b` following its note name, `Ab`. A chromatic scale can be played this way:

    @1
    CC#DD#EFF#GG#AA#BBbAAbGGbFEEbDDbC


###Note Length###

To specify the current note length, use the `L` command. The letter `L` should be followed by a number ranging from 1 to 64, the factor which should divide the measure length. `L4` sets the 1/4 of the measure length, so you get the quarter note length. `L8` gives you the eighth note length. Your length definition stays effective until the next `L` command.

The below example plays some long, half notes followed by a string of fast 32nd notes:

    @1
    L2 CDE L32 GABAGFED


###Rests###

Rests can be easily inserted with a symbol `:`. The length of the rest is determined by the current note length.

    @1
    L4 C:CDE:EFG:G:E:::

The above example should show you how rests can be inserted intuitively.


###Octave Changes###

To specify the current octave range, use the `O` command followed by a number. The middle octave where the middle C sits is `O4`, and it's the default starting octave of each channel. The example below plays some notes in a high octave range `O6` then plays low notes in the octave `O2`.

    @1
    O5 GG:G:DE:C::: O2 C:::

Another way to navigate through octave ranges is to use the `<` and `>` symbols. `<` means go down by one octave, and `>` means go up by one octave. The example below sweeps up and down the entire possible octave ranges:

    @1
    O1 C>C>C>C>C>C>C>C<C<C<C<C<C<C<C


###Ties###

You can elongate a note quite easily by the multiple of the current note length. To do that, use the `~` symbol after the note you want to elongate. For example, try this example:

    @1
    L8 C~~DE~~CE~C~E~~~

You can be pretty liberal with chaining `~` symbols if you want, for example try `L8C~~~~~~~` :)


###Tuplets###

Tuplets are useful when you have to insert triplets or any other notes in non-standard subdivisions. Tuplets can be entered using the `[]` symbols. You simply enclose the notes you want to group in the space of the current note length:

    @1
    L4 C [EDC] D [FED] E [GFE]

You should have heard the notes enclosed in brackets as triplets in the space of a quarter note.

You can specify this grouping space, also. For example, if you want to write a quintuplets in a quarter note space in the midst of 8th notes:

    @1
    L8 C~~~ [4CDbC<B>C] >C<BbAbGFE


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


###Repeats###

If you want to repeat a phrase or a section of your music, you can use the `{}` symbols. Anything enclosed in these symbols are repeated.

    @1
    {EbDD:} EbDD: Bb~~~

You can actually nest a repeat inside another repeat. For example:

    @1
    {CDEF{GFEF}ED} {{{C~}}}

Note that nesting makes the total number of repeats grow exponentially. The latter part of the above example is repeated 2 x 2 x 2 = 8 times. 


## Drum Section in Detail ##

Writing `@D` starts the drum section.


###Drum Hits###

You can write a snare hit with `S`, and a kick with `K`. These normal hits come in UPPERCASE alphabets. With `:` giving a rest, a basic kick-and-snare groove can be written:

    @D
    K:S: KKS:

The hi-hat is written with `H`. A disco groove pattern can be played like this:

    @D
    K:H: S:H: K:H: S:HH


###Quieter Drum Hits###

If you write the drum hits in lower case, you get quieter drum hits. These can be added to give rich dynamics to your groove:

    @D
    K::k S::k :kK: S:HH K:hh S:kk :shh hsSS  


###Note Length###

Just as we did for a music channel, we can specify the note length as you go in the drum channel as well. Use the `L` command with a number ranging from 1 to 64, with the number being the subdivision factor of the measure. For example:

    @D
    L2 S L4 S L8 S L16 SS L32 SSSS L64 SSSSSSSS   

But you might want to set to the smallest subdivision once at the beginning and rely more on using rests and ties. You'll find this way much easier especially when working with groove-based music :)


###Rests###

A rest is indicated with a `:` as illustrated above. The length of a rest is determined by the currently set note length by the `L` command.


###Ties###

Instead of using a rest to fill a gap, you can use a tie symbol `~` to elongate a hit. The difference is that the tie ensures that the note gets played through the elongated space. When you hit and rest, if the note length is set very short, the abrupt cut-off of the hit might become obvious. Try this example to hear the difference:

    @D
    L64
    S::: S::: S::: S:::
    S~~~ S~~~ S~~~ S~~~


###Tuplets###

Tuplets make it possible to express quite complicated timings. Just as you saw in tuplets used in a music channel, you have two ways of expressing tuplets. The first way is to enclose hits simply with `[]` where the total length of grouped hits is the currently set note length. The second way is specifying the total length of grouped hits in the format `[n(hits)]`.

Using the both methods, let's illustrate some interesting things you can do to your groove with tuplets:

    @D
    L16
    K::[KK] S~~~ K::: [4SSS]
    K[KK]K[KK] KK[8HHHH] ss[8sss][8SSSS][8SSSSSSSS]
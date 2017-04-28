MXDRV MML (DOCUMENT IS WORK IN PROGRESS)
=========

This page is based on http://www16.atwiki.jp/mxdrv/pages/19.html, which is based on documentation from MXC, a MML compiler for the X68000. There are other compilers available as well, with more features, such as note.x (available [here](http://nfggames.com/x68000/Mirrors/x68pub/x68tools/SOUND/MXDRV/)).

Lines starting with A-H or P are considered to be MML data.
If a note length is preceded by a '%', the length is in clock ticks. Example: `c+%36`.
Note lengths can be connected using `^`. Example: f2 ^ 8 ^ 192.
Note length is up to 256 ticks. A quarter note is 48 ticks.

Commands
--------

Below, `#` represents a number.

`t#` Set the tempo, the number of quarter notes in one second.

`@t#` Set tempo by setting the value of OPM register B. Initial value `@t200`.

`@#` Set the current voice. Range is `@0` - `@255`. Initial value is undefined.

`o#` Set octave. Range is `o0` - `o8`. Initial value is `o4`.

`<` Transpose one octave below.

`>` Transpose one octave above.

`l#` Set note length. Default is `l4`.

`a` - `g` Note. Append a `+` to go up one semitone (sharp), and a `-` to go down a semitone (flat).
Notes are followed by a numerical value to specify length. Otherwise the value of `l#` is used.

`n#` Note by numeric value. Range is `n0` - `n95`. `n0` is `o0d+` and `n95` is `o8d`. In this case, sound length is separated with a `,`.

`r#` Rest. Length is the same as note length.

`q#` Staccato. Specify note duration, in eights of the full duration. Range is `q1` - `q8`. Default is `q8`.

`@q#` Staccato, with note duration specified as number of ticks.

`_` Portamento. If specified right after a note, the sound glides from one note to the next.

`&` Legato. Valid only right after a note, it disables key-off, therefore creating a legato effect with the next note.

`v#` Set volume. Range is `v0` - `v15`. Initial value `v8`.

`@v#` Fine volume. Range is `@v0` - `@v127`.

`(` Lower volume.

`)` Raise volume.

`p#` Pan. `p0` = No output, `p1` = Left only. `p2` = Right only. `p3` = Left and right. Default value `p3`.

`L` Specify loop start. After track ends, optionally loop from here (player will decide how many loops).

`D#` Detune in 1/64 semitones. Range is `D-32767` - `D32767`. Initial value is `D0`.

`[]#` Repeat a specified number of times. Range is `2` - `255`.

`/` Inside a repeat, during the last repetition, skip from here to the end of the repeat.

`y#1,#2` Write OPM register `#1` with value `#2`.

`k#` Delay this many ticks before playing note. Default is `k0`.

`w#` Set OPM noise frequency. Range is `w0` - `w31`.

`S#` Sync send. Resume playback on the specified channel, if it is in Sync wait mode. Values are A-H, P or 0-8.

`W` Sync wait. Wait for a Sync send command on another channel.

`MP#1,#2,#3` Set Pitch LFO values.
* `#1`: LFO waveform (0 = sawtooth, 1 = square, 2 = triangle)
* `#2`: 1/4 LFO cycle in ticks
* `#3`: LFO max amplitude

`MPON` Modulate Pitch ON. Enable pitch LFO.

`MPOF` Modulate Pitch OFf. Disable pitch LFO.

`MA#1,#2,#3` Set Amplitude LFO values.
* `#1` LFO waveform (0 = sawtooth, 1 = square, 2 = triangle)
* `#2` 1/4 LFO cycle in ticks
* `#3` LFO max amplitude

`MAON` Modulate Amplitude ON. Enable amplitude LFO.

`MAOF` Modulate Amplitude OFf. Disable amplitude LFO.

`MD#` Delay from key-on to the start of LFO modulation. Range is `MD0` - `MD255`. `MD0` means no delay. Does not apply to OPM LFO.

`MH#1,#2,#3,#4,#5,#6,#7` Set OPM LFO values.
* `#1` LFO waveform (0 = sawtooth, 1 = square, 2 = triangle, 3 = noise)
* `#2` LFRQ
* `#3` PMD
* `#4` AMD
* `#5` PMS
* `#6` AMS
* `#7` Key sync. `0` = asynchronous, `1` = synchronous.

`MHON` Enable OPM LFO.

`MHOF` Disable OPM LFO.

`!` Ignore all following commands on the current channel.

`F#` Change PCM frequency. Default is `F4`
* `F0`: 3.9kHz
* `F1`: 5.2kHz
* `F2`: 7.8kHz
* `F3`: 10.4kHz
* `F4`: 15.6kHz

Voice definition
----------------

If a line starts with @, it is the beginning of a voice definition. A voice can be defined at any point in the text.

```
@1 = {
	/* AR  DR  SR  RR  SL  OL  KS  ML DT1 DT2 AME */
       28,  4,  0,  5,  1, 37,  2,  1,  7,  0,  0,
       22,  9,  1,  2,  1, 47,  2, 12,  0,  0,  0,
       29,  4,  3,  6,  1, 37,  1,  3,  3,  0,  0,
       15,  7,  0,  5, 10,  0,  2,  1,  0,  0,  1,
	/* CON FL OP */
       2,  7, 15
}
```

Auxiliary commands
------------------

If a line starts with `#`, it is considered an auxiliary command.

* `#title "..."` Define title of the song.
* `#pcmfile "..."` Define a PDX file associated with this song. Extension is optional. File is searched in the same directory as resulting MDX file, or in a pre-defined PDX path.

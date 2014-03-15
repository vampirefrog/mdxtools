MXDRV MML (DOCUMENT IS WORK IN PROGRESS)
=========

This page is based on http://www16.atwiki.jp/mxdrv/pages/19.html, which is based on documentation from MXC, the MML compiler for the X68000.

Lines starting with A-H or P are considered to be MML data.
If a note length is preceded by a '%', the length is in clock ticks. Example: `c+%36`.
Note lengths can be connected using `^`. Example: f2 ^ 8 ^ 192.
Note length is up to 256 ticks. A quarter note is 48 ticks.

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
#1: LFO waveform (0 = sawtooth, 1 = square, 2 = triangle)
#2: 1/4 LFO cycle in ticks
#3: LFO max amplitude

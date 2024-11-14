MDX binary format
==================

MDX is a Music Macro Language format, with commands such as Note, Rest, Set Tempo and many others. It was used for storing music on the Sharp X68000 computer. There is a large library of MDX files available (see [README](../README.md) for some links).

WARNING: this document is work in progress. Since I've had to rely on Google Translate, some of the commands might be ambiguously translated. Wherever you see the question marks, there are uncertainties.

Info here is based on http://www16.atwiki.jp/mxdrv/pages/23.html

MDX files are composed of a header, the voices, and the MML commands.

Header format
-------------

* Title in Shift_JIS encoding, followed by 0x0d 0x0a 0x1a
* PDX file name, followed by 0x00 (just 0x00 if there is no PDX file)
* Word: Voice data offset
* Word[9 or 16] MML data offset

The voice and MML offsets are relative to the voice data offset position (right after the title and PDX file name). To implement this in code, just use a ftell() after you've read the PDX file name. The MML data offset array points to each of the channels. Some files use 9 channels, other use 16. To detect which is which, subtract the file position of the first MML data offset word from its value, then divide by 2. See [MDX.h](../MDX.h) for the implementation.

The FM channels are labeled A through H. The PCM channel is labeled P. The extra channels, when the Mercury expansion card is present, are labeled Q through W.

Voice format
------------

This corresponds to the OPM registers. Each field is 1 byte. The x4 fields are 4 bytes corresponding to M1, M2, C1, C2. Total is 27 bytes per voice.

<table align="center">
<tr>
<td width="60" align="center">7</td>
<td width="60" align="center">6</td>
<td width="60" align="center">5</td>
<td width="60" align="center">4</td>
<td width="60" align="center">3</td>
<td width="60" align="center">2</td>
<td width="60" align="center">1</td>
<td width="60" align="center">0</td>
</tr>
<tr>
<td colspan="8" align="center">Voice ID</td>
</tr>
<tr>
<td></td>
<td></td>
<td colspan="3" align="center">FL</td>
<td colspan="3" align="center">CON</td>
</tr>
<tr>
<td></td><td></td><td></td><td></td>
<td colspan="4" align="center">Slot Mask</td>
</tr>
<tr>
<td></td><td colspan="3" align="center">DT1</td><td colspan="4" align="center">MUL</td>
<td>×4</td>
</tr>
<tr>
<td></td><td colspan="7" align="center">TL</td>
<td>×4</td>
</tr>
<tr>
<td colspan="2" align="center">KS</td><td></td><td colspan="5" align="center">AR</td>
<td>×4</td>
</tr>
<tr>
<td><small>AME</small></td><td></td><td></td><td colspan="5" align="center">D1R</td>
<td>×4</td>
</tr>
<tr>
<td colspan="2" align="center">DT2</td><td></td><td colspan="5" align="center">D2R</td>
<td>×4</td>
</tr>
<tr>
<td colspan="4" align="center">D1L</td><td colspan="4" align="center">RR</td>
<td>×4</td>
</tr>
</table>

Unused bits are set to 0. Set bits 6 and 7 of [FL & CON] to 0.

MDX Commands
------------

Each command consists of one byte, followed by specific parameters. Integers are in big endian format.

<table>
<tr><th width="25%" style="width: 25%">Command</th><th>MML</th><th>Description</th></tr>
<tr><td><code>0x00-0x7f</code></td><td><code>r</code></td><td>Rest +1 clock cycles.</td></tr>
<tr><td><code>0x80-0xdf</code> <code>n</code></td><td><code>n#,#</code></td><td>Note data, followed by duration <code>n</code> + 1. <code>0x80</code> corresponds to MML <b>o0d+</b>, which means octave 0, note D♯. <code>0xdf</code> corresponds to <b>o8d</b>, octave 8, D. To calculate the MIDI equivalent note, subtract <code>0x80</code> and add <code>3</code>. For the PCM channels, P through W, this represents the sample index, the first sample is <code>0x80</code>, second sample is <code>0x81</code> and so on.</td></tr>
<tr><td><code>0xff</code> <code>n</code></td><td><code>@t#</code></td><td>Set tempo to <code>n</code>. Tempo applies to all tracks at the same time, not just current track.</td></tr>
<tr><td><code>0xfe</code> <code>n</code> <code>m</code></td><td><code>y#1,#2</code></td><td>Set OPM register <code>n</code> to value <code>m</code>.</td></tr>
<tr><td><code>0xfd</code> <code>n</code></td><td><code>@#</code></td><td>Set current voice.</td></tr>
<tr><td><code>0xfc</code> <code>n</code></td><td><code>p#</code></td><td>Pan.</td></tr>
<tr><td><code>0xfb</code> <code>n</code></td><td><code>@v#</code></td><td>Set volume.</td></tr>
<tr><td><code>0xfa</code></td><td><code>(</code></td><td>Decrease volume.</td></tr>
<tr><td><code>0xf9</code></td><td><code>)</code></td><td>Increase volume.</td></tr>
<tr><td><code>0xf8</code> <code>n</code></td><td><code>q#</code></td><td>Set note length (staccato).</td></tr>
<tr><td><code>0xf7</code></td><td><code>&amp;</code></td><td>Legato. Disable key off for next note. Corresponds to MML command <code>&amp;</code>, except it precedes the note instead of following it.</td></tr>
<tr><td><code>0xf6</code> <code>n</code> <code>0x00</code></td><td><code>[</code></td><td>Repeat <code>n</code> times until a Repeat end command. Corresponds to MML command <code>[</code>, but loop multiplier is specified here instead of the closing <code>]</code>.</td></tr>
<tr><td><code>0xf5</code> <code>nn</code></td><td><code>]</code></td><td>Repeat end. Loop back <code>nn</code> (signed word) bytes.</td></tr>
<tr><td><code>0xf4</code> <code>nn</code></td><td><code>/</code></td><td>Repeat escape. In the last repetition, skip <code>nn</code> bytes, thus shortening it. Loosely corresponds to MML command <code>/</code>, inside <code>[]</code>.</td></tr>
<tr><td><code>0xf3</code> <code>nn</code></td><td><code>D#</code></td><td>Detune <code>nn</code>/64 semitones.</td></tr>
<tr><td><code>0xf2</code> <code>nn</code></td><td><code>_</code></td><td>Portamento. Change pitch <code>nn</code>/16384 semitones per clock. This is issued before the starting note.</td></tr>
<tr><td><code>0xf1</code> <code>0x00</code></td><td><code>!</code>(?)</td><td>Performance end.</td></tr>
<tr><td><code>0xf1</code> <code>nn</code></td><td><code>L</code></td><td>Performance end, but loop from <code>nn</code> bytes (<code>nn</code> is a negative int16).</td></tr>
<tr><td><code>0xf0</code> <code>n</code></td><td><code>k#</code></td><td>Delay key-on <code>n</code> ticks.</td></tr>
<tr><td><code>0xef</code> <code>n</code></td><td><code>S#</code></td><td>Sync send on channel <code>n</code>. If channel <code>n</code> is in Sync Wait, resume playback on that channel.</td></tr>
<tr><td><code>0xee</code></td><td><code>W</code></td><td>Sync Wait on current channel. Pause playback on this channel until resumed by a Sync Send signal (see above).</td></tr>
<tr><td><code>0xed</code> <code>n</code></td><td><code>F#</code></td><td>ADPCM / noise frequency set to <code>n</code>. For FM channels (A-H), 0 means disable noise, 128+ means set noise frequency (without bit 7). For ADPCM channels (P-W), sets sampling frequency.</td></tr>
<tr><td><code>0xec</code> <code>0x80</code></td><td><code>MPOF</code></td><td>Pitch LFO disable.</td></tr>
<tr><td><code>0xec</code> <code>0x81</code></td><td><code>MPON</code></td><td>Pitch LFO enable.</td></tr>
<tr><td><code>0xec</code> <code>m</code> <code>nn</code> <code>aa</code></td><td><code>MP#1,#2,#3</code></td><td>LFO Pitch control. <code>m</code> controls the waveform (0=sawtooth, 1=square, 2=triangle), <code>nn</code> is the frequency, expressed in clock cycles / 4, and <code>aa</code> is the amplitude.</td></tr>
<tr><td><code>0xeb</code> <code>0x80</code></td><td><code>MAOF</code></td><td>LFO Amplitude Modulation disable.</td></tr>
<tr><td><code>0xeb</code> <code>0x81</code></td><td><code>MAON</code></td><td>LFO Amplitude Modulation enable.</td></tr>
<tr><td><code>0xeb</code> <code>m</code> <code>nn</code> <code>aa</code></td><td><code>MA#1,#2,#3</code></td><td>LFO Volume control. Same parameters as LFO Pitch.</td></tr>
<tr><td><code>0xea</code> <code>0x80</code></td><td><code>MHOF</code></td><td>OPM LFO Off.</td></tr>
<tr><td><code>0xea</code> <code>0x81</code></td><td><code>MHON</code></td><td>OPM LFO On.</td></tr>
<tr><td><code>0xea</code> <code>m</code> <code>n</code> <code>o</code> <code>p</code> <code>q</code></td><td><code>MH#1,#2,#3,#4,#5,#6,#7</code></td><td>Set OPM LFO values <code>m</code>=sync/wave, <code>n</code>=lfrq, <code>o</code>=PMD, <code>p</code>=AMD, <code>q</code>=PMS/AMS.</td></tr>
<tr><td><code>0xe9</code> <code>n</code></td><td><code>MD#</code></td><td>Set LFO key-on delay.</td></tr>
<tr><td><code>0xe8</code></td><td></td><td>Enable PCM8 mode (PCM channels <code>Q</code>-<code>W</code>).</td></tr>
<tr><td><code>0xe7</code> <code>0x01</code> <code>n</code></td><td></td><td>Fade-out at speed <code>n</code>.</td></tr>
</table>


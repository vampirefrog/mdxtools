mdxtools
========

A bunch of tools for handling the MDX music format (music for the Sharp X68000)

The rest of this document is work in progress.

Download music library
----------------------

* See zip file near end of page - http://www.gamesx.com/wiki/doku.php?id=x68000:sxsi_disk_image_with_games_and_lots_of_mdx_files
* Also this (not sure if same collection) http://nfggames.com/forum2/index.php?topic=4813.0

Ways to listen to MDX files
---------------------------

Windows:

* Winamp plugin (source available): http://www.vesta.dti.ne.jp/~tsato/soft_sound.html
* Foobar2000 plugin: http://www.vesta.dti.ne.jp/~tsato/soft_fb2k.html
* Hoot: http://dmpsoft.s17.xrea.com/hoot/index.html
* MXV: http://gorry.haun.org/mx/index_e.html
* MDXWin: http://www.pat.hi-ho.ne.jp/sata68/mdxwin.html#mdxwin
* KbMedia Player: http://hwm5.gyao.ne.jp/kobarin/

Unix:

* MDXPlay: http://homepage3.nifty.com/StudioBreeze/software/mdxplay-e.html

X68000:

* MMDSP (runs on the X68000 or an emulator): http://www.gamesx.com/wiki/doku.php?id=x68000:sxsi_disk_image_with_games_and_lots_of_mdx_files

About MML (Music Macro Language)
--------------------------------
MML is a text format, containing sound commands similar to MDX and to MIDI as well.

* MML resources & compilers: http://woolyss.com/chipmusic-mml.php
* MML description for MXDRV (japanese): http://www16.atwiki.jp/mxdrv/pages/19.html

Other tools and resources
-------------------------

* mdxpg (convert to MIDI): http://www.geocities.jp/sam_kb/VOPM/MDXPG/index.html
* nfggames x68000 file archive: http://nfggames.com/x68000/
* especially the tools here (see MDX_TOOL.lzh) http://nfggames.com/x68000/Mirrors/x68pub/x68tools/SOUND/MXDRV/

MDX file format
---------------

MDX is a Music Macro Language format, with commands such as Note, Rest, Set Tempo and many others. It was used for storing music on the Sharp X68000 computer. There is a large library of MDX files available.

WARNING: this document is work in progress. Since I've had to rely on Google Translate, some of the commands might be ambiguously translated. Wherever you see the question marks, there are uncertainties.

Info here is based on http://www16.atwiki.jp/mxdrv/pages/23.html

MDX files are composed of a header, the voices, and the MML commands.

Header format:

* Title in Shift_JIS encoding, followed by 0x0d 0x0a 0x10
* PDX file name, followed by 0x00 (just 0x00 if there is no PDX file)
* Word: Voice data offset
* Word[9 or 16] MML data offset

The voice and MML offsets are relative to the voice data offset position (right after the title and PDX file name). To implement this in code, just use a ftell() after you've read the PDX file name. The MML data offset array points to each of the channels. Some files use 9 channels, other use 16. To detect which is which, subtract the file position of the first MML data offset word from its value, then divide by 2. See MDX.h for the implementation.

Voice format. This corresponds to the registers in the YM2151 sound chip. Each field is 1 byte. The x4 fields are 4 bytes corresponding to M1, M2, C1, C2.

* Voice ID
* FL & CON
* Slot Mask
* DT1 & MUL x 4
* TL x 4
* KS & AR x 4
* AME & D1R x 4
* DT2 & D2R x 4
* D1R & RR x 4

MML Commands. Each command consists of one byte, followed by specific parameters.

* `0x00-0x7f` • Rest +1 clock cycles
* `0x80-0xdf` `n` • Note data, followed by duration `n` + 1. To calculate the MIDI equivalent note, subtract 0x80 and add 3.
* `0xff` `n` • Set tempo to `n`.
* `0xfe` `n` `m` • Set OPM register `n` to value `m`.
* `0xfd` `n` • Set current voice.
* `0xfc` `n` • Set output phase.
* `0xfb` `n` • Set volume.
* `0xfa` • Decrease volume.
* `0xf9` • Increase volume.
* `0xf8` `n` • Sound length
* `0xf7` • Disable key off.
* `0xf6` `n` `0x00` • Repeat `n` times until a Repeat end command.
* `0xf5` `nn` • Repeat end. Loop back `nn` (signed word) bytes.
* `0xf4` `nn` • Repeat escape. In the last repetition, skip `nn` bytes, thus shortening it.
* `0xf3` `nn` • Detune `nn`/64 semitones.
* `0xf2` `nn` • Portamento semitones/16384. (???)
* `0xf1` `0x00` • Performance end.
* `0xf1` `nn` • Loop from nn. (???)
* `0xf0` `n` • Key-on delay. (???)
* `0xef` `n` • Sync send on channel `n`. (???)
* `0xee` • Sync wait. (???)
* `0xed` `n` • ADPCM / noise frequency set to `n`. (???)
* `0xec` `0x80` • LFO Pitch MPOF.
* `0xec` `0x81` • LFO Pitch MPON.
* `0xec` `m` `nn` `aa` • LFO Pitch control. `m` controls the waveform (0=sawtooth, 1=square, 2=triangle), `nn` is the frequency, expressed in clock cycles / 4, and `aa` is the amplitude.
* `0xeb` `0x80` • LFO Volume MAOF.
* `0xeb` `0x81` • LFO Volume MAON.
* `0xeb` `m` • LFO Volume control. Same as LFO Pitch.
* `0xea` `0x80` • LFO OPM MHOF.
* `0xea` `0x81` • LFO OPM MHON.
* `0xea` `m` `n` `o` `p` `q` • OPM LFO `m`: sync/wave, `n`: lfrq, `o`: pmd, `p`: amd, `q`: pms/ams 
* `0xe9` `n` • Set LFO delay.
* `0xe8` • PCM8 expansion/ mode shift. (???)
* `0xe7` `0x01` `n` • Fade-out at speed `n`.

About the Sharp x68000
----------------------

It is a desktop computer released by Sharp in 1987 ([see wikipedia page](http://en.wikipedia.org/wiki/Sharp_X68000)). It contains two sound chips, the YM2151 (8 FM channels) and OKI MSM6258 (1 4-bit PCM). There is an expansion card called Mercury unit that can increase the PCM channels to 8.

mdxtools
========

A bunch of tools for handling the MDX music format (music for the Sharp X68000)

- rest of this document is work in progress -

About the file format
---------------------

MDX is a Music Macro Language format, with commands such as Note, Rest, Set Tempo and many others. It was used for storing music on the Sharp X68000 computer. There is a large library of MDX files available.

Download music library
----------------------

See zip file near end of page - http://www.gamesx.com/wiki/doku.php?id=x68000:sxsi_disk_image_with_games_and_lots_of_mdx_files
Also this (not sure if same collection) http://nfggames.com/forum2/index.php?topic=4813.0

Ways to listen to MDX files
---------------------------

Winamp plugin (source available): http://www.vesta.dti.ne.jp/~tsato/soft_sound.html
Foobar2000 plugin: http://www.vesta.dti.ne.jp/~tsato/soft_fb2k.html
Hoot: http://dmpsoft.s17.xrea.com/hoot/index.html
MXV: http://gorry.haun.org/mx/index_e.html
MDXWin: http://www.pat.hi-ho.ne.jp/sata68/mdxwin.html#mdxwin
MMDSP (runs on the X68000 or an emulator): http://www.gamesx.com/wiki/doku.php?id=x68000:sxsi_disk_image_with_games_and_lots_of_mdx_files


File format
-----------

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
0x00 - 0x7f - Rest +1 clock cycles
0x80 - 0xdf - Note data (??????????)
0xff tempo.b - Set Tempo
0xfe register.b data.b - Set OPM register
0xfd voice.b - Set current voice
0xfc phase.b - Set output phase (?????????)
0xfb volume.b - Set volume
0xfa - Decrease volume
0xf9 - Increase volume
0xf8 length.b - Sound length (?????????)
0xf7
0xf6
0xf5
0xf4
0xf3
0xf2
0xf1
0xf0
0xef
0xee
0xed
0xec
0xeb
0xea
0xe9
0xe8
0xe7



About the Sharp x68000
----------------------

It is a desktop computer released by Sharp in 1987 ([see wikipedia page](http://en.wikipedia.org/wiki/Sharp_X68000)). It contains two sound chips, the YM2151 (8 FM channels) and OKI MSM6258 (1 4-bit PCM). There is an expansion card called Mercury unit that can increase the PCM channels to 8.

mdxtools
========

A bunch of tools for handling the MDX music format (music for the Sharp X68000 using the MXDRV sound driver).

These tools are aimed at the large collections of MDX files available online (see below for download links). They are meant to convert to other formats, such as vgm, midi and for the curious, MML and dumping the raw MDX data.

* mdx2vgm (WIP) - convert to VGM format
* mdxdump - dump all the information in a MDX file.
* mdxstat - analyze and print out some statistics and totals about given MDX files.
* mdx2mml - a MML decompiler
* mdx2midi (WIP) - a MML to MIDI file converter. Use in conjunction with pdx2sf2 to hear the samples too.
* mididump - Dump all the info and events in a .mid file. Useful for development. You can also use MidiPiano for a GUI version.
* mdx2opm (WIP) - Extract OPM voices from MDX file (usable in [VOPM](http://www.geocities.jp/sam_kb/VOPM/)).
* pdx2wav - extract the samples from a PDX file, into 16-bit WAV files.
* pdx2sf2 - generate a SoundFont file with the samples from the PDX file. Useful for importing a song into a DAW.

The rest of this document is work in progress.

Download music library
----------------------

* Neko's collection with >28k songs http://nfggames.com/X68000/Music/MXDRV_Neko68k.rar
* Another collection (to open, use [DiskExplorer](http://hp.vector.co.jp/authors/VA013937/editdisk/index_e.html)) http://nfggames.com/forum2/index.php?topic=4813.0

Ways to listen to MDX files
---------------------------

Windows:

* Winamp plugin (source available): http://www.vesta.dti.ne.jp/~tsato/soft_sound.html
* Foobar2000 plugin: http://www.vesta.dti.ne.jp/~tsato/soft_fb2k.html
* Hoot: http://dmpsoft.s17.xrea.com/hoot/index.html
* MXV, MXDRVg.dll, X68Sound.dll (sources available): http://gorry.haun.org/mx/index_e.html
* MDXWin: http://www.pat.hi-ho.ne.jp/sata68/mdxwin.html#mdxwin
* KbMedia Player: http://hwm5.gyao.ne.jp/kobarin/

Unix:

* MDXPlay (command line player): http://homepage3.nifty.com/StudioBreeze/software/mdxplay-e.html

X68000:

* MMDSP (runs on the X68000 or an emulator): http://www.gamesx.com/wiki/doku.php?id=x68000:sxsi_disk_image_with_games_and_lots_of_mdx_files

About MML (Music Macro Language)
--------------------------------
MML is a text format, containing sound commands similar to MDX and to MIDI as well.

* MML resources & compilers: http://woolyss.com/chipmusic-mml.php
* MML description for MXDRV (japanese): http://www16.atwiki.jp/mxdrv/pages/19.html
* Another MML description: http://www.geocities.co.jp/Playtown-Denei/9628/mck/mckc-e.txt
* NRTDRV MML description: http://nrtdrv.sakura.ne.jp/index.cgi?page=MML%A5%B3%A5%DE%A5%F3%A5%C9%B0%EC%CD%F7

Compiling MML files to MDX files: Get run68 from http://sourceforge.jp/projects/sfnet_run68/ , then get MDX_TOOL.lzh from http://nfggames.com/x68000/Mirrors/x68pub/x68tools/SOUND/MXDRV/ . Unpack the lzh file in the same folder as run68, then open a command prompt, navigate to the run68 folder, and run `run68.exe mxc.x <file.mml>` .

You can find other ways of compiling on this page: http://gimic.jp/index.php?MDX%E3%83%87%E3%83%BC%E3%82%BF%E4%BD%9C%E6%88%90

Decompiling: Similar procedure, but get DMDX from DMDX.lzh.


Other tools and resources
-------------------------

* mdxpg (convert to MIDI): http://www.geocities.jp/sam_kb/VOPM/MDXPG/index.html
* nfggames x68000 file archive: http://nfggames.com/x68000/
* especially the tools here (see MDX_TOOL.lzh) http://nfggames.com/x68000/Mirrors/x68pub/x68tools/SOUND/MXDRV/

File formats
------------

* [MML](docs/MML.md)
* [MDX](docs/MDX.md)
* [PDX](docs/PDX.md)


About the Sharp x68000
----------------------

It is a desktop computer released by Sharp in 1987 ([see wikipedia page](http://en.wikipedia.org/wiki/Sharp_X68000)). It contains two sound chips, the YM2151 (8 FM channels) and OKI MSM6258 (1 4-bit PCM). There is an expansion card called Mercury unit that can increase the PCM channels to 8.

Emulators
---------

* [XM6 2.06](http://yohkai.no-ip.info/x680x0/XM6.htm) (source available for 2.05)
* [XM6 Pro-68k](http://mijet.eludevisibility.org/XM6%20Pro-68k/XM6%20Pro-68k.html)
* [XM6 TypeG](http://www.geocities.jp/kugimoto0715/xm6g_win32.html)
* [XM6i](http://xm6i.org/)
* [WinX68k](http://www.geocities.jp/winx68khighspeed/) (source available on the [XM6 Pro-68k page](http://mijet.eludevisibility.org/XM6%20Pro-68k/XM6%20Pro-68k.html))
* [Virtual X68000](http://www.vx68k.org/vx68k/) (linux)
* [More emulators at Zophar's](http://www.zophar.net/x68000.html)

Licensing
---------

Please see the LICENSE file for more information.

Contributing
------------

If you would like to improve or contribute to this project, I highly encourage you to submit pull requests instead of maintaining your own fork. This way, it is less confusing for users. I will do my best to include your patches.

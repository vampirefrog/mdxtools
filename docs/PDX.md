PDX file format
===============

File starts with 96 32-bit unsigned integer pairs, each pair containing offset and length of ADPCM data chunks for each note value. To play a particular sample, simply play the note with the corresponding value on channels `P` - `W`.

The ADPCM data is 4-bit. See PDX.h for decoding code.

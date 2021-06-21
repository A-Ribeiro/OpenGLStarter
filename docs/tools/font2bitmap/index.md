# OpenGLStarter

[Back to HOME](../../index)

## Font tool: font2bitmap - OpenGL Font 2.0

The command line tool __font2bitmap__ now generates the new format(Binary ASilva OpenGL Font, version 2.0 - basof2) and the old at the same time.

The input is any Freetype2 supported font format (.ttf, .otf, etc...).

The parameters to the command line tool:

* __-i:__ input file(.ttf, .otf, freetype compatible font)
* __-f:__ face in font to be selected (default: 0)
* __-o:__ output file without extension
* __-s:__ character size in pixels to be exported (minimum:14).
* __-c:__ text file with all characters to be generated in utf8.
* __-p:__ size in pixels of the space between the characters exported.
* __-t:__ outline thickness real number.

Example:

```bash
# Generates a font file called output-20.basof2
# parameters: -s 20 the square pixel size is 20 pixels.
#             -c charset.utf8 the input character list utf-8 file
#             -p 6 the minimum distance in pixels from each 
#                  character from another in the exported atlas.
#             -t 0.33333 the thickness of the outline
font2bitmap -i "input.ttf" -o "output" -s 20 -c charset.utf8 -p 6 -t 0.33333
```


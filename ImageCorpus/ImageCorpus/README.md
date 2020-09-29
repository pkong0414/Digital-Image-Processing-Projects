Image Corpus Program

The goal of this project is to make an program that will take in an input directory and adjust images to parameters
specified in the command line of the program's invocation.

To make the executable:
$make

To clean up:
$make clean

./corpus [-help] [ -a -g -r=rows -c=cols -t=filetype @indir @outdir]

NOTE: @ indicates positional parameters and must be entered with input directory first and output directory second.

"{ help h usage ?	|       | print this message															}"
"{ @indir			|<none>	| Input Directory.																}"
"{ @outdir			|		| Output Directory.																}"
"{ a aspect			|		| if specified preserves the aspect ratio of the images							}"
"{ g gray			|		| Saves the output images as grayscale [default:saves as input]					}"
"{ r rows			|	480	| Maximum number of rows in the output image [default: 480]						}"
"{ c cols			|	640	| Maximum number of columns in the output [default: 640]						}"
"{ t type			|<none>	| Output img type ( jpg, tif, bmp, or png) [default original file is retained]  }"

NOTE: Also in DEBUG mode it turns out that boolean parameters at the commandline are always true (at least it's what it
seems to me since I spent hours wondering why can't I make the parameter false).

If you have any idea please let me know.
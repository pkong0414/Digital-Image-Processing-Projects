// ImageCorpus.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <Windows.h>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/core/utility.hpp>
#include <iostream>
#include <string>
#include <sstream>
#include <cstdlib>
#include <time.h>
#include <dirent.h>
#include <sys/stat.h>
#include <vector>
#include <list>

#define MAX_PATH 255

int depthfirstapply(char* path, std::list <std::string>& fileList);

void upScaleImage(std::string path);
void downScaleImage(std::string path);

int main(int argc, char** const argv)
{

	//debugging output for command line parsing

	for (int c = 0; c < argc; c++) {
		printf("argv[ %d ]: %s\n", c, argv[c]);
	}

	//row and column parameters
	int rParam;
	int cParam;
	bool aspectFlag;
	bool grayscaleFlag;
	int key = 0;
	std::string currentImage;

	const cv::String keys =
	{
		"{help h usage ?	|       | print this message															}"
		"{@directory		|<none> | Directory that contains the pictures to browse								}"
		"{@indir			|		| Input Directory.																}"
		"{@outdir			|		| Output Directory.																}"
		"{aspect a			|	    | if specified preserves the aspect ratio of the images							}"
		"{gray g			|		| Saves the output images as grayscale [default:saves as input]					}"
		"{rows r			|	480	| Maximum number of rows in the output image [default: 480]						}"
		"{cols c			|	640	| Maximum number of columns in the output [default: 640]						}"
		"{type t			|		| Output img type ( jpg, tif, bmp, or png) [default: original file is retained] }"
	};

	//work on this later
	//parsing the argument on the commandline
	cv::CommandLineParser parser(argc, argv, keys);
	if (parser.has("help"))
	{
		parser.about("\nThis is an image corpus program.\n");
		parser.printMessage();
		return 0;
	}

	rParam = parser.get<int>("rows");
	cParam = parser.get<int>("cols");

	printf("row: %d\n", rParam);
	printf("col: %d\n", cParam);

	cv::String fileName = parser.get<cv::String>(0);
	std::cout << "fileName: " << fileName << std::endl;

	char* filePath = new char[sizeof(fileName)];
	strcpy_s(filePath, sizeof(fileName), fileName.c_str());
	printf("filePath: %s\n", filePath);
	if (!parser.check())
	{
		parser.printErrors();
		return 0;
	}
}

int depthfirstapply(char* path, std::list <std::string>& fileList) {
	/*	int depthfirstapply(char *path, int pathfun(char *path1));
	 *
	 *    The depthfirstapply function traverses the tree, starting at path. It applies the pathfun function to each file
	 *    that it encounters in the traversal. The depthfirstapply function returns the sum of the positive return values
	 *    of pathfun, or -1 if it failed to traverse any subdirectory of the directory. An example of possible pathfun is
	 *    the sizepathfun function specified in the next part.
	 */

	 //This will work but will need to add a little bit in order to do the traversals!
	 //printf( "depthCount: %d\n", depthCount );
	struct dirent* direntp;
	DIR* dirp;
	struct stat statbuf;
	char newPath[PATH_MAX];
	int key;

	//strcat( path, "/" );

	if ((dirp = opendir(path)) == NULL) {
		perror("Failed to open directory.");
		return 1;
	}

	while ((direntp = readdir(dirp)) != NULL) {
		if ((strcmp(direntp->d_name, ".") != 0) && (strcmp(direntp->d_name, "..") != 0)) {
			//printf("dirent->d_name: %s\n", path);
			sprintf_s(newPath, "%s\\%s", path, direntp->d_name);
			//printf("newPath: %s\n", newPath);

			if (stat(newPath, &statbuf) == -1) {
				perror("File Stat Error!\n");
				return 1;
			}
			else {
				if (S_ISREG(statbuf.st_mode) == 1) {
					//printf("%s is a file.\n", newPath);

					fileList.push_back(newPath);

					// Save a copy of grayscale image in a file on disk.

					/*
					std::string gray_pic_file = newPath;
					gray_pic_file.insert(gray_pic_file.find_last_of('.'), "_gray");
					cv::imwrite(gray_pic_file, img_gray);
					*/

				}
				if (S_ISDIR(statbuf.st_mode) == 1) {
					//printf("%s is a directory.\n", newPath);
					depthfirstapply(newPath, fileList);
				}
			}
		}
	}
	while ((closedir(dirp) == -1) && (errno == EINTR));


	return 0;
}

void upScaleImage(std::string path) {


}

void downScaleImage(std::string path) {
	//downscaling images a shrinking of the original images and returning it back to its original dimensions

}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file

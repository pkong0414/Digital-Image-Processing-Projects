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

//we'll probably take the image resizing from assignment1
void displayResizeImg(std::string path, int oldWidth, int oldHeight, int newCols, int newRows, std::string fileType, bool aspectFlag, bool grayscaleFlag);

void downScaleImage(std::string path);

//global variables
static std::string outDir;

int main(int argc, char** const argv)
{

	//debugging output for command line parsing

	for (int c = 0; c < argc; c++) {
		printf("argv[ %d ]: %s\n", c, argv[c]);
	}

	//row and column parameters
	int rParam;
	int cParam;
	bool aspectFlag = false;
	bool grayscaleFlag = false;
	std::string fileType;
	int key = 0;

	std::string currentImage;

	const cv::String keys =
	{
		"{help h usage ?	|       | print this message															}"
		"{@indir			|<none>	| Input Directory.																}"
		"{@outdir			|		| Output Directory.																}"
		"{aspect a			|	    | if specified preserves the aspect ratio of the images							}"
		"{gray g			|		| Saves the output images as grayscale [default:saves as input]					}"
		"{rows r			|	480	| Maximum number of rows in the output image [default: 480]						}"
		"{cols c			|	640	| Maximum number of columns in the output [default: 640]						}"
		"{@type t			|<none>	| Output img type ( jpg, tif, bmp, or png) [0: default original file is retained] }"
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
	if (parser.has("gray")) {
		grayscaleFlag = true;
	}
	if (parser.has("aspect")) {
		aspectFlag = true;
	}
	if (parser.has("type")) {
		fileType = parser.get<cv::String>(2);
	}
	outDir = parser.get<std::string>(1);

	cv::String fileName = parser.get<cv::String>(0);
	std::cout << "fileName: " << fileName << std::endl;

	char* filePath = new char[sizeof(fileName)];
	strcpy_s(filePath, sizeof(fileName), fileName.c_str());
	printf("filePath: %s\n", filePath);

	printf("row: %d\n", rParam);
	printf("col: %d\n", cParam);

	if (!parser.check())
	{
		parser.printErrors();
		return 0;
	}



	try
	{
		std::list <std::string> fileResult;

		//debugging parameter argument output
		/*
		for (int c = 0; c < argc; c++) {
			std::cout << "argv[" << c << "]: " << argv[c] << std::endl;
		}
		*/

		if (argc >= 2)
		{
			//std::cerr << "usage: " << argv[0] << " image_file" << std::endl;
			depthfirstapply(filePath, fileResult);
		}

		//testing output
		// Vector with std::list
		std::vector < std::string > vecOfFile(fileResult.begin(), fileResult.end());
		std::vector<std::string>::iterator move;
		//testing output


		move = vecOfFile.begin();
		while (key != 'q') {
			//case to go next image

			// NOTE: Consider that there may not be a file that is not an image.

			// Read the image
			cv::Mat image = cv::imread(*move);

			// Make sure that the image is read properly.
			if (image.empty()) {
				//erase the file off of the vector instead of throwing error.
				printf("Cannot open input image is not a file.\n");
				move++;
			}

			// Read the same image as grayscale image.
			//cv::Mat img_gray = cv::imread(*move, cv::IMREAD_GRAYSCALE);

			// Display color image
			cv::imshow(*move, image);

			// filepath name
			std::cout << "filename: " << *move << std::endl;
			// Image dimensions
			std::cout << "Image size is: " << image.cols << "x" << image.rows << std::endl;
			// Image pixel size
			std::cout << "Pixel size: " << image.cols * image.rows << std::endl;

			//display Resize Image function
			displayResizeImg(*move, image.cols, image.rows, cParam, rParam, fileType, aspectFlag, grayscaleFlag);

			// Display grayscale image
			//cv::imshow("Grayscale Rendering", img_gray);

			//uchar pxl_gray = img_gray.at<uchar>(r, c);
			//std::cout << "Gray scale pixel at (" << r << "," << c << ") = " << (int)pxl_gray << std::endl;

			//keypress wait
			key = cv::waitKeyEx(0);
			cv::destroyAllWindows();

			if (key == ' ' || key == 'n') {
				if (move != vecOfFile.end()) {
					++move;
				}
				if (move == vecOfFile.end()) {
					return (0);
				}
			}
			//case to go to previous image
			if (key == 'p') {
				if (move != vecOfFile.begin()) {
					move--;
				}
				else {
					return (0);

				}
			}
		}
	}
	catch (std::string& str)
	{
		std::cerr << "Error: " << argv[0] << ": " << str << std::endl;
		return (1);
	}
	catch (cv::Exception& e)
	{
		std::cerr << "Error: " << argv[0] << ": " << e.msg << std::endl;
		return (1);
	}

	return (0);
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

void displayResizeImg(std::string path, int oldWidth, int oldHeight, int newCols, int newRows, std::string fileType, bool aspectFlag, bool grayscaleFlag) {
	//we are going to use this function to detect aspect ratio

	cv::Mat image = cv::imread(path);
	double aspectRatio;
	//screen limitations
	//we'll be using int GetSystemMetrics( code ) to get the screen of the primary monitor;
	//I found Max screen size to be annoying so I'm using CXFULLSCREEN and CYFULLSCREEN instead
	//I only need Primary Monitor so these will do the job: SM_CXFULLCREEN ( code: 16 ) / SM_CYFULLSCREEN ( code: 17 ).
	int ColumnLimit = GetSystemMetrics(16);
	int ColumnDefault = 1080;
	int RowLimit = GetSystemMetrics(17);
	int RowDefault = 720;
	int newHeight = 0;
	int newWidth = 0;

	//we have two ways:
	// to find newHeight: newHeight = ( oldHeight / oldWidth ) * newWidth
	// to find newWidth: newWidth = newHeight / ( oldHeight / oldWidth )

	//there are cases to consider:
		//PRIORITY: WE MUST MAINTAIN ASPECT RATIO!
		//PRIORITY: WE ALSO MUST KEEP WITHIN SCREEN SIZE
			//1. The normal condition. Assuming both the image and the user entered parameters that does not exceed screen size.
			//2. The image is greater than the screen allows and must be set to to the proposed parameters.
				//2.a. The proposed parameters is smaller than the screen and therefore will proceed accordingly
				//2.b. The proposed parameters is greater than the screen and will be set to MAXIMUM space allowed, while respecting aspect ratio.
			//3. The image is acceptable to screensize, but the parameters exceed that of the screen allowance.

	//This is case 2
	if (aspectFlag == true) {
		if (oldWidth > ColumnLimit || oldHeight > RowLimit) {
			if (oldWidth > ColumnLimit && oldHeight > RowLimit) {
				//In this case we have an image where both Columns and Rows exceed screensize.
				if (oldWidth > oldHeight) {
					//if original image Column is bigger than the image Height, we'll base the new dimensions on columns first!
					if (newCols <= ColumnLimit) {
						//case 2.a
						//User parameters did not exceed our screensize.
						newWidth = newCols;
						newHeight = ((double)oldHeight / (double)oldWidth) * newWidth;
					}
					else
					{
						//case 2.b
						//User parameters exceed screensize.
						newWidth = ColumnLimit;
						newHeight = ((double)oldHeight / (double)oldWidth) * newWidth;
					}
				}
				else {
					//if original image Row is bigger than the image Column, we'll base the new dimensions on row first!
					if (newRows <= RowLimit) {
						//User parameter did not exceed our screensize.
						newHeight = newRows;
						newWidth = newHeight / ((double)oldHeight / (double)oldWidth);
					}
					else
					{
						//User parameter exceed screensize.
						newHeight = RowLimit;
						newWidth = newHeight / ((double)oldHeight / (double)oldWidth);
					}
				}
			}
			else if (oldWidth > ColumnLimit && oldHeight < RowLimit) {
				//In this case we have Columns that exceed screensize, we must prioritize Columns as the default newWidth

				if (newCols > ColumnLimit) {
					//Assuming the user input exceeds screensize.
					newWidth = ColumnLimit;
					newHeight = ((double)oldHeight / (double)oldWidth) * newWidth;
				}
				else {
					//Assuming the user input does not exceed screensize.
					newWidth = newCols;
					newHeight = newHeight = ((double)oldHeight / (double)oldWidth) * newWidth;
				}
			}
			else if (oldHeight > RowLimit && oldWidth < ColumnLimit) {
				//In this case we have Rows that exceed screensize, we must prioritize Rows as the default newHeight
				if (newRows > RowLimit) {
					//Assuming the user input exceeds screensize.
					newHeight = RowLimit;
					newWidth = newHeight / ((double)oldHeight / (double)oldWidth);
				}
				else {
					//Assuming the user input does not exceed screensize.
					newHeight = newRows;
					newWidth = newHeight / ((double)oldHeight / (double)oldWidth);
				}
			}
		}
		else {
			//This is case 3
			//This is case 1
			if (oldWidth > oldHeight) {
				//if oldCols is bigger, we'll base the new dimensions on columns first!
				if (newCols > ColumnLimit) {
					//Assuming the user input exceeds screensize.
					newWidth = ColumnLimit;
					newHeight = ((double)oldHeight / (double)oldWidth) * newWidth;
				}
				else {
					//Assuming the user input does not exceed screensize.
					newWidth = newCols;
					newHeight = ((double)oldHeight / (double)oldWidth) * newWidth;
				}
			}
			else {
				if (newRows > RowLimit) {
					//Assuming the user input exceeds screensize.
					newHeight = RowLimit;
					newWidth = newHeight / ((double)oldHeight / (double)oldWidth);
				}
				else {
					newHeight = newRows;
					newWidth = newHeight / ((double)oldHeight / (double)oldWidth);
				}
			}
		}
	} else {
		//if aspectFlag is false
		newHeight = newRows;
		newWidth = newCols;
	}

	cv::Mat Resized;
	int ColumnOfNewImage = newWidth;
	int RowsOfNewImage = newHeight;

	if (grayscaleFlag == false) {
		//for now let's implement the resize function
		cv::resize(image, Resized, cv::Size(ColumnOfNewImage, RowsOfNewImage));

		//Display resized image.
		cv::imshow("Resized Image", Resized);

	}
	if (grayscaleFlag == true) {
		// Read the same image as grayscale image.
		cv::Mat img_gray = cv::imread(path, cv::IMREAD_GRAYSCALE);

		// Display grayscale image
		cv::imshow("Grayscale Rendering", img_gray);
		cv::waitKey();

		// Save a copy of grayscale image in a file on disk.
		std::string gray_pic_file = path;
		gray_pic_file.insert(gray_pic_file.find_last_of('.'), "_gray");
		cv::imwrite(gray_pic_file, img_gray);
	}

	// Resized image dimensions
	std::cout << "\nResized size is: " << Resized.cols << "x" << Resized.rows << std::endl;
	// Resized pixel size
	std::cout << "Pixel size: " << Resized.cols * Resized.rows << std::endl << std::endl;


	//TODO: need to create a way to write metadata.


	return;
}

void downScaleImage(std::string path) {
	//downscaling images a shrinking of the original images and returning it back to its original dimensions.

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

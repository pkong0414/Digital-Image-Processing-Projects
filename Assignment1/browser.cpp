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

int depthfirstapply(char* path, std::list <std::string> &fileList);
void displayResizeImg(std::string path, int oldCols, int oldRows, int newCols, int newRows);

int main(int argc, char** const argv)
{
	/*
	//debugging output for command line parsing

	for (int c = 0; c < argc; c++) {
		printf("argv[ %d ]: %s\n", c, argv[c]);
	}
	*/

	//the parameter of the x screen
	int cParam = 0;
	//the parameter of the y screen
	int rParam = 0;
	int key = 0;
	std::string currentImage;

	const std::string keys =
	{
		"{help h usage ?	|      | print this message								}"
	#ifdef _WIN32
		"{rows r			|  0   | Max number of rows on screen					}"
		"{cols c			|  0   | Max number of columns on screen				}"
	#else
		"{rows r			| 720  | Max number of rows on screen					}"
		"{cols c			|1280  | Max number of columns on screen				}"
	#endif
		"{@directory		|<none>| Directory that contains the pictures to browse	}"
	};

	//work on this later
	//parsing the argument on the commandline
	cv::CommandLineParser parser(argc, argv, keys);
	if (parser.has("h"))
	{
		parser.about("\nThis is an image browser program.\n"
			"invoke: browser -h -r numRows -c numCol <fileDirectory>.\n");
		parser.printMessage();
		return 0;
	}
	rParam = parser.get<uint>("rows");
	if (rParam == 0)
		rParam = 720;		//default parameters
	cParam = parser.get<uint>("cols");
	if (cParam == 0)
		cParam = 1080;		//default parameters

	/*
#pragma once

	// OpenCV command line parser functions
	// Variable keys accepted by command line parser
	//!< keys contains possible arguments and their default values
	//!< rows defaults to 0 in Windows and 720 in Linux or Apple
	//!< cols defaults to 0 in Windows and 1280 in Linux or Apple

	const std::string keys =
	{
		"{help h usage ?	|      | print this message								}"
	#ifdef _WIN32
		"{rows r			|  0   | Max number of rows on screen					}"
		"{cols c			|  0   | Max number of columns on screen				}"
	#else
		"{rows r			| 720  | Max number of rows on screen					}"
		"{cols c			|1280  | Max number of columns on screen				}"
	#endif
		"{@directory		|<none>| Directory that contains the pictures to browse	}"
	};
	parser.about("Image Browser v1.0");
	if (parser.has("help") || dir.empty())
	{
		parser.printMessage();
		return (1);
	}

	// Find the maximum number of rows and columns on screen.  If those
	// are specified in command line, use those.  Otherwise, use the
	// default values in case of Linux or Apple, or compute to the maximum
	// dimensions of primaryt screen in case of Windows.

	maxrows = parser.get<uint>("rows");		//!< Maximum number of rows in display window
	maxcols = parser.get<uint>("cols");		//!< Maximum number of columns in display window

#ifdef _WIN32
		// Default for Windows is 0 rows and 0 columns.  If no dimensions are specified,
		// determine the screen size and use those dimensions as maximum rows/columns.

	if (maxrows == 0 || maxcols == 0)
	{
		maxcols = static_cast<int>(GetSystemMetrics(SM_CXSCREEN));
		maxrows = static_cast<int>(GetSystemMetrics(SM_CYSCREEN));
	}
#endif

	// Make sure that the parameters are correctly parsed

	if (!parser.check())
	{
			parser.printErrors();
		return (1);
	}
	*/
	
	char* fileDir = argv[argc - 1];
	
	/*
	printf("r: %d\n", rParam);
	printf("c: %d\n", cParam);
	*/

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
			depthfirstapply(fileDir, fileResult);
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
			if (image.empty())
				throw (std::string("Cannot open input image ") + *move);
			
			// Read the same image as grayscale image.
			//cv::Mat img_gray = cv::imread(*move, cv::IMREAD_GRAYSCALE);
			
			// Display color image
			cv::imshow( *move, image);

			// Image dimensions
			std::cout << "Image size is: " << image.cols << "x" << image.rows << std::endl;
			// Image pixel size
			std::cout << "Pixel size: " << image.cols * image.rows << std::endl;

			//display Resize Image function
			displayResizeImg(*move, image.cols, image.rows, cParam, rParam);

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
				if (move == vecOfFile.end() ) {
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

int depthfirstapply(char* path, std::list <std::string> &fileList) {
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

void displayResizeImg(std::string path, int oldWidth, int oldHeight, int newCols, int newRows) {
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
				newWidth= newHeight / ((double)oldHeight / (double)oldWidth);
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

	//testing area
	cv::Mat Resized;
	int ColumnOfNewImage = newWidth;
	int RowsOfNewImage = newHeight;

	//for now let's implement the resize function
	cv::resize(image, Resized, cv::Size(ColumnOfNewImage, RowsOfNewImage));

	//Display resized image.
	cv::imshow("Resized Image", Resized);

	// Resized image dimensions
	std::cout << "\nResized size is: " << Resized.cols << "x" << Resized.rows << std::endl;
	// Resized pixel size
	std::cout << "Pixel size: " << Resized.cols * Resized.rows << std::endl << std::endl;

	return;
}
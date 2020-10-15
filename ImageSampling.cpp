// ImageSampling.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <Windows.h>
#include <opencv2/core.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <iostream>
#include <string>
#include <sstream>
#include <cstdlib>
#include <time.h>
#include <sys/stat.h>
#include <fstream>

#define MAX_PATH 255

//function prototypes
cv::Mat pixelDeletion( cv::Mat image );
cv::Mat pixelReplication( cv::Mat image );
cv::Mat averagingDownSamp(cv::Mat image );
void nearestNeighbor(std::string path );
void adjustGammaIntensity(std::string path);

//global variables


int main(int argc, char** const argv)
{

	//debugging output for command line parsing

	for (int c = 0; c < argc; c++) {
		printf("argv[ %d ]: %s\n", c, argv[c]);
	}

	//row and column parameters
	int sParam;
	int dParam;
	int iParam;

	int key = 0;

	std::string currentImage;

	const char* keys =
	{
		"{ help h usage ?   |      | print this message															}"
		"{ sampling s       | 1    | Sampling method; 1 for pixel deletion replication, 2 for pixel averaging/interpolation [default: 1]}"
		"{ depth d          | 1    | Number of levels for downsampling [default: 1]					            }"
		"{ intensity i      | 1    | Intensity levels, between 1 and 7 [default: 1]					}"
		"{ @image           |<none>| The image file					}"
	};

	

	//work on this later
	//parsing the argument on the commandline
	cv::CommandLineParser parser(argc, argv, keys);

	if (argc < 3)
	{
		//std::cerr << "usage: " << argv[0] << " image_file" << std::endl;
		parser.about("\nThis is an image corpus program.\n");
		parser.printMessage();
		return EXIT_SUCCESS;
	}

	if (parser.has("help"))
	{
		parser.about("\nThis is an image corpus program.\n");
		parser.printMessage();
		return EXIT_SUCCESS;
	}

	//getting s parameter
	if (parser.get<int>("s") < 1) {
		sParam = 1;
	}
	else {
		sParam = parser.get<int>("s");
	}
	
	//getting d parameter
	if (parser.get<int>("d") < 1) {
		dParam = 1;
	}
	else {
		dParam = parser.get<int>("d");
	}
	
	//getting i parameter
	if (parser.get<int>("i") < 1) {
		iParam = 1;
	}
	else if (parser.get<int>("i") > 7) {
		iParam = 7;
	}
	else {
		iParam = parser.get<int>("i");
	}
	

	printf("sample: %d\n", sParam);
	printf("depth: %d\n", dParam);
	printf("intensity: %d\n", iParam);

	cv::String imageFile = parser.get<std::string>(0);
	printf("Image file: %s\n", imageFile.c_str());

	//Matrix vars for sampling 1
	cv::Mat pixDelImage;
	cv::Mat pixRelImage;

	//Matrix vars for sampling 2
	cv::Mat pixAvgImage;
	cv::Mat pixInterpImage;

	if (!parser.check())
	{
		parser.printErrors();
		return 0;
	}

	try
	{
		cv::Mat image = cv::imread(imageFile);

		// Make sure that the image is read properly.
		if (image.empty()) {
			//printing error message
			printf("Cannot open input image is not a file.\n");
		}

		cv::imshow("Source Image", image);
		//keypress wait
		key = cv::waitKeyEx(0);
		if (sParam == 1) {

			pixDelImage = pixelDeletion(image);
			if (dParam > 1) {
				int d = 1;
				while (d != dParam) {
					pixDelImage = pixelDeletion(pixDelImage);
					d++;
				}
			}
			pixRelImage = pixelReplication(image);
			
			//displaying output after finished
			cv::imshow("pixel deleted downsample", pixDelImage);
			cv::imshow("pixel replicated upsample", pixRelImage);
		}

		if (sParam == 2) {
			int d = 1;

			pixAvgImage = averagingDownSamp(image);
			//pixInterpImage;
			while (d != dParam) {

			}

		}

		

		//keypress wait
		key = cv::waitKeyEx(0);
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


void displayResizeImg(std::string path, int oldWidth, int oldHeight, int newCols, int newRows, std::string fileType) {
	//we are going to use this function to detect aspect ratio

	// Reading image
	cv::Mat image = cv::imread(path);

	if (image.empty()) {
		throw (std::string("Cannot open input image "));
		return;
	}




	//TODO: need to create a way to write metadata.

	return;
}

cv::Mat pixelDeletion( cv::Mat image ) {
	//This function will downsample an image using pixel depletion.
	printf("image rows: %d, image cols: %d\n", image.rows, image.cols);
	cv::Mat dest_mat(image.rows / 2, image.cols / 2, image.type());

	for (int c = 0; c < dest_mat.cols; c++) {
		for (int r = 0; r < dest_mat.rows; r++) {
			dest_mat.at<cv::Vec3b>(r, c) = image.at<cv::Vec3b>(r * 2, c * 2);
		}
	}

	//debug output
	//cv::imshow("pixel deleted image", dest_mat);
	return dest_mat;
}

cv::Mat pixelReplication(cv::Mat image) {
	//This function will upsample using pixel replication.
	cv::Mat dest_mat(image.rows * 2, image.cols * 2, image.type());

	for( int c = 0; c < image.cols; c++ ) {
		for (int r = 0; r < image.rows; r++) {
			dest_mat.at<cv::Vec3b>(2* r, 2 * c) = image.at<cv::Vec3b>(r, c);
			dest_mat.at<cv::Vec3b>(2 * r + 1, 2 * c) = image.at<cv::Vec3b>(r, c);
			dest_mat.at<cv::Vec3b>(2 * r, 2 * c + 1) = image.at<cv::Vec3b>(r, c);
			dest_mat.at<cv::Vec3b>(2 * r + 1, 2 * c + 1) = image.at<cv::Vec3b>(r, c);

		}
	}

	//debug output
	//cv::imshow("pixel replicated image", dest_mat);
	return dest_mat;
}

cv::Mat averagingDownSamp(cv::Mat image) {
	//This function will downsample using averaging
	cv::Mat dest_mat(image.rows / 2, image.cols / 2, image.type());

	for (int c = 0; c < dest_mat.cols; c++) {
		for (int r = 0; r < dest_mat.rows; r++) {
			dest_mat.at<cv::Vec3b>(r, c) = ( image.at<cv::Vec3b>(r * 2, c * 2) + image.at<cv::Vec3b>(r * 2 + 1, c * 2) + image.at<cv::Vec3b>(r * 2, c * 2 + 1) + image.at<cv::Vec3b>(r * 2 + 1, c * 2 + 1)) / 4;
		}
	}

	//debug output
	cv::imshow("averaging pixel image", dest_mat);
	return dest_mat;
}

void interpolationUpSamp(std::string path) {
	//This function will upsample using nearest Neighbor algorithm

}

void adjustGammaIntensity(std::string path) {
	//This function will adjust the image's gamma intensity

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
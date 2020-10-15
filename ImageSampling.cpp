// ImageSampling.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <Windows.h>
#include <opencv2/core.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <math.h>
#include <iostream>
#include <string>
#include <sstream>
#include <cstdlib>
#include <time.h>
#include <sys/stat.h>
#include <fstream>

#define MAX_PATH 255

//function prototypes
cv::Mat pixelDeletion(cv::Mat image, bool grayScale);
cv::Mat pixelReplication( cv::Mat image, bool grayScale);
cv::Mat averagingDownSamp(cv::Mat image, bool grayScale);
cv::Mat interpolationUpSamp(cv::Mat image, bool grayScale);
cv::Mat adjustGammaIntensity(cv::Mat image, int intensity);

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
		iParam = 1 - 1;
	}
	else if (parser.get<int>("i") > 7) {
		iParam = 7 - 1;
	}
	else {
		iParam = parser.get<int>("i") - 1;
	}
	

	printf("sample: %d\n", sParam);
	printf("depth: %d\n", dParam);
	printf("intensity: %d\n", iParam + 1);

	cv::String imageFile = parser.get<std::string>(0);
	printf("Image file: %s\n", imageFile.c_str());

	//Matrix vars for sampling 1
	cv::Mat pixDelImage;
	cv::Mat pixRelImage;
	cv::Mat grayDelImage;
	cv::Mat grayRelImage;

	//Matrix vars for sampling 2
	cv::Mat pixAvgImage;
	cv::Mat pixInterpImage;
	cv::Mat grayAvgImage;
	cv::Mat grayInterpImage;

	//Matrix var for gamma correction
	cv::Mat gammaCorrectImage;
	cv::Mat grayGammaCorrectImage;

	int d = 1;

	if (!parser.check())
	{
		parser.printErrors();
		return 0;
	}

	try
	{
		cv::Mat image = cv::imread(imageFile);
		cv::Mat grayImage = cv::imread(imageFile, cv::IMREAD_GRAYSCALE);

		// Make sure that the image is read properly.
		if (image.empty()) {
			//printing error message
			printf("Cannot open input image is not a file.\n");
		}

		cv::imshow("Source Image", image);
		cv::imshow("Gray Source Image", grayImage);

		//keypress wait
		key = cv::waitKeyEx(0);

		//destroying all windows
		cv::destroyAllWindows();

		if (sParam == 1) {

			pixDelImage = pixelDeletion(image, 0);
			grayDelImage = pixelDeletion(grayImage, 1);
			if (dParam > 1) {
				while (d != dParam) {
					pixDelImage = pixelDeletion(pixDelImage, 0);
					grayDelImage = pixelDeletion(grayDelImage, 1);
					d++;
				}
			}
			pixRelImage = pixelReplication(image, 0);
			grayRelImage = pixelReplication(grayImage, 1);

			//gamma correction
			gammaCorrectImage = adjustGammaIntensity(image, iParam);
			grayGammaCorrectImage = adjustGammaIntensity(grayImage, iParam);
			
			//displaying output after finished
			cv::imshow("pixel deleted downsample", pixDelImage);
			cv::imshow("grayscale pixel deleted downsample", grayDelImage);

			//keypress wait
			key = cv::waitKeyEx(0);
			cv::destroyAllWindows();

			cv::imshow("pixel replicated upsample", pixRelImage);
			cv::imshow("grayscale pixel replicated sample", grayRelImage);

			//keypress wait
			key = cv::waitKeyEx(0);
			cv::destroyAllWindows();

			cv::imshow("gamma corrected sample", gammaCorrectImage);
			cv::imshow("grayscale gamma corrected sample", grayGammaCorrectImage);

			//keypress wait
			key = cv::waitKeyEx(0);
			cv::destroyAllWindows();
		}

		if (sParam == 2) {
			pixAvgImage = averagingDownSamp(image, 0);
			grayAvgImage = averagingDownSamp(grayImage, 1);
			while (d != dParam) {
				pixAvgImage = averagingDownSamp(pixAvgImage, 0);
				grayAvgImage = averagingDownSamp(grayAvgImage, 1);
				d++;
			}
			pixInterpImage = interpolationUpSamp(image, 0);
			grayInterpImage = interpolationUpSamp(grayImage, 1);

			//gamma correction
			gammaCorrectImage = adjustGammaIntensity(image, iParam);
			grayGammaCorrectImage = adjustGammaIntensity(grayImage, iParam);

			//displaying output after finished
			cv::imshow("averaged pixel downsample", pixAvgImage);
			cv::imshow("grayscale averaged pixel downsample", grayAvgImage);

			//keypress wait
			key = cv::waitKeyEx(0);

			cv::destroyAllWindows();

			cv::imshow("interpolation upsample", pixInterpImage);
			cv::imshow("grayscale interpolation upsample", grayInterpImage);

			//keypress wait
			key = cv::waitKeyEx(0);
			cv::destroyAllWindows();

			cv::imshow("gamma corrected sample", gammaCorrectImage);
			cv::imshow("grayscale gamma corrected sample", grayGammaCorrectImage);

			//keypress wait
			key = cv::waitKeyEx(0);

			cv::destroyAllWindows();
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

cv::Mat pixelDeletion(cv::Mat image, bool grayScale) {
	//This function will downsample an image using pixel depletion.
	printf("image rows: %d, image cols: %d\n", image.rows, image.cols);
	cv::Mat dest_mat(image.rows / 2, image.cols / 2, image.type());
	
	if (grayScale == false) {
		for (int c = 0; c < dest_mat.cols; c++) {
			for (int r = 0; r < dest_mat.rows; r++) {
				dest_mat.at<cv::Vec3b>(r, c) = image.at<cv::Vec3b>(r * 2, c * 2);
			}
		}
	}
	else {
		for (int c = 0; c < dest_mat.cols; c++) {
			for (int r = 0; r < dest_mat.rows; r++) {
				dest_mat.at<uchar>(r, c) = image.at<uchar>(r * 2, c * 2);
			}
		}
	}

	//debug output
	//cv::imshow("pixel deleted image", dest_mat);
	return dest_mat;
}

cv::Mat pixelReplication(cv::Mat image, bool grayScale) {
	//This function will upsample using pixel replication.
	cv::Mat dest_mat(image.rows * 2, image.cols * 2, image.type());

	if (grayScale == false) {
		for (int c = 0; c < image.cols; c++) {
			for (int r = 0; r < image.rows; r++) {
				dest_mat.at<cv::Vec3b>(2 * r, 2 * c) = image.at<cv::Vec3b>(r, c);
				dest_mat.at<cv::Vec3b>(2 * r + 1, 2 * c) = image.at<cv::Vec3b>(r, c);
				dest_mat.at<cv::Vec3b>(2 * r, 2 * c + 1) = image.at<cv::Vec3b>(r, c);
				dest_mat.at<cv::Vec3b>(2 * r + 1, 2 * c + 1) = image.at<cv::Vec3b>(r, c);

			}
		}
	}
	else {
		for (int c = 0; c < image.cols; c++) {
			for (int r = 0; r < image.rows; r++) {
				dest_mat.at<uchar>(2 * r, 2 * c) = image.at<uchar>(r, c);
				dest_mat.at<uchar>(2 * r + 1, 2 * c) = image.at<uchar>(r, c);
				dest_mat.at<uchar>(2 * r, 2 * c + 1) = image.at<uchar>(r, c);
				dest_mat.at<uchar>(2 * r + 1, 2 * c + 1) = image.at<uchar>(r, c);

			}
		}
	}

	//debug output
	//cv::imshow("pixel replicated image", dest_mat);
	return dest_mat;
}

cv::Mat averagingDownSamp(cv::Mat image, bool grayScale) {
	//This function will downsample using averaging
	cv::Mat dest_mat(image.rows / 2, image.cols / 2, image.type());

	if (grayScale == false) {
		for (int c = 0; c < dest_mat.cols; c++) {
			for (int r = 0; r < dest_mat.rows; r++) {
				dest_mat.at<cv::Vec3b>(r, c) = (image.at<cv::Vec3b>(r * 2, c * 2) + image.at<cv::Vec3b>(r * 2 + 1, c * 2) + image.at<cv::Vec3b>(r * 2, c * 2 + 1) + image.at<cv::Vec3b>(r * 2 + 1, c * 2 + 1)) / 4;
			}
		}
	}
	else {
		for (int c = 0; c < dest_mat.cols; c++) {
			for (int r = 0; r < dest_mat.rows; r++) {
				dest_mat.at<uchar>(r, c) = (image.at<uchar>(r * 2, c * 2) + image.at<uchar>(r * 2 + 1, c * 2) + image.at<uchar>(r * 2, c * 2 + 1) + image.at<uchar>(r * 2 + 1, c * 2 + 1)) / 4;
			}
		}
	}

	//debug output
	//cv::imshow("averaging pixel image", dest_mat);
	return dest_mat;
}

cv::Mat interpolationUpSamp(cv::Mat image, bool grayScale) {
	//This function will upsample using nearest Neighbor algorithm
	cv::Mat dest_mat(image.rows * 2, image.cols * 2, image.type());

	if (grayScale == false) {
		for (int c = 0; c < image.cols - 1; c++) {
			for (int r = 0; r < image.rows - 1; r++) {
				dest_mat.at<cv::Vec3b>(2 * r, 2 * c) = image.at<cv::Vec3b>(r, c);
				dest_mat.at<cv::Vec3b>(2 * r + 1, 2 * c) = (image.at<cv::Vec3b>(r, c) + image.at<cv::Vec3b>(r + 1, c)) / 2;
				dest_mat.at<cv::Vec3b>(2 * r, 2 * c + 1) = (image.at<cv::Vec3b>(r, c) + image.at<cv::Vec3b>(r, c + 1)) / 2;
				dest_mat.at<cv::Vec3b>(2 * r + 1, 2 * c + 1) = (image.at<cv::Vec3b>(r, c) + image.at<cv::Vec3b>(r + 1, c) + image.at<cv::Vec3b>(r, c + 1) + image.at<cv::Vec3b>(r + 1, c + 1)) / 4;
			}
		}
	}
	else {
		for (int c = 0; c < image.cols - 1; c++) {
			for (int r = 0; r < image.rows - 1; r++) {
				dest_mat.at<uchar>(2 * r, 2 * c) = image.at<uchar>(r, c);
				dest_mat.at<uchar>(2 * r + 1, 2 * c) = (image.at<uchar>(r, c) + image.at<uchar>(r + 1, c)) / 2;
				dest_mat.at<uchar>(2 * r, 2 * c + 1) = (image.at<uchar>(r, c) + image.at<uchar>(r, c + 1)) / 2;
				dest_mat.at<uchar>(2 * r + 1, 2 * c + 1) = (image.at<uchar>(r, c) + image.at<uchar>(r + 1, c) + image.at<uchar>(r, c + 1) + image.at<uchar>(r + 1, c + 1)) / 4;
			}
		}
	}

	//debug output
	//cv::imshow("interpolation up sample image", dest_mat);
	return dest_mat;
}

cv::Mat adjustGammaIntensity(cv::Mat image, int intensity) {
	//This function will adjust the image's gamma intensity

	//s = (cr)^gamma

	double gammaValues[7] = { 0.1, 0.5, 1.0, 3.0, 3.5, 4.5, 5.0 };
	double gammaValue = gammaValues[intensity];

	cv::Mat dest_mat(image.size(), CV_32FC1);
	image.convertTo(image, CV_32FC1, 1.0 / 255.0);

	cv::pow(image, gammaValue, dest_mat);
	dest_mat.convertTo(dest_mat, CV_8UC1, 255);
	
	return dest_mat;

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
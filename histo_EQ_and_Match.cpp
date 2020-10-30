// histo_EQ_and_Match.cpp : This file contains the 'main' function. Program execution begins and ends there.
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
#include <dirent.h>
#include <sys/stat.h>
#include <vector>
#include <list>
#include <fstream>


//function prototypes
void createHistogram(cv::Mat image, int histogram[]);
void readingProbHistoFile(float probHistogram[], cv::String histoFileName);
void histoNormalize(int histogram[], float probHisto[], int totalPixels);
void histoEqualization( float probHisto[], int equalizedLUT[]);
cv::Mat imageEqualization(cv::Mat image, std::string filename, int equalizedLUT[]);
void writeToFile();
cv::Mat histogramMatching(cv::Mat image, int equalizedLUT1[], int equalizedLUT2[]);
int findLUTIndex(int pixelValue, int LUT1[], int LUT2[]);

//global variables

int main(int argc, char** const argv)
{

	//debugging output for command line parsing

	for (int c = 0; c < argc; c++) {
		printf("argv[ %d ]: %s\n", c, argv[c]);
	}

	//row and column parameters
	int key;
	int mParam;

	const char* keys =
	{
		"{ help h usage ?   |      | print this message}"
		"{ @infile          |<none>| Input File.       }"
		"{ @histofile       |<none>| Histogram File.   }"
		"{ mode m           |1     | Mode of the program:\n"	
		                            "                 1: The program will perform histogram equalization (This is default)\n"
		                            "                 2: The program will perform histogram equalization on an image\n" 
		                            "                 3: The program will perform histogram matching}"
	};

	//work on this later
	//parsing the argument on the commandline
	cv::CommandLineParser parser(argc, argv, keys);
	if (parser.has("help")) {
		parser.about("\nThis is a program that will do histogram normalization, matching, and .\n");
		parser.printMessage();
		return EXIT_SUCCESS;
	}

	if (argc < 2) {
		parser.about("\nThis is a program that will do histogram normalization, matching, and .\n");
		parser.printMessage();
		return EXIT_SUCCESS;
	}

	std::cout << parser.has("mode") << std::endl;
	if (parser.has("mode")) {
		// it appears debug mode will always make parameters true
		// take note to make it false to test false conditions.
		mParam = parser.get<int>("mode");
	}

	//This is the default picture we are working with
	cv::String filename = "C:\\Users\\PatK0\\OneDrive\\Desktop\\CS4420\\images\\Yuniel.jpg";

	//These files will be used depending on mode parameter
	cv::String referencedFileName;
	cv::String histoFileName;

	if (mParam == 2) {
		referencedFileName = parser.get<std::string>(0);
		printf("referencedFileName: %s\n", referencedFileName.c_str());
	}
	if (mParam == 3) {
		histoFileName = parser.get<std::string>(1);
		printf("histoFileName: %s\n", histoFileName.c_str());
	}

	printf("mParam: %d\n", mParam);

	printf("filename: %s\n", filename.c_str());

	//printf("histoFile: %s\n", histoFile.c_str());

	// experiment with xml later. For now I'll use metadata text to write what I need to write.

	if (!parser.check())
	{
		parser.printErrors();
		return 0;
	}

	try
	{
		int totalPixels;
		int histogram[256];
		float probHisto[256];
		float probTotal = 0;
		int equalizedLUT[256];
		float runningTotal = 0;

		int* LUTPtr;
		LUTPtr = equalizedLUT;

		//debugging parameter argument output
		/*
		for (int c = 0; c < argc; c++) {
			std::cout << "argv[" << c << "]: " << argv[c] << std::endl;
		}
		*/
		for(int i = 0; i < 256; i++) {
			histogram[ i ] = 0;
			equalizedLUT[ i ] = 0;
		}

		//testing output
		cv::Mat image = cv::imread(filename, cv::IMREAD_GRAYSCALE);

		totalPixels = image.cols * image.rows;

		printf("total pixels: %d\n", totalPixels);
		// Display color image
		cv::imshow("testPicture", image);

		//debugging output
		printf("cols: %d\n", image.cols);
		printf("rows: %d\n", image.rows);

		//creating a histogram
		createHistogram(image, histogram);

		//for now we will use this function to write a histogram for a sample file. 
		//Will later turn this into a function that can be used with a 2nd referenced image if needed.
		//writing to histogram.txt
		//writeToFile();

		//getting probability histogram
		histoNormalize( histogram, probHisto, totalPixels);

		//checking to see if the probability is consistent
		for (int p = 0; p < 256; p++) {
			probTotal += probHisto[p];
		}

		//equalizing the normalized histogram
		//NOTE: we pass the array by reference to get back an array of the equalizedLUT
		histoEqualization(probHisto, equalizedLUT);

		//debugging output
		for (int i = 0; i < 256; i++) {
			//printf("MAIN: equalizedLUT[ %d ]: %d\n", i, equalizedLUT[i]);
			//printf("histogram[ %d ]: %d\n", i, histogram[i]);
			//printf("histoNorm[ %d ]: %f\n", i, histoNorm[i]);
			//printf("probHisto[ %d ]: %f\n", i, probHisto[i]);
		}

		if (mParam == 1) {
			//mode 1 will perform Histogram equalization (default)

			//debugging output
			printf("probTotal: %f\n", probTotal);

			//applying histogram equalization to image.
			cv::Mat newImage = imageEqualization(image, filename, equalizedLUT);

			//displaying results
			cv::imshow("equalized image", newImage);

			// Image dimensions
			std::cout << "Image size is: " << image.cols << "x" << image.rows << std::endl;
			// Image pixel size
			std::cout << "Pixel size: " << image.cols * image.rows << std::endl;

			//keypress wait
			printf("finished!\n");
			key = cv::waitKeyEx(0);

			return 0;

		}
		if (mParam == 2) {
			//mode 2 will perform Histogram matching with image

			//same procedure except we'll be performing it on a referenced image to get a equalizedLUT of that image.
			cv::Mat refImage = cv::imread(referencedFileName, cv::IMREAD_GRAYSCALE);
			int refHistogram[256];
			float refProbHisto[256];
			int refEqualizedLUT[256];
			float refProbTotal = 0;

			int totalPixels = refImage.cols * refImage.rows;

			for (int i = 0; i < 256; i++) {
				//initializing all the arrays
				refHistogram[i] = 0;
				refProbHisto[i] = 0;
				refEqualizedLUT[i] = 0;
			}

			printf("total pixels: %d\n", totalPixels);

			//creating a histogram
			createHistogram(refImage, refHistogram);

			//for now we will use this function to write a histogram for a sample file. 
			//Will later turn this into a function that can be used with a 2nd referenced image if needed.
			//writing to histogram.txt
			//writeToFile();

			//getting probability histogram
			histoNormalize(refHistogram, refProbHisto, totalPixels);

			//checking to see if the probability is consistent
			for (int p = 0; p < 256; p++) {
				refProbTotal += refProbHisto[p];
			}

			//checking to see consistency in refProbHistogram
			printf("refProbTotal: %f\n", refProbTotal);

			//equalizing the normalized histogram
			//NOTE: we pass the array by reference to get back an array of the equalizedLUT
			histoEqualization(refProbHisto, refEqualizedLUT);

			cv::Mat histoMatchImage = histogramMatching(image, equalizedLUT, refEqualizedLUT);
			cv::imshow("referenced image", refImage);
			cv::imshow("histogram matched image", histoMatchImage);
			printf("finished!\n");
			key = cv::waitKeyEx(0);

			return 0;
		}
		if (mParam == 3) {
			//mode 3 will perform Histogram matching with normalized histogram file

			float probHisto2[256];
			int equalizedLUT2[256];
			//initializing histogram2
			for (int m = 0; m < 256; m++) {
				equalizedLUT2[m] = 0;
			}

			readingProbHistoFile( probHisto2, histoFileName);

			for (int p = 0; p < 256; p++) {
				printf("probHisto2[p]: %f\n", probHisto2[p]);
			}

			//equalizing the normalized histogram
			histoEqualization(probHisto2, equalizedLUT2);

			for (int n = 0; n < 256; n++) {
				printf("equalizedLUT2[ %d ]: %d\n", n, equalizedLUT2[n]);
			}

			//now performing the image histogram matching.
			cv::Mat histoMatchImage2 = histogramMatching(image, equalizedLUT, equalizedLUT2);

			cv::imshow("histogram matched image", histoMatchImage2);

			cv::String savePath = "C:\\Users\\PatK0\\OneDrive\\Desktop\\CS4420\\images";
			cv::String saveFile = "\\histoMatchedImage.jpg";

			savePath.append(saveFile);
			// I'm going to use the absolute path here cause it makes sense for my own files.
			// I don't want it to be in reference dir of this project.
			cv::imwrite( savePath, histoMatchImage2);

			printf("finished!\n");
			key = cv::waitKeyEx(0);

			return 0;

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

	//closing files
	

	return (0);
}

//******************************* functions ******************************************

void createHistogram(cv::Mat image, int histogram[]) {

	for (int r = 0; r < image.rows; r++) {
		for (int c = 0; c < image.cols; c++) {
			//printf("pixel color: %d\n", image.at<uchar>(c, r));
			//operation to create histogram.

			histogram[(int)image.at<uchar>(r, c)]++;
		}
	}
}

void readingProbHistoFile(float probHistogram[], cv::String histoFileName) {
	std::fstream outFile;
	std::string line;
	int index = 0;
	outFile.open(histoFileName, std::ios::in);
	
	if (outFile.is_open()) {
		while (outFile) {
			outFile >> line;
			if (outFile.eof())
				break;
			//debugging output
			//std::cout << "index: " << index << " line: " << line << std::endl;

			probHistogram[index] = std::stof(line);
			index++;
		}

		//debugging output
		for( int t = 0; t < 256; t++) {
			printf("probHistogram[ %d ]: %f\n", t, probHistogram[t]);
		}

	}
	else printf("cannot open file.\n");

	outFile.close();
	return;
}

void histoNormalize( int histogram[], float probHisto[], int totalPixels ) {

	for(int j = 0; j < 256; j++) {
		//printf("histogram[ %d ]: %d\n", j, histogram[j]);
		probHisto[j] = (float)histogram[j] / (float)totalPixels;
		//histoNorm[j] = histogram[j] * (1 / totalPixels);
	}
}

void histoEqualization( float probHisto[], int equalizedLUT[]) {
	float runningTotal = 0;

	for (int q = 0; q < 256; q++) {
		equalizedLUT[q] = (int)((runningTotal + probHisto[q]) * 255);
		runningTotal += probHisto[q];
	}

	//debugging output
	/*for (int i = 0; i < 256; i++) {
		printf("EQ_FUNC: equalizedLUT[ %d ]: %d\n", i, equalizedLUT[i]);
	}*/
	
}

cv::Mat imageEqualization(cv::Mat image, std::string filename, int equalizedLUT[]) {
	cv::Mat newImage = cv::imread(filename, cv::IMREAD_GRAYSCALE);

	for (int r = 0; r < image.rows; r++) {
		for (int c = 0; c < image.cols; c++) {
			//printf("pixel color: %d\n", image.at<uchar>(c, r));
			//operation to equalize image with histogram.

			newImage.at<uchar>(r, c) = equalizedLUT[newImage.at<uchar>(r, c)];
		}
	}

	return newImage;
}

void writeToFile() {
	//this will write the histogram_norm.txt
	//for the sake of things really want to use Yuniel2 image so it shall be named thus.
	std::ofstream inFile;
	std::string filename = "C:\\Users\\PatK0\\OneDrive\\Desktop\\CS4420\\images\\Yuniel2_histogram_norm.txt";
	inFile.open(filename);
	int histogram[256];
	float probHisto[256];
	int totalPixels;
	cv::Mat image = imread("C:\\Users\\PatK0\\OneDrive\\Desktop\\CS4420\\images\\Yuniel2.jpg", cv::IMREAD_GRAYSCALE);

	totalPixels = image.cols * image.rows;
	cv::imshow("histogram file picture", image);

	for (int z = 0; z < 256; z++) {
		histogram[z] = 0;
	}

	// Image dimensions
	std::cout << "Histogram Image size is: " << image.cols << "x" << image.rows << std::endl;
	// Image pixel size
	std::cout << "Histogram Pixel size: " << image.cols * image.rows << std::endl;

	//creating a histogram
	createHistogram(image, histogram);

	//calculating probability for histogram2
	histoNormalize(histogram, probHisto, totalPixels);

	if (inFile.is_open())
	{
		printf("writing to %s... \n", filename.c_str());
		for (int i = 0; i < 256; i++) {
			inFile << probHisto[i] << std::endl;
		}
		inFile.close();
	}
	else std::cout << "Unable to open file\n";
}

cv::Mat histogramMatching(cv::Mat image, int equalizedLUT1[], int equalizedLUT2[]) {
	cv::Mat histoMatchedImg = image;
	int errno;
	//we'll be checking for the image's gray level first
	for (int r = 0; r < image.rows; r++) {
		for (int c = 0; c < image.cols; c++) {
			//printf("gray level at (r: %d c:%d): %d\n", r, c, image.at<uchar>(r, c));
			//assigning index as a value to the histo matched image.
			
			if ((errno = (histoMatchedImg.at<uchar>(r, c) = findLUTIndex(image.at <uchar>(r, c), equalizedLUT1, equalizedLUT2)) == -1)) {
				printf("error occured during histogram matching!\n");
				exit(1);
			}
		}
	}

	return histoMatchedImg;
}

int findLUTIndex(int pixelValue, int LUT1[], int LUT2[]) {
	int index2;
	int LUT1Value = LUT1[pixelValue];
	

	for (int L2 = 0; L2 < 256; L2++) {
		//we assume 2 cases with this
		// 1. we get the best case where the value of LUT1Value matches LUT2
		// 2. we reached a number too high and must find a value where LUT2 < LUT1Value
		
		// I think we should handle the 2nd case first.
		if (LUT2[L2] > LUT1Value) {
			//we've hit a higher value and must turn back!
			for (int back = L2; back >= 0; back--) {
				if (LUT2[back] < LUT1Value) {
					index2 = back;
					return index2;
				}
			}
		}
		
		if (LUT2[L2] == LUT1Value) {
			index2 = L2;
			return index2;
		}
	}
	return -1;
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

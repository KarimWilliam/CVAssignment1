

#include "pch.h"
#include <iostream>
#include<opencv2/opencv.hpp>
using namespace std;
using namespace cv;
Mat FlashBang(Mat, double);
Mat Overlay(Mat, Mat);
Mat RotateImg(Mat, double);
Mat sherlay(Mat , Mat, Mat );

int main()
{
	std::cout << "Computer Vision Assignment\n";
	double alpha = 5.5;
	Size size(1121,788);
	Size size2(335, 430);
	Size size3(90, 140);
	double angle = -15;
	namedWindow("image", WINDOW_AUTOSIZE);
	namedWindow("image2", WINDOW_AUTOSIZE);
	namedWindow("image3", WINDOW_AUTOSIZE);
	namedWindow("image4", WINDOW_AUTOSIZE);

	//Color Correction
	Mat woman = imread("A1I/Q1I1.png");
	Mat bat = imread("A1I/Q1I2.jpg");

	Mat flashWoman = FlashBang(woman, alpha);
	Mat flippedBat = Mat::zeros(bat.size(), bat.type());
	Mat shiftedBat = Mat::zeros(bat.size(), bat.type());
	Mat trans_mat = (Mat_<double>(2, 3) << 1, 0, 200, 0, 1, 0);
	Mat resizedBat = Mat::zeros(bat.size(), bat.type());
	Mat finalOut = Mat::zeros(bat.size(), bat.type());
	
	//operations on bat image 
	cv::flip(bat, flippedBat, 1);
	cv::warpAffine(flippedBat,shiftedBat,trans_mat,bat.size());
	resize(shiftedBat, resizedBat, size);

	//combining images
	finalOut=Overlay(resizedBat, flashWoman);

	//Show output
	imshow("image2", finalOut);
	imwrite("output/finalOut.png", finalOut);

	
	//Fitting Frames
	Mat sherlock = imread("A1I/Q2I1.jpg");
	cv::cvtColor(sherlock, sherlock, CV_BGR2BGRA);
	Mat closeFrame = imread("A1I/Q2I3.jpg");
	cv::cvtColor(closeFrame, closeFrame, CV_BGR2BGRA);
	Mat farFrame = imread("A1I/Q2I2.jpg");


	//Close Frame
	Mat closeFrameRot = RotateImg(closeFrame, 6);
	resize(sherlock, sherlock, size2);
	sherlock.copyTo(closeFrameRot(cv::Rect(382, 165, sherlock.cols, sherlock.rows)));
	closeFrame = RotateImg(closeFrameRot, -6);
	imshow("image3", closeFrame);
	imwrite("output/out1.png", closeFrame);


	//Far Frame
	sherlock = imread("A1I/Q2I1.jpg");
	resize(sherlock, sherlock, size3);;
	sherlock.copyTo(farFrame(cv::Rect(1220, 377, sherlock.cols, sherlock.rows)));
	imshow("image4", farFrame);
	imwrite("output/out2.png", farFrame);


	//Different Perspective Frame
	sherlock = imread("A1I/Q2I1.jpg");
	Mat tiltedFrame = imread("A1I/Q3I1.jpg");

	//defining points for warping prespective
	Point2f src1 = cv::Point2f(1, 1);
	Point2f src2 = cv::Point2f(1, 685);
	Point2f src3 = cv::Point2f(500, 1);
	Point2f src4 = cv::Point2f(500, 685);
	Point2f dst1 = cv::Point2f(162, 34);
	Point2f dst2 = cv::Point2f(158, 390);
	Point2f dst3 = cv::Point2f(470, 70);
	Point2f dst4 = cv::Point2f(463, 353);
	cv::Point2f source_points[4];
	cv::Point2f dest_points[4];
	source_points[0] = src1;
	source_points[1] = src2;
	source_points[2] = src3;
	source_points[3] = src4;

	dest_points[0] = dst1;
	dest_points[1] = dst2;
	dest_points[2] = dst3;
	dest_points[3] = dst4;

	//warping prespective of sherlock image
	Mat transformMatrix = cv::getPerspectiveTransform(source_points, dest_points);
	cv::warpPerspective(sherlock, sherlock, transformMatrix, tiltedFrame.size());
	
	//making mask for sherlock
	cv::Mat mask = cv::Mat::zeros(sherlock.size(), sherlock.type());
	mask = (sherlock < 1);



	Mat warpOut = sherlay(sherlock, tiltedFrame, mask);
	

	//outputing image
	imshow("image", warpOut);
	imwrite("output/warpOut.png", warpOut);

	waitKey(0);
	return 0;
}


Mat FlashBang(Mat img, double alpha) {
	
	Mat output = Mat::zeros(img.size(), img.type());

	for (int i = 0; i < img.cols; i++) {
		for (int j = 0; j < img.rows; j++) {
			for (int c = 0; c < 3; c++) {
				output.at<Vec3b>(j, i)[c] =saturate_cast<uchar>(alpha*(img.at<Vec3b>(j, i)[c]));
			}
		}
		alpha -= 0.005;
	}
	return output;


}

Mat Overlay(Mat img1, Mat img2) {
	Mat output = img2;
	//addWeighted(img1, 0.5, img2, 0.5, 0.0, output);
	for (int i = 0; i < img1.cols; i++) {
		for (int j = 0; j < img1.rows; j++) {
			for (int c = 0; c < 3; c++) {
				if (img1.at<Vec3b>(j,i)[0]>20 && img1.at<Vec3b>(j, i)[1] > 20 && img1.at<Vec3b>(j, i)[2] > 20 ) {
					output.at<Vec3b>(j, i)[c] = saturate_cast<uchar>(((img1.at<Vec3b>(j, i)[c]+ img2.at<Vec3b>(j, i)[c])/2));
				}
		
			}
		}
	}
	return output;
}

Mat RotateImg(Mat img, double angle) {


	// get rotation matrix for rotating the image around its center in pixel coordinates
	cv::Point2f center((img.cols - 1) / 2.0, (img.rows - 1) / 2.0);
	cv::Mat rot = cv::getRotationMatrix2D(center, angle, 1.0);
	// determine bounding rectangle, center not relevant
	cv::Rect2f bbox = cv::RotatedRect(cv::Point2f(), img.size(), angle).boundingRect2f();
	// adjust transformation matrix
	rot.at<double>(0, 2) += bbox.width / 2.0 - img.cols / 2.0;
	rot.at<double>(1, 2) += bbox.height / 2.0 - img.rows / 2.0;

	cv::Mat dst;
	cv::warpAffine(img, dst, rot, bbox.size());
	return dst;

}

Mat sherlay(Mat img1, Mat img2, Mat mask) {
	Mat output = img2;

	for (int i = 0; i < mask.cols; i++) {
		for (int j = 0; j < mask.rows; j++) {
			for (int c = 0; c < 3; c++) {
				if (mask.at<Vec3b>(j, i)[0]<1 ) {
					output.at<Vec3b>(j, i)[c] = img1.at<Vec3b>(j, i)[c];
					
				}

			}
		}
	}
	return output;
}

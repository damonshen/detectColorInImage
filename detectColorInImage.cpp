#include <stdio.h>
#include <opencv2/opencv.hpp>
#include "opencv2/highgui/highgui.hpp"

using namespace cv;
using namespace std;

IplImage* GetThresholdedImage(IplImage* img,int low_h,int low_s,int low_v,int high_h,int high_s,int high_v)
{
	// Convert the image into an HSV image
	IplImage* imgHSV = cvCreateImage(cvGetSize(img), 8, 3);
	cvCvtColor(img, imgHSV, CV_BGR2HSV);

	IplImage* imgThreshed = cvCreateImage(cvGetSize(img), 8, 1);
	// Values 96,119,81 to 158,255,226 working perfect for blue at around 6pm
	// Values 20,100,100 to 30,255,255 working perfect for yellow at around 6pm
	// Values 38,100,100 to 75,255,255 working perfect for green at around 6pm
	// Values 170,160,60 to 180,255,255 working perfect for red at around 6pm
	//Orange  0-22  Yellow 22- 38  Green 38-75  Blue 75-130  Violet 130-160  Red 160-179
	cvInRangeS(imgHSV, cvScalar(low_h,low_s,low_v), cvScalar(high_h,high_s,high_v), imgThreshed);

	cvReleaseImage(&imgHSV);

	return imgThreshed;
}

void objecttrack(IplImage* imgthresh,IplImage* frame,int h,int s,int v)
{
	// Calculate the moments to estimate the position of the ball
	CvMoments *moments = (CvMoments*)malloc(sizeof(CvMoments));

	cvMoments(imgthresh, moments, 1);

	// The actual moment values
	double moment10 = cvGetSpatialMoment(moments, 1, 0);
	double moment01 = cvGetSpatialMoment(moments, 0, 1);
	double area = cvGetCentralMoment(moments, 0, 0);
	cout << moment10 << " " << moment01 << endl;
	// Holding the last and current ball positions
	static int posX = 0;
	static int posY = 0;


	posX = moment10/area;
	posY = moment01/area;

	cout << posX << " " <<  posY << endl;
	// We want to draw a line only if its a valid position
	if( posX>0 && posY>0)
	{
		// Draw a yellow line from the previous point to the current point
		cvRectangle(frame, cvPoint(posX-50, posY-50), cvPoint(posX+50, posY+50), cvScalar(h,s,v),2,CV_AA,0);
		printf("position_start: (%d,%d)	;	position_end: (%d,%d)\n", posX-50, posY-50, posX+50, posY+50);
	}
	free(moments);
}

IplImage* crop( IplImage* src,  CvRect roi){
	// Must have dimensions of output image
	IplImage* cropped = cvCreateImage( cvSize(roi.width,roi.height), src->depth, src->nChannels );

	// Say what the source region is
	cvSetImageROI( src, roi );

	// Do the copy
	cvCopy( src, cropped );
	cvResetImageROI( src );

	return cropped;
}

int main( int argc, char** argv )
{
	/*
	   Mat image;
	   image = imread( argv[1], 1 );
	 */
	IplImage* frame = cvLoadImage(argv[1],1);

	if( argc != 2 )
	{
		printf( "No image data \n" );
		return -1;
	}
#if 0 
	   IplImage* imgRedThresh = GetThresholdedImage(frame,0,240,140,3,255,255);
	   IplImage* imgOrangeThresh = GetThresholdedImage(frame,20,255,245,27,255,255);
	   IplImage* imgYellowThresh = GetThresholdedImage(frame,26,229,239,40,255,255);
	   IplImage* imgGreenThresh = GetThresholdedImage(frame,50,100,100,75,255,255);
	   IplImage* imgLightBlueThresh = GetThresholdedImage(frame,79,250,234,100,255,247);
	   IplImage* imgBlueThresh = GetThresholdedImage(frame,101,180,140,105,255,255);
	   IplImage* imgNavyBlueThresh = GetThresholdedImage(frame,105,175,84,122,255,109);
	   IplImage* imgPurpleThresh = GetThresholdedImage(frame,120,155,150,147,255,180);


	   namedWindow( "red", CV_WINDOW_AUTOSIZE );
	   cvShowImage( "red", imgRedThresh );
	   namedWindow( "orange", CV_WINDOW_AUTOSIZE );
	   cvShowImage( "orange", imgOrangeThresh );
	   namedWindow( "yellow", CV_WINDOW_AUTOSIZE );
	   cvShowImage( "yellow", imgYellowThresh );
	   namedWindow( "green", CV_WINDOW_AUTOSIZE );
	   cvShowImage( "green", imgGreenThresh );
	   namedWindow( "light blue", CV_WINDOW_AUTOSIZE );
	   cvShowImage( "light blue", imgLightBlueThresh );
	   namedWindow( "blue", CV_WINDOW_AUTOSIZE );
	   cvShowImage( "blue", imgBlueThresh );
	   namedWindow( "navy blue", CV_WINDOW_AUTOSIZE );
	   cvShowImage( "navy blue", imgNavyBlueThresh );
	   namedWindow( "purple", CV_WINDOW_AUTOSIZE );
	   cvShowImage( "purple", imgPurpleThresh );
#endif

	//	objecttrack(imgGreenThresh,frame,100,255,0);
	namedWindow( "Original", CV_WINDOW_AUTOSIZE );
	cvShowImage( "Original", frame );


	//crop the subimage
	IplImage* cropped = crop(frame, cvRect( 20,20, 50,50 ));

	namedWindow( "crop", CV_WINDOW_AUTOSIZE );
	cvShowImage( "crop", cropped);


	waitKey(0);

	return 0;
}


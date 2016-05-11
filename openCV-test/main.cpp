#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

#ifdef _DEBUG
#pragma comment(lib, "opencv_core2410d.lib")
#pragma comment(lib, "opencv_imgproc2410d.lib")
#pragma comment(lib, "opencv_highgui2410d.lib")
#else
#pragma comment(lib, "opencv_core249.lib")
#pragma comment(lib, "opencv_imgproc249.lib")
#pragma comment(lib, "opencv_highgui249.lib")
#endif

using namespace cv;
using namespace std;

RNG rng(12345);
Mat src; Mat src_gray;
int thresh = 100;
int max_thresh = 255;
Mat src_mat, gray_mat, canny_mat;
void thresh_callback(int, void* );
Mat contour_mat;
Mat bounding_mat;
vector<vector<Point> > contours;
int main( int, char** argv )
{
    
    const char * file_name = "/Users/alex/Desktop/openCV-test/openCV-test/shape.JPG";

    
    
    
    const char* src_name = "Source";
    const char* contours_name = "Contours";
    const char* bounding_name = "Bounding";
    
    //1.Read image file & clone.
    src_mat = imread( file_name );
    contour_mat = src_mat.clone();
    bounding_mat = src_mat.clone();
    
    //2. Convert to gray image and apply canny edge detection
    cvtColor( src_mat, gray_mat, COLOR_RGB2GRAY );
    Canny(gray_mat, canny_mat, 30, 128, 3, false);

    Mat im_th;
    threshold(src_mat, im_th, 220, 255, THRESH_BINARY_INV);
    
    // Floodfill from point (0, 0)
    Mat im_floodfill = im_th.clone();
    floodFill(im_floodfill, cv::Point(0,0), Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) ));
    
    // Invert floodfilled image
    Mat im_floodfill_inv;
    bitwise_not(im_floodfill, im_floodfill_inv);
    
    // Combine the two images to get the foreground.
    Mat im_out = (im_th | im_floodfill_inv);

    src = im_out;
    
    /// Convert image to gray and blur it
    cvtColor( src, src_gray, CV_BGR2GRAY );
    blur( src_gray, src_gray, Size(3,3) );
    
    /// Create Window
    const char* source_window = "Source";
    namedWindow( source_window, CV_WINDOW_AUTOSIZE );
    
    
    createTrackbar( " Threshold:", "Source", &thresh, max_thresh, thresh_callback );
    
    
    
    thresh_callback( 0, 0 );
    
    double area, length;
    for(int i = 0; i < contours.size(); ++i)
    {
        Point2f center;
        float radius;
        
        area = contourArea(contours[i]);
        length = arcLength(contours[i], true);
        minEnclosingCircle(contours[i], center, radius);
        
        //draw contour property value at the contour center.
        
        if (length > 10) {
            char buffer[64] = {0};
            sprintf(buffer, "Area: %.2lf", area);
            putText(src, buffer, center, FONT_HERSHEY_SIMPLEX, .3, Scalar(0), 2);
            
            sprintf(buffer, "Length: %.2lf", length);
            putText(src, buffer, Point(center.x,center.y+20), FONT_HERSHEY_SIMPLEX, .3, Scalar(0), 2);
        }
        
        
    }
    imshow( source_window, src );
    // Display images
    //imshow("Foreground", im_out);
    
    //3.2 draw contours & property value on the source image.
//    double area, length;
//     drawContours(contour_mat, contours, -1, cv::Scalar(123), 4);  //draw contours on the image
    
//    3.3 find bounding of each contour, and draw it on the source image.
    
//    4. show window
//    namedWindow( src_name, WINDOW_AUTOSIZE );
//    namedWindow( contours_name, WINDOW_AUTOSIZE );
//    namedWindow( bounding_name, WINDOW_AUTOSIZE );
    
//    imshow(src_name, src_mat);
//    imshow(contours_name, contour_mat);
//    imshow(bounding_name, bounding_mat);
    
    waitKey(0);
    
    return 0;
}

/** @function thresh_callback */
void thresh_callback(int, void* )
{
    Mat threshold_output;
    
    vector<Vec4i> hierarchy;
    
    /// Detect edges using Threshold
    threshold( src_gray, threshold_output, thresh, 255, THRESH_BINARY );
    /// Find contours
    findContours( threshold_output, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );
    
    /// Approximate contours to polygons + get bounding rects and circles
    vector<vector<Point> > contours_poly( contours.size() );
    vector<Rect> boundRect( contours.size() );
    vector<Point2f>center( contours.size() );
    vector<float>radius( contours.size() );
    
    for( int i = 0; i < contours.size(); i++ )
    { approxPolyDP( Mat(contours[i]), contours_poly[i], 3, true );
        boundRect[i] = boundingRect( Mat(contours_poly[i]) );
        minEnclosingCircle( (Mat)contours_poly[i], center[i], radius[i] );
    }
   
    /// Draw polygonal contour + bonding rects + circles
    Mat drawing = src_mat;
    for( int i = 0; i< contours.size(); i++ )
    {
        Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
        drawContours( drawing, contours_poly, i, color, 5, 8, vector<Vec4i>(), 0, Point() );
        //rectangle( drawing, boundRect[i].tl(), boundRect[i].br(), color, 2, 8, 0 );
        //circle( drawing, center[i], (int)radius[i], color, 2, 8, 0 );
    }

    /// Show in a window
    namedWindow( "Contours", CV_WINDOW_AUTOSIZE );
    imshow( "Contours", drawing );
}

#include<opencv2/opencv.hpp>
#include<opencv2/core/utils/logger.hpp>

#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>

using namespace std;
using namespace cv;

//Globals
Mat mat_src;
Mat mat_out;
int w;
int h;
int c;

//Struct holding the () operator function applied in the Matrix forEach method
//Operates in parallel on all pixels within the image
//If the pixel is adjacent to the edge, it stays a set value
//If its surrounding pixels (at the given radius distance from the current pixel) are non-background valued,
//then it adds the add_amt to its current value and to those pixels at the radius distance's values
//The pixel values are checked via a globally defined reference Matrix to avoid referencing changing values 
struct DistanceTransform {
    int radius;

    DistanceTransform(int r) {
        radius = r;
    }

    void operator()(Vec<uchar,1> & p, const int* position) const {
        
        int x = position[0];
        int y = position[1];
        int x_max = w;
        int y_max = h;
        bool edge = false;
        int add_amt = 16;
        
        //If the current pixel is a non-background value
        if ((int)p[0] > 1) {
            
            //Check if background valued pixels are at radius distance
            if (y - radius > 0 && !edge) {
                if ((int)mat_src.at<Vec<uchar, 1>>(x, y - radius)[0] == 0) {
                    edge = true;
                }
            }
            if (y + radius < y_max && !edge) {
                if ((int)mat_src.at<Vec<uchar, 1>>(x, y + radius)[0] == 0) {
                    edge = true;
                }
            }
            if (x - radius > 0 && !edge) {
                if ((int)mat_src.at<Vec<uchar, 1>>(x - radius, y)[0] == 0) {
                    edge = true;
                }
            }
            if (x + radius < x_max && !edge) {
                if ((int)mat_src.at<Vec<uchar, 1>>(x + radius, y)[0] == 0) {
                    edge = true;
                }
            }
            if (x + radius < x_max && y + radius < y_max && !edge) {
                if ((int)mat_src.at<Vec<uchar, 1>>(x + radius, y + radius)[0] == 0) {
                    edge = true;
                }
            }
            if (x + radius < x_max && y - radius > 0 && !edge) {
                if ((int)mat_src.at<Vec<uchar, 1>>(x + radius, y - radius)[0] == 0) {
                    edge = true;
                }
            }
            if (x - radius > 0 && y + radius < y_max && !edge) {
                if ((int)mat_src.at<Vec<uchar, 1>>(x - radius, y + radius)[0] == 0) {
                    edge = true;
                }
            }
            if (x - radius > 0 && y - radius > 0 && !edge) {
                if ((int)mat_src.at<Vec<uchar, 1>>(x - radius, y - radius)[0] == 0) {
                    edge = true;
                }
            }

            //If it's not an edge pixel, add to itself and the neighbor pixels at the radius distance
            if (!edge) {
                
                if (y - radius > 0) {
                    mat_out.at<Vec<uchar, 1>>(x, y - radius)[0] = (int)mat_out.at<Vec<uchar, 1>>(x, y - radius)[0] + add_amt;
                    if ((int)mat_out.at<Vec<uchar, 1>>(x, y - radius)[0] > 255) {
                        mat_out.at<Vec<uchar, 1>>(x, y - radius)[0] = 255;
                    }
                }
                if (y + radius < y_max) {
                    mat_out.at<Vec<uchar, 1>>(x, y + radius)[0] = (int)mat_out.at<Vec<uchar, 1>>(x, y + radius)[0] + add_amt;
                    if ((int)mat_out.at<Vec<uchar, 1>>(x, y + radius)[0] > 255) {
                        mat_out.at<Vec<uchar, 1>>(x, y + radius)[0] = 255;
                    }
                }
                if (x - radius > 0) {
                    mat_out.at<Vec<uchar, 1>>(x - radius, y)[0] = (int)mat_out.at<Vec<uchar, 1>>(x - radius, y)[0] + add_amt;
                    if ((int)mat_out.at<Vec<uchar, 1>>(x - radius, y)[0] > 255) {
                        mat_out.at<Vec<uchar, 1>>(x - radius, y)[0] = 255;
                    }
                }
                if (x + radius < x_max) {
                    mat_out.at<Vec<uchar, 1>>(x + radius, y)[0] = (int)mat_out.at<Vec<uchar, 1>>(x + radius, y)[0] + add_amt;
                    if ((int)mat_out.at<Vec<uchar, 1>>(x + radius, y)[0] > 255) {
                        mat_out.at<Vec<uchar, 1>>(x + radius, y)[0] = 255;
                    }
                }
                p[0] = (int)p[0] + add_amt;
                if ((int)p[0] > 255) {
                    p[0] = 255;
                }
            } else {
                p[0] = add_amt;
            }
 
        }
    }
};

int main() {
    
    cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_SILENT);

    //Read in image and display attributes
    Mat image = cv::imread("beans.png", IMREAD_GRAYSCALE);
    w = image.cols;
    h = image.rows;
    c = image.channels();
    cout << w << " x " << h << " :: " << c << " channels " << endl;

    //Define matrix with threshold RGB value
    //All pixels falling below this value will be converted to black
    Mat mat_threshold(w, h, CV_8U,230);
    Mat mat_res = image - mat_threshold;
    mat_res *= 255;

    //Invert the previous result (black to white and vice-versa)
    Mat mat_inv(w, h, CV_8U, 255);
    mat_res = mat_inv - mat_res;
    //Reduce the white pixels to a lower value to allow for additions later
    mat_res *= 0.1;

    //Copy the current image Matrix to the global reference Matrix and the final output Matrix
    mat_src = mat_res;
    mat_out = mat_res;

    //Because the forEach operates in parallel, it's run multiple times with different radii,
    //with each progressive pass adding more value to the inner pixels and leaving the outer pixels darker
    for (int i = 1; i <= 5; i++) {
        mat_out.forEach<Vec<uchar, 1>>(DistanceTransform(i));
    }

    //Show the image result in a window
    imshow("I;m thinkin bout thos Beans", mat_out);
    waitKey(0);

    return 0;
}
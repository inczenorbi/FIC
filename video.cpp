#include <sstream>
#include <string>
#include <iostream>
//#include <opencv2\highgui.h>
#include "opencv2/highgui/highgui.hpp"
//#include <opencv2\cv.h>
#include "opencv2/opencv.hpp"


#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <opencv2/core/core.hpp>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>

#include "func.c"

#define cx 416
#define cy 307
#define cr 261

using namespace std;
using namespace cv;
//initial min and max HSV filter values.
//these will be changed using trackbars
int X_MIN = 0;
int X_MAX = 1080;
int Y_MIN = 0;
int Y_MAX = 1024;

int BH_MIN = 0;
int BH_MAX = 256;
int BS_MIN = 0;
int BS_MAX = 256;
int BV_MIN = 0;
int BV_MAX = 256;

//default capture width and height
const int FRAME_WIDTH = 640;
const int FRAME_HEIGHT = 480;
//max number of objects to be detected in frame
const int MAX_NUM_OBJECTS = 50;
//minimum and maximum object area
const int MIN_OBJECT_AREA = 20 * 20;
const int MAX_OBJECT_AREA = FRAME_HEIGHT*FRAME_WIDTH / 1.5;
//names that will appear at the top of each window
const std::string windowName = "Original Image";
const std::string windowName1 = "HSV Image";
const std::string windowName2 = "Thresholded Image";
const std::string windowName3 = "After Morphological Operations";
const std::string trackbarWindowName = "Trackbars";


int socketfd, portno, n;	
struct hostent *server;
struct sockaddr_in serv_addr;

int socket(char *port) {
	char hostIP[] = "193.226.12.217";
	portno = atoi(port);

	socketfd = socket(AF_INET, SOCK_STREAM, 0);
	if(socketfd < 0) {
		cout<<"Socket error!\n"<<endl;
		return 1;
	}

	server = gethostbyname(hostIP);
	if(server == NULL) {
		cout<<"Server error!\n"<<endl;
		return 1;
	}

	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;

	bcopy((char *) server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
	serv_addr.sin_port = htons(portno);

	if(connect(socketfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr))<0){
		cout<<"Connection error!\n"<<endl;
		return 1;
	}
	return 0;
}

void on_mouse(int e, int x, int y, int d, void *ptr) {
	if (e == EVENT_LBUTTONDOWN) {
		cout << "Left button of the mouse is clicked - position (" << x << ", " << y << ")" << endl;
	}
}

void on_trackbar(int, void*) {
    //This function gets called whenever a
    // trackbar position is changed
}

string intToString(int number) {
	std::stringstream ss;
	ss << number;
	return ss.str();
}

void createTrackbars() {
	//create window for trackbars
	namedWindow(trackbarWindowName, 0);
	//create memory to store trackbar name on window
	char TrackbarName[50];
	sprintf(TrackbarName, "H_MIN", BH_MIN);
	sprintf(TrackbarName, "H_MAX", BH_MAX);
	sprintf(TrackbarName, "S_MIN", BS_MIN);
	sprintf(TrackbarName, "S_MAX", BS_MAX);
	sprintf(TrackbarName, "V_MIN", BV_MIN);
	sprintf(TrackbarName, "V_MAX", BV_MAX);

	sprintf(TrackbarName, "X", X_MIN);
	sprintf(TrackbarName, "Y", Y_MIN);
	//create trackbars and insert them into window
	//3 parameters are: the address of the variable that is changing when the trackbar is moved(eg.H_LOW),
	//the max value the trackbar can move (eg. H_HIGH),
	//and the function that is called whenever the trackbar is moved(eg. on_trackbar)
	//                                  ---->    ---->     ---->
	createTrackbar("H_MIN", trackbarWindowName, &BH_MIN, BH_MAX, on_trackbar);
	createTrackbar("H_MAX", trackbarWindowName, &BH_MAX, BH_MAX, on_trackbar);
	createTrackbar("S_MIN", trackbarWindowName, &BS_MIN, BS_MAX, on_trackbar);
	createTrackbar("S_MAX", trackbarWindowName, &BS_MAX, BS_MAX, on_trackbar);
	createTrackbar("V_MIN", trackbarWindowName, &BV_MIN, BV_MAX, on_trackbar);
	createTrackbar("V_MAX", trackbarWindowName, &BV_MAX, BV_MAX, on_trackbar);

	createTrackbar("X", trackbarWindowName, &X_MIN, X_MAX, on_trackbar);
	createTrackbar("Y", trackbarWindowName, &Y_MIN, Y_MAX, on_trackbar);
}

void drawObject(int x, int y, Mat &frame) {
	//use some of the openCV drawing functions to draw crosshairs
	//on your tracked image!

	//UPDATE:JUNE 18TH, 2013
	//added 'if' and 'else' statements to prevent
	//memory errors from writing off the screen (ie. (-25,-25) is not within the window!)

	circle(frame, Point(x, y), 20, Scalar(0, 255, 0), 2);
	if (y - 25 > 0)
		line(frame, Point(x, y), Point(x, y - 25), Scalar(0, 255, 0), 2);
	else line(frame, Point(x, y), Point(x, 0), Scalar(0, 255, 0), 2);
	if (y + 25 < FRAME_HEIGHT)
		line(frame, Point(x, y), Point(x, y + 25), Scalar(0, 255, 0), 2);
	else line(frame, Point(x, y), Point(x, FRAME_HEIGHT), Scalar(0, 255, 0), 2);
	if (x - 25 > 0)
		line(frame, Point(x, y), Point(x - 25, y), Scalar(0, 255, 0), 2);
	else line(frame, Point(x, y), Point(0, y), Scalar(0, 255, 0), 2);
	if (x + 25 < FRAME_WIDTH)
		line(frame, Point(x, y), Point(x + 25, y), Scalar(0, 255, 0), 2);
	else line(frame, Point(x, y), Point(FRAME_WIDTH, y), Scalar(0, 255, 0), 2);

	putText(frame, intToString(x) + "," + intToString(y), Point(x, y + 30), 1, 1, Scalar(0, 255, 0), 2);
	//cout << "x,y: " << x << ", " << y;
}

void morphOps(Mat &thresh) {
	//create structuring element that will be used to "dilate" and "erode" image.
	//the element chosen here is a 3px by 3px rectangle
	Mat erodeElement = getStructuringElement(MORPH_RECT, Size(3, 3));
	//dilate with larger element so make sure object is nicely visible
	Mat dilateElement = getStructuringElement(MORPH_RECT, Size(8, 8));

	erode(thresh, thresh, erodeElement);
	erode(thresh, thresh, erodeElement);

	dilate(thresh, thresh, dilateElement);
	dilate(thresh, thresh, dilateElement);
}

void trackFilteredObject(int &x, int &y, Mat threshold, Mat &cameraFeed) {

	Mat temp;
	threshold.copyTo(temp);
	//these two vectors needed for output of findContours
	vector< vector<Point> > contours;
	vector<Vec4i> hierarchy;
	//find contours of filtered image using openCV findContours function
	findContours(temp, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);
	//use moments method to find our filtered object
	double refArea = 0;
	bool objectFound = false;
	if (hierarchy.size() > 0) {
		int numObjects = hierarchy.size();
		//if number of objects greater than MAX_NUM_OBJECTS we have a noisy filter
		if (numObjects < MAX_NUM_OBJECTS) {
			for (int index = 0; index >= 0; index = hierarchy[index][0]) {

				Moments moment = moments((cv::Mat)contours[index]);
				double area = moment.m00;

				//if the area is less than 20 px by 20px then it is probably just noise
				//if the area is the same as the 3/2 of the image size, probably just a bad filter
				//we only want the object with the largest area so we safe a reference area each
				//iteration and compare it to the area in the next iteration.
				if (area > MIN_OBJECT_AREA && area<MAX_OBJECT_AREA && area>refArea) {
					x = moment.m10 / area;
					y = moment.m01 / area;
					objectFound = true;
					refArea = area;
				}
				else objectFound = false;
			}
			//let user know you found an object
			if (objectFound == true) {
				putText(cameraFeed, "Tracking Object", Point(0, 50), 2, 1, Scalar(0, 255, 0), 2);
				//draw object location on screen
				//cout << x << "," << y;
				drawObject(x, y, cameraFeed);
			}
		}
		else putText(cameraFeed, "TOO MUCH NOISE! ADJUST FILTER", Point(0, 50), 1, 2, Scalar(0, 0, 255), 2);
	}
}

int main(int argc, char* argv[]) {
	//some boolean variables for different functionality within this
	//program
	bool trackObjects = true;
	bool useMorphOps = true;

  char buffer[10];
	
  if(socket(argv[1])) return 1;

	Point p;
	//Matrix to store each frame of the webcam feed
	Mat cameraFeed;
	//matrix storage for HSV image
	Mat HSV;
	//matrix storage for binary threshold image
	Mat threshold;
	//x and y values for the location of the object
	int x, y;
	//create slider bars for HSV filtering
	createTrackbars();
	//video capture object to acquire webcam feed
	VideoCapture capture;
	//open capture object at location zero (default location for webcam)
	capture.open("rtmp://172.16.254.63/live/live");
	//set height and width of capture frame
	capture.set(CV_CAP_PROP_FRAME_WIDTH, FRAME_WIDTH);
	capture.set(CV_CAP_PROP_FRAME_HEIGHT, FRAME_HEIGHT);
	//start an infinite loop where webcam feed is copied to cameraFeed matrix
	//all of our operations will be performed within this loop

    // Allocate our points and the playground
    area *c;
    point *o = NULL, *d = NULL, *e = NULL;
    int oc[6], dc[6], ec[6];

    o = new point();
    e = new point();
    d = new point();
    c = new area();
    if(!o || !e || !d || !c) {
        printf("Allocation error!\n");
        return 1;
    }
    c->o = new point();
    if(!c->o) {
        printf("Allocation error!\n");
        return 1;
    }

    if(argc != 5) { // We need to get the R manually
        while(1) {
            capture.read(cameraFeed);
            if(cameraFeed.empty ()!= 0) {
                return 1;
            }
            else {
                cvtColor(cameraFeed, HSV, COLOR_BGR2HSV);
                inRange(HSV, Scalar(BH_MIN, BS_MIN, BV_MIN), Scalar(BH_MAX, BS_MAX, BV_MAX), threshold); // We need to get the center of the black area
                drawObject(X_MIN, Y_MIN, cameraFeed); // We can draw the object where we want
                if (useMorphOps)
                    morphOps(threshold);
                if (trackObjects)
                    trackFilteredObject(x, y, threshold, cameraFeed);
                drawObject(x, y, cameraFeed);

                imshow(windowName2, threshold);
                imshow(windowName, cameraFeed);
                //imshow(windowName1, HSV);
                setMouseCallback("Original Image", on_mouse, &p);
                
            //socket transmision
          
            
            if(connect(socketfd,(struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
                perror("ERROR connecting");    
            sprintf(buffer,"%s" ,"r"); // else GO GO GO!
            n = write(socketfd, "r", 1);
            
            if(n < 0)
                perror("Error writing");

            setMouseCallback("Original Image", on_mouse, &p);

            bzero(buffer, 256);
            
                waitKey(30);
            }
        }
        return 0;
    }
    // Initialization:
    c->o->x = cx;
    c->o->y = cy;
    c->r = cr;
    getColor(argv[2], oc);
    getColor(argv[3], dc);
    getColor(argv[4], ec);

	while (1) {
        //store image to matrix
        capture.read(cameraFeed);
        if(cameraFeed.empty ()!= 0) {
            return 1;
        }
        else {
            //convert frame from BGR to HSV colorspace
            cvtColor(cameraFeed, HSV, COLOR_BGR2HSV);
            //filter HSV image between values and store filtered image to threshold matrix

		x = 0;
		y = 0;
            inRange(HSV, Scalar(oc[0], oc[2], oc[4]), Scalar(oc[1], oc[3], oc[5]), threshold); // Detecting our position
            //perform morphological operations on thresholded image to eliminate noise
            //and emphasize the filtered object(s)
            if (useMorphOps)
                morphOps(threshold);
            //pass in thresholded frame to our object tracking function
            //this function will return the x and y coordinates of the
            //filtered object
            if (trackObjects)
                trackFilteredObject(x, y, threshold, cameraFeed);
            o->x = x;
            o->y = y;

            inRange(HSV, Scalar(dc[0], dc[2], dc[4]), Scalar(dc[1], dc[3], dc[5]), threshold); // Detecting our direction
            //perform morphological operations on thresholded image to eliminate noise
            //and emphasize the filtered object(s)
            if (useMorphOps)
            morphOps(threshold);
            //pass in thresholded frame to our object tracking function
            //this function will return the x and y coordinates of the
            //filtered object
            if (trackObjects)
            trackFilteredObject(x, y, threshold, cameraFeed);
            d->x = x;
            d->y = y;

		x = 0;
		y = 0;
            inRange(HSV, Scalar(ec[0], ec[2], ec[4]), Scalar(ec[1], ec[3], ec[5]), threshold); // Detecting the enemy's position
            //perform morphological operations on thresholded image to eliminate noise
            //and emphasize the filtered object(s)
            if (useMorphOps)
                morphOps(threshold);
            //pass in thresholded frame to our object tracking function
            //this function will return the x and y coordinates of the
            //filtered object
            if (trackObjects)
                trackFilteredObject(x, y, threshold, cameraFeed);
            e->x = x;
            e->y = y;

            //show frames
            //imshow(windowName2, threshold);
            imshow(windowName, cameraFeed);
            //imshow(windowName1, HSV);

            //socket transmision
            bzero(buffer, 256);
            
            if(checkWinLoseCondition(e, o, c) || (e->x == 0 && e->y == 0) || (o->x == 0 && o->y == 0))
                sprintf(buffer, "s");           // if somebody won the game, our robot stops
            else
                sprintf(buffer, "%s", chooseAction(c, o, d, e)); // else GO GO GO!

            strcat(buffer, "s");
            n = write(socketfd, buffer, strlen(buffer));
            if(n < 0)
                perror("Error writing");

            setMouseCallback("Original Image", on_mouse, &p);
            //delay 30ms so that screen can refresh.
            //image will not appear without this waitKey() command
            waitKey(30);
        }
	}

	return 0;
}

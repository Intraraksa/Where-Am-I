#include "ros/ros.h"
#include "ball_chaser/DriveToTarget.h"
#include <sensor_msgs/Image.h>
#include <std_msgs/Float64.h>

// Define a global client that can request services
ros::ServiceClient client;

// This function calls the command_robot service to drive the robot in the specified direction
void drive_robot(float lin_x, float ang_z)
{
    // TODO: Request a service and pass the velocities to it to drive the robot
    ball_chaser::DriveToTarget srv;
    srv.request.linear_x = lin_x;
    srv.request.angular_z = ang_z;
    if (!client.call(srv)){
    }
}

// This callback function continuously executes and reads the image data
void process_image_callback(const sensor_msgs::Image img)
{
    //int white_pixel[3][1][1] = {{255},{225},{225}};
    int white_pixel = 255;
    // TODO: Loop through each pixel in the image and check if there's a bright white one
    // Then, identify if this pixel falls in the left, mid, or right side of the image
    // Depending on the white ball position, call the drive_bot function and pass velocities to it
    // Request a stop when there's no white ball seen by the camera
    int  white_ball_located = -1;//assume there isn't a ball
    int  left,right;
    float lin_x = 0 , ang_z = 0;//if there isn't a ball
    //image step = 2400 
    //interate each pixel
    //ROS_INFO("%d",img.encoding);

//(img.data[2][i][0] == white_pixel[2][0][0]&&img.data[1][i][0] == white_pixel[1][0][0]&&img.data[0][i][0] == white_pixel[0][0][0])


    for (int i = 0; i < img.height * img.step; i = i+3) {
        if (img.data[i] == white_pixel && img.data[i+1] == white_pixel && img.data[i+2] == white_pixel) { //rgbrgbrgb
            white_ball_located = i % img.step;//find the location of white ball
            //int a = sizeof(img.data[0])/sizeof(int);
            //ROS_INFO("%d",img.data[1920000]);
            break;
        }
    }       
    //seperate into 3 area 
    //ROS_INFO("%d",img.data[i]);
    left = img.step * 0.3;
    right = img.step *0.7;
    //left
    if (white_ball_located <= left && white_ball_located >= 0) {
        lin_x = 0, ang_z = 0.5;}
    //right
    else if (white_ball_located >= right){
        lin_x = 0,ang_z = -0.5;}
    //mid
    else if (white_ball_located >left && white_ball_located < right) {
        lin_x = 0.2;} 

    // I try to stop robot when too much close to a ball but fail
    else if (white_ball_located <= left && white_ball_located >= right){
        lin_x = 0 , ang_z = 0;}
    drive_robot(lin_x, ang_z);
}

int main(int argc, char** argv)
{
    // Initialize the process_image node and create a handle to it
    ros::init(argc, argv, "process_image");
    ros::NodeHandle n;

    // Define a client service capable of requesting services from command_robot
    client = n.serviceClient<ball_chaser::DriveToTarget>("/ball_chaser/command_robot");

    // Subscribe to /camera/rgb/image_raw topic to read the image data inside the process_image_callback function
    ros::Subscriber sub1 = n.subscribe("/camera/rgb/image_raw", 10, process_image_callback);

    // Handle ROS communication events
    ros::spin();

    return 0;
}

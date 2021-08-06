//This header takes care of all ROS functions:
#include "ros.h"
#include "std_msgs/String.h"
#define ESP32

//ROS DEFINITIONS =============================================================================
ros::NodeHandle LED;

// //ROS motor control
void leftLEDcontrol(const std_msgs::String& msg1); 
void rightLEDcontrol(const std_msgs::String& msg2);

char leftLEDcommand[] = "";
char rightLEDcommand[] = "";

ros::Subscriber<std_msgs::String> LeftLEDstrip("/led/left/cmd", leftLEDcontrol); 
ros::Subscriber<std_msgs::String> RightLEDstrip("/led/right/cmd", rightLEDcontrol); 

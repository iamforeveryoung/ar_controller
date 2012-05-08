/* Copyright (c) 2012, individual contributors
*
* Permission to use, copy, modify, and/or distribute this software for any
* purpose with or without fee is hereby granted, provided that the above
* copyright notice and this permission notice appear in all copies.
*
* THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
* WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
* MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
* ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
* WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
* ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
* OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/

/*
Written for the Julius Lab at Rensselaer Polytechnic Institute, Troy, NY 12180
Please cite if you use this code

Allows reading of AR Drone Navdata and the sending of linear and angular velocity commands to the drone
*/
 
#include "ros/ros.h"
#include "std_msgs/String.h"
#include "std_msgs/Empty.h"
#include "geometry_msgs/Twist.h"
#include "ardrone_brown/Navdata.h"
#include "time.h"

class navHandler
{
private:
int cur_alt;
float cur_rotX;
float cur_rotY;
float cur_rotZ;
float cur_vx;
float cur_vy;
float cur_vz;

ros::NodeHandle p;
ros::Publisher pub1; //cmd_vel Twist
ros::Publisher pub2; //takeoff Empty
ros::Publisher pub3; //land    Empty 
ros::Publisher pub4; //reset   Empty

public:
            
    //constructor, creates publishers that can publish commands to the appropriate topics
    navHandler()
    {
        pub1 = p.advertise<geometry_msgs::Twist>("cmd_vel", 100);
        pub2 = p.advertise<std_msgs::Empty>("ardrone/takeoff", 100);
        pub3 = p.advertise<std_msgs::Empty>("ardrone/land", 100);
        pub4 = p.advertise<std_msgs::Empty>("ardrone/reset", 100);
        
    }

    //sends msg to start takeoff
    void launcher()
    {
        std_msgs::Empty launch_msg;
        pub2.publish(launch_msg);
        ROS_INFO("Takeoff");         
    }
    
    //sends msg to land
    void lander()
    {
        std_msgs::Empty land_msg;
        pub3.publish(land_msg);
        ROS_INFO("Landing");
    }
    
    //sends msg to reset
    void reset()
    {
        std_msgs::Empty reset_msg;
        pub4.publish(reset_msg);
        ROS_INFO("Reset");
    }
    
    //sends trajectory control commands
    void sendCmd(float lx, float ly, float lz, float ax, float ay, float az)
    {
        //sends velocity in m/s - values should be between 1 and -1
        geometry_msgs::Twist cmd_msg;
        cmd_msg.linear.x = lx;
        cmd_msg.linear.y = ly;
        cmd_msg.linear.z = lz;
        
        //sends velcoity in radians/s
        cmd_msg.angular.x = ax; //not used - tilt used for linear motion
        cmd_msg.angular.y = ay; //not used - tilt used for linear motion
        cmd_msg.angular.z = az;
        
        pub1.publish(cmd_msg);
        
    }
    
    //send all velocities back to zero
    void stabilize()
    {
        sendCmd(0, 0, 0, 0, 0, 0);
        ROS_INFO("Stabilized");
        
    }
      
    //test responder to received navdata        
    void droneResponder()
    {
        /*
        if(cur_alt < 700 && cur_alt > 300)
        {
            sendCmd(0, 0, .5, 0, 0, 0);
        }       
        else if(cur_alt >= 700)
        {    
            // sendCmd(0, 0, 0, 0, 0, 1);
            lander();
            //stabilize();

        }
        if(cur_alt == 0)
        {
           //stabilize();          
           //launcher();
           
        }
        */
        if(cur_alt < 500)
        {
            lander();
        }
    }
    
    //Callback function, responds to /ardrone/navdata msgs
    void navdataHandler(const ardrone_brown::Navdata::ConstPtr& msg)
    {
      ROS_INFO("Altitude: [%d]", msg->altd);
      cur_alt = msg->altd;
      ROS_INFO("rotX: [%f]", msg->rotX);
      cur_rotX = msg->rotX;
      ROS_INFO("rotY: [%f]", msg->rotY);
      cur_rotY = msg->rotY;
      ROS_INFO("rotZ: [%f]", msg->rotZ);
      cur_rotZ = msg->rotZ;
      ROS_INFO("vx: [%f]", msg->vx);
      cur_vx = msg->vx;
      ROS_INFO("vy: [%f]", msg->vy);
      cur_vy = msg->vy;
      ROS_INFO("vz: [%f]", msg->vz);
      cur_vz = msg->vz;
      
      droneResponder();
    }
};


int main(int argc, char **argv)
{
    //ros initializaton
    ros::init(argc, argv, "arcontroller");
    
    //create a node handle
    ros::NodeHandle n;
    //create instance of responder class
    navHandler nav;
    
    //subscribe to navdata, specify callback function in repsonder class
    ros::Subscriber sub = n.subscribe("ardrone/navdata", 1000, &navHandler::navdataHandler, &nav);
    
    //spin
    ros::spin();
    
    return 0;
    
}


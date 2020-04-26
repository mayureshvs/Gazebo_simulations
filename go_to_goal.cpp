#include "ros/ros.h"
#include "nav_msgs/Odometry.h"
#include "geometry_msgs/Twist.h"
#include "math.h"
#include <tf/tf.h>

struct pos{
  double x;
  double y;
  double theta;  
  pos():x(0),y(0),theta(0){}
  pos(double x_temp, double y_temp,double theta_temp):x(x_temp),y(y_temp),theta(theta_temp){}
};

struct pos current;
struct pos goal(3.0,3.0,0.0);

void odomCallback(const nav_msgs::Odometry::ConstPtr& msg)
{
  current.x=msg->pose.pose.position.x;
  current.y=msg->pose.pose.position.y;
  tf::Quaternion q(
   msg->pose.pose.orientation.x,
   msg->pose.pose.orientation.y,
   msg->pose.pose.orientation.z,
   msg->pose.pose.orientation.w);
   tf::Matrix3x3 m(q);
   double roll, pitch, yaw;
   m.getRPY(roll, pitch, yaw);
   current.theta=yaw;
  ROS_INFO("pos x: [%lf]", msg->pose.pose.position.x);
  ROS_INFO("pos y: [%lf]", msg->pose.pose.position.y);
  //ROS_INFO("pos yaw: [%lf]", yaw);
}


 
double desired_theta(pos &curr, pos &goal)
{
  double theta;
  theta=atan((goal.y-curr.y)/(goal.x-curr.x));
  return theta;
}

ros::Subscriber subscriber_init(){

  ros::NodeHandle n_sub;
  ros::Subscriber sub = n_sub.subscribe("odom", 1000, odomCallback);
  return sub;
}

ros::Publisher publisher_init(){
  ros::NodeHandle n_pub;
  ros::Publisher cmd_pub = n_pub.advertise<geometry_msgs::Twist>("cmd_vel", 1000);
  //ros::Rate loop_rate(10);
  return cmd_pub;
}

int main(int argc, char **argv)
{

  //pos goal(10,10,0);
  //pos current;
  double des_theta;
  //ros::spinOnce();
  ros::init(argc, argv, "go_to_goal");
  ros::Subscriber sub = subscriber_init();
  ros::Publisher cmd_pub = publisher_init();
ros::Rate loop_rate(10);
  ros::spinOnce();
 double ang_vel=1.0;
   geometry_msgs::Twist vel;
  while(ros::ok()){
      
    while(ros::ok()){
          des_theta = desired_theta(current,goal);
    if((des_theta-current.theta)>0)
    {
    ang_vel=1.0;
    }
    else{
    ang_vel=-1.0;    
    }
          ROS_INFO("pos des-theta: [%lf]", des_theta);
          ROS_INFO("pos current-theta: [%lf]", current.theta);
          if(!(current.theta >= (des_theta-0.1) && current.theta <=(des_theta+0.1)))
          {
        vel.linear.x = 0.0;
        vel.angular.z = ang_vel;
        ROS_INFO("if pos z: [%lf]", vel.angular.z);
        cmd_pub.publish(vel);
     ros::spinOnce();
        loop_rate.sleep();
        }
        else{
            vel.linear.x = 0.0;
            vel.angular.z = 0.0;
            ROS_INFO("else pos z: [%lf]", vel.angular.z);
            cmd_pub.publish(vel);
		ros::Rate loop_rate(10);
         ros::spinOnce();
            break;
            }
    }

      if(!(current.x <= (goal.x)+0.1 && current.x>=(goal.x)-0.1 && current.y <= (goal.y)+0.1 && current.y >= (goal.y)-0.1))
      {
        vel.linear.x = 2.0;
    vel.angular.z = 0.0;
        cmd_pub.publish(vel);
        loop_rate.sleep();
        ros::spinOnce();
        
      }else
      {
        vel.linear.x = 0.0;
        cmd_pub.publish(vel);
    ros::spinOnce();
        loop_rate.sleep();
        break;
      }

 
 }

    vel.angular.z = 0.0;
    cmd_pub.publish(vel);
    ROS_INFO("----------------------------------------------------");
 return 0;
}


// This program publish filtered geometry/Twist messages 

#include <ros/ros.h>
#include <geometry_msgs/Twist.h> // For geometry_msgs::Twist
#include <std_srvs/Empty.h>

ros::Publisher *pub;
double max_ang_vel;
bool enable_filter = true;

// A call back function . Executed each time a new turtle1/cmd_vel message arrives.
void twistMessageFilter(const geometry_msgs::Twist& msg) 
{
	static int total_msg = 0;

	if (enable_filter) 
	{
		if(msg.angular.z < max_ang_vel)
		{
			// Publish the filtered message.
			pub->publish(msg);
		} else {
			total_msg++;
		}
	} else 
	{
		// Publish all received message.
		pub->publish(msg);
	}

	ROS_INFO_STREAM_THROTTLE(5.0,"Total dropped messages vel_filter: " << total_msg);
}

bool toogle_fitler(std_srvs::Empty::Request &req, std_srvs::Empty::Response &resp)
{
	enable_filter = !enable_filter;

	ROS_INFO_STREAM( (enable_filter ? "Enable" : "Disable") << " filter");

	return true;
}

int main (int argc, char **argv) 
{
	// Initialize the ROS system and become a node.
	ros::init(argc, argv,"filter_velocity");
	ros::NodeHandle nh;

	// Register our service with the master
	ros::ServiceServer server = nh.advertiseService("toogle_fitler",&toogle_fitler);

	// Create a publisher pointer.
	pub = new ros::Publisher(nh.advertise<geometry_msgs::Twist>("turtle1/cmd_vel_filtered",1000));

	// Create a subscriber object.
	ros::Subscriber sub = nh.subscribe("turtle1/cmd_vel", 1000, &twistMessageFilter);

	const std::string PARAM_NAME = "~max_ang_vel";
	// const std::string PARAM_NAME = "/max_ang_vel";

	// Let ROS take over .
	// ros::spin();

	// Loop at 2Hz until the node is shut down.
	ros::Rate rate(2);

	while(ros::ok())
	{
		// Get the maximum velocity parameter
		if(!ros::param::get(PARAM_NAME,max_ang_vel))
		{
			ROS_FATAL_STREAM("Could not get parameter " << PARAM_NAME);
			exit(1);
		}

		ros::spinOnce();

		// Wait until it's time for another iteration.
		rate.sleep();
	}

	// destroy pointer of objects
	delete pub;

	return 0;
}
#include <ros/ros.h>
#include "serialib.h"
#include "std_msgs/String.h"
#include <sstream>
#include <string>
#include <vector>

#if defined (_WIN32) || defined( _WIN64)
#define         DEVICE_PORT             "COM9"                               // COM1 for windows
#endif

#ifdef __linux__
#define         DEVICE_PORT             "/dev/ttyO1"                         // ttyS0 for linux
#endif


using namespace std;

int main(int argc, char **argv)
{
	serialib LS;                                                            // Object of the serialib class
    int Ret;                                                               // Used for return values
    char Buffer[128];
	//char temp[6];
	std::vector<char> data;
	vector<char>::iterator it;
	
	Ret = LS.Open(DEVICE_PORT,115200); 
	if (Ret!=1) {                                                           // If an error occured...
        printf ("Error while opening port. Permission problem ?\n");        // ... display a message ...
        ROS_INFO("Error while opening port");
		return Ret;                                                         // ... quit the application
    }
	
	
	ros::init(argc, argv, "ReadRoomba");
	ros::NodeHandle nh;
	
	ros::Publisher data_pub = nh.advertise<std_msgs::String>("Roomba_Data", 0);
	
	ros::Rate loop_rate(60);//loop at 66Hz   1/15ms = 66.7 = 66
	
	/*LS.WriteChar(128);
    LS.WriteChar(132);*/
	
	while(ros::ok()) 
	{
		LS.WriteChar(128);
		LS.WriteChar(132);
		LS.WriteChar(129);
		LS.WriteChar(11);
		LS.WriteChar(148);//send command to roomba to send back data
		LS.WriteChar(1);
		LS.WriteChar(7);
		
		Ret=LS.Read(Buffer,5,1000); //read the data from roomba;
		
		
		for (const char* p = Buffer; *p; ++p)
        {
			data.push_back(*p);
        }

		string tempstr;
		int tempint;
		std::stringstream convert;

		
		/*for(int i=0;i<=data.size();i++)
		{
			ROS_INFO_STREAM("Data post:" << (int)data[i]);
		}*/
		
		switch ((int)data[3]) 
	{
		case 1:
			LS.WriteChar(128);
			LS.WriteChar(132);
			LS.WriteChar(146);
			LS.WriteChar(0);
			LS.WriteChar(255);
			LS.WriteChar(255);
			LS.WriteChar(1);
			break;
		case 2:
			LS.WriteChar(128);
			LS.WriteChar(132);
			LS.WriteChar(146);
			LS.WriteChar(255);
			LS.WriteChar(1);
			LS.WriteChar(0);
			LS.WriteChar(255);
			break;
		case 3:
			LS.WriteChar(128);
			LS.WriteChar(132);
			LS.WriteChar(146);
			LS.WriteChar(255);
			LS.WriteChar(1);
			LS.WriteChar(0);
			LS.WriteChar(255);
			break;
		default:
			LS.WriteChar(128);
			LS.WriteChar(132);
			LS.WriteChar(146);
			LS.WriteChar(0);
			LS.WriteChar(255);
			LS.WriteChar(0);
			LS.WriteChar(255);
			break;
	}
		
		
		std_msgs::String msg;
		convert<<(int)data[3];
		tempstr = convert.str();
		std::stringstream ss;
		ss.str(tempstr);

		msg.data = ss.str();
		
		data_pub.publish(msg);
		
		data[3] = NULL;
		data.clear();
		
		
		ros::spinOnce();
		loop_rate.sleep();
	}
}

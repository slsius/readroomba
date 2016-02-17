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
	
	Ret = LS.Open(DEVICE_PORT,115200); 
	if (Ret!=1) {                                                           // If an error occured...
        printf ("Error while opening port. Permission problem ?\n");        // ... display a message ...
        ROS_INFO("Error while opening port");
		return Ret;                                                         // ... quit the application
    }
	
	
	ros::init(argc, argv, "ReadRoomba");
	ros::NodeHandle nh;
	
	ros::Publisher data_pub = nh.advertise<std_msgs::String>("Roomba_Data", 1000);
	
	ros::Rate loop_rate(60);//loop at 66Hz   1/15ms = 66.7 = 66
	
	LS.WriteChar(128);
    LS.WriteChar(132);
	
	while(ros::ok()) 
	{
		LS.WriteChar(148);//send command to roomba to send back data
		LS.WriteChar(1);
		LS.WriteChar(7);
		
		Ret=LS.Read(Buffer,5,1000); //read the data from roomba;
		
		
		for (const char* p = Buffer; *p; ++p)
        {
            //printf("%02X\n", *p);
            //cout << (int)*p << endl;
			ROS_INFO_STREAM("P value"<<(int)*p);
			date.push_back(*p);
			//temp[i] = *p;
            //temp[i] = (int)*p;
            //cout <<"P value: " << (int)temp[i] <<"end" <<endl;
        }

		string tempstr;
		int tempint;
		std::stringstream convert;
		/*for(int i=0;i<6;i++)
		{
			//ROS_INFO_STREAM("Packet" << i <<": " << temp[i]);
			ROS_INFO("%02X", temp[i]);
			ROS_INFO_STREAM("        Packet" << i <<": " << (int)temp[i]);
			//tempstr = tempstr + to_string((int)temp[i]);
			convert << (int)temp[i];
			tempstr = tempstr + convert.str();
		}
		ROS_INFO_STREAM(tempstr);
		ROS_INFO_STREAM("***********************");
		*/
		
		for(auto it = data.begin(); it!= data.end();++it)
		{
			ROS_INFO_STREAM("Position"<<it<<": "<<*it);
		}
		
		
		std_msgs::String msg;

		std::stringstream ss;
		ss.str(date[3]);

		msg.data = ss.str();
		
		data_pub.publish(msg);
		
		
		ros::spinOnce();
		loop_rate.sleep();
	}
}

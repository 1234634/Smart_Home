#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <iostream>
#include <thread>
#include <mutex>
#include <map>
#include <iterator>
#include<bits/stdc++.h>

#define DATA_PORT 8080

using namespace std;

class Device
{
    private:
        string room; // room in which is device
        string device_name;
        map<string, string> device_data;// name of data and data itself i.e. map<"TEMPERATURE", "30"> normaly device will have only one
        string device_data_message; // message explaining basic informations about device , name, in which room is it, written in requiered ssdp format
        string data_port;
        mutex m; // mutex used for mutalexclusive acces to device data
    public:

        Device()
        {
            this->device_name = "device";
            this->room = "LIVING ROOM";
            this->device_data["TEMPERATURE"] = "0";
            this->data_port = to_string(DATA_PORT);
        }

        Device(string d_name,string r,string dd1,string dd2,string dp)
        {
            this->device_name=d_name;
            this->room=r;
            this->device_data[dd1]=dd2;
            this->data_port=dp;
        }

	 void set_device_name(string device_name)
        {
            this->device_name = device_name;
        }

        void set_room(string room)
        {
            this->room = room;
        }

        void set_port(string port)
        {
            this->data_port = port;
        }


        string get_room()
        {
            return room;
        }

	string get_device_name()
        {
            return this->device_name;
        }


        string get_topic_value(string topic)
        {
            return this->device_data[topic];
        }
	
	map<string,string> get_device_data()
        {
            return this->device_data;
        }
	
	string get_data_port()
        {
            return this->data_port;
        }


        string get_all_topics()
        {
	    // device topici sta on ima u sebi ("temperature"...)
            string topics;
            for(auto it = this->device_data.begin(); it != this->device_data.end(); it++)
            {
                topics += it->first;
                topics += ",";
            }

            topics[topics.length() - 1] = '\0';
            return topics;
        }



        void set_topic_value(string key, string value)
        {
            m.lock();
            this->device_data[key] = value;
            m.unlock();
        }


        bool topic_exist(string topic)
        {
            map<string, string>::iterator it = this->device_data.find(topic);
            if(it != this->device_data.end())
                return true;

            return false;
        }
};

Device* make_device(string message)
{

        string search_message = message;

        string delimiter = "|";
        cout << search_message << endl;

        string device_name = search_message.substr(0, search_message.find(delimiter));
        search_message.erase(0, search_message.find(delimiter) + delimiter.length());

        string room = search_message.substr(0, search_message.find(delimiter));
        search_message.erase(0, search_message.find(delimiter) + delimiter.length());

        string device_data1 = search_message.substr(0, search_message.find(delimiter));
        search_message.erase(0, search_message.find(delimiter) + delimiter.length());

        string device_data2 = search_message.substr(0, search_message.find(delimiter));
        search_message.erase(0, search_message.find(delimiter) + delimiter.length());

        string data_port = search_message.substr(0, search_message.find(delimiter));
        search_message.erase(0, search_message.find(delimiter) + delimiter.length());

        Device *prepared_device = new Device(device_name,room,device_data1,device_data2,data_port);


        return prepared_device;

}
      

string make_device_info_message(Device* device)
{

    string return_value = device->get_device_name() + "|" + device->get_room() + "|";

    for(auto it = device->get_device_data().begin(); it != device->get_device_data().end(); it++)
    {
        return_value += it->first;
        return_value += "|";
	return_value += it->second;
	return_value += '|';   
    }

    
    return_value += device->get_data_port();
    return return_value;
}

int main()
{
    string mes = "A|B|C|D|E";
    Device *temp = make_device(mes);
    string a = temp->get_device_name();
    cout << make_device_info_message(temp);

    return 0;
}



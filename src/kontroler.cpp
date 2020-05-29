#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <iostream>
#include <thread>
#include <iterator>
#include<bits/stdc++.h>
#include <mutex>
#include <map>
#include"device.cpp"
using namespace std;

//ports
#define BROADCAST_PORT 8080
#define LISTEN_PORT BROADCAST_PORT + 1
#define DATA_PORT LISTEN_PORT + 1


//len
#define FOUND_MESSAGE_LEN 20
#define DEFAULT_BUFLEN 512

//bool found = false;

//const char found_message[] = "Device found!";

//list of sensors
const string fingerprint_sensor = "SWITCH";
const string oven_sensor = "OVEN_SENSOR";
const string temperature_sensor = "TEMPERATURE_SENSOR";
const string air_conditioning = "AIR_CONDITIONING";
const string door_opener = "DOOR_OPENER";
const string light_bulb = "LIGHT_BULB";







map<string,Device*> device_map;


void find_devices()
{

    struct sockaddr_in address;
    int read_size, broadcast_socket;
    char message[DEFAULT_BUFLEN];
    string search_message;
    socklen_t address_size = sizeof(address);

    broadcast_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(broadcast_socket == -1)
    {
        perror("Broadcast socket creation failed");
        return;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(BROADCAST_PORT);

    if(bind(broadcast_socket, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("Failed to bind udp socket.\n");
        return;
    }

    while((read_size = recvfrom(broadcast_socket , message , DEFAULT_BUFLEN , 0, (struct sockaddr*)&address, &address_size)) > 0 )
    {
        message[read_size] = '\0';
        search_message = message;
        thread add_device_thread(add_device, ref(search_message));
        add_device_thread.detach();
    }
}


void get_device_parameters(Device device)
{
    //uzeti port iz sensor
    //napraviti update sensor funkciju koja update sve topice u sensor.map
    //pozvati fju

    //while 1 recv parsirati poruku
    //pozvati update sensor funkciju u threadu!!!!! thread d(parametri), d.detach()!!!!!!!!!!
}

void update_device(Device device)
{
    //postavlja sve vrednosti topica u mapi na date vrednosti;
}



/*int main()
{


    find_devices();
// thread find sensors




//join find sensors thread


}*/

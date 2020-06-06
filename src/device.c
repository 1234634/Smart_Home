
#define BUFFER_SIZE 100

typedef struct Device_descriptor
{
	char id[BUFFER_SIZE];
	char group[BUFFER_SIZE];
	char value[BUFFER_SIZE];
	int gpio_pin;  
	char topic[BUFFER_SIZE]; 
	char info[BUFFER_SIZE]; // device description
    int condition; // condition for actuators automatisation

}Device;

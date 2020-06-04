
#define BUFFER_SIZE 100

typedef struct Device_descriptor
{
	char id[BUFFER_SIZE];
	char group[BUFFER_SIZE];
	char value[BUFFER_SIZE];
	char gpio_pin[BUFFER_SIZE];
	char topic[BUFFER_SIZE];
}Device;

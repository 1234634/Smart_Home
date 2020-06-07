#include "mqtt_helper.h"



#define ADDR "test.mosquitto.org"
#define PORT "1883"
#define SENSORS "sensors"
#define ACTUATORS "actuators"
#define MAX_DEVICES 20
#define SENSORS_READ_DELAY 3
#define CONTROLER_TOPIC "controler"
#define DEVICES_INFO_TOPIC "devices/info"//device tema bez oznakom funkcionalnosti
#define DEVICES_FUNC_TOPIC "devices/func" //device tema sa oznakom funkcionalnosti
#define GET_DEV_VALUE "Get_Dev_Value"
#define SET_DEV_VALUE "Set_Dev_Value"
#define GET_DEV_INFO "Get_Dev_Info"
#define SET_DEV_INFO "Set_Dev_Info"
#define PROPERTY_CHANGED "PropertyChanged"
#define SENSOR_MES "Sensor_Mes"
#define DELIMITER '.'

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

struct pub_packet
{
    struct mqtt_client* client;
    int socket;
    pthread_t* client_daemon;

};

struct callback_packets
{
    char* topic;
    char* mes;

};
void client_error_check(struct pub_packet * arg_packet)
{
    
        if ((*(arg_packet->client)).error != MQTT_OK)
         {
             fprintf(stderr, "error: %s\n", mqtt_error_str((*(arg_packet->client)).error));
             exit_example(EXIT_FAILURE, arg_packet->socket, arg_packet->client_daemon);
         }



}

char* check_device_condition(int arg_cond, int arg_value )
{
           if(arg_value > arg_cond)
           {
                return "ON";
           }
            else
            {
                return "OFF";
            }

}

void set_dev_value(char** arg_tokens, Device(* arg_devices)[20],struct pub_packet * arg_packet,int arg_last_elem_index)
{
    char * id = *(arg_tokens+1); 
    char * new_value = *(arg_tokens+2); 
    char message[400];


    int i;
    for ( i = 0; i < arg_last_elem_index; i++)
    {
    
        if( strcmp((*arg_devices)[i].id, id) == 0 )
        {  
	        strcpy((*arg_devices)[i].value,new_value);

	    	sprintf(message,"%s.%s.value.%s",PROPERTY_CHANGED,(*arg_devices)[i].id,(*arg_devices)[i].value);
	    	mqtt_publish(arg_packet->client,CONTROLER_TOPIC , message, strlen( message) + 1, MQTT_PUBLISH_QOS_0);
	    	printf(" published : topic:%s; message: %s \n",CONTROLER_TOPIC, message);


		 client_error_check(arg_packet);

        }
         
    }
}


void set_dev_info(char** arg_tokens, Device(* arg_devices)[20],struct pub_packet * arg_packet,int arg_last_elem_index)
{
    char * id = *(arg_tokens+1); 
    char * new_info = *(arg_tokens+2); 
    char message[400];


    int i;
    for ( i = 0; i < arg_last_elem_index; i++)
    {
    
        if( strcmp((*arg_devices)[i].id, id) == 0 )
        {  
	        strcpy((*arg_devices)[i].info,new_info);

	    	sprintf(message,"%s.%s.info.%s",PROPERTY_CHANGED,(*arg_devices)[i].id,(*arg_devices)[i].info);
	    	mqtt_publish(arg_packet->client,CONTROLER_TOPIC , message, strlen( message) + 1, MQTT_PUBLISH_QOS_0);
	    	printf(" published : topic:%s; message: %s \n",CONTROLER_TOPIC, message);


		 client_error_check(arg_packet);

        }
         
    }
}


void automation_control(char** arg_tokens, char* arg_topic,Device(* arg_devices)[20],struct pub_packet * arg_packet,int arg_last_elem_index)
{
    const char split[2] = "l"; 
    char * value_token = strtok(*(arg_tokens +1),split); // "25lr" -> 25
    int value =atoi(value_token);
    char message[400];


    int i;
    char new_dev_value[5];
    for ( i = 0; i < arg_last_elem_index; i++)
    {
    
        if( (strcmp((*arg_devices)[i].topic, arg_topic) == 0) &&  (strcmp((*arg_devices)[i].group, ACTUATORS) == 0)  )
        {
        
            strcpy(new_dev_value, check_device_condition((*arg_devices)[i].condition,value));
          
	    if( strcmp((*arg_devices)[i].value, new_dev_value) != 0)
            { 
		strcpy((*arg_devices)[i].value,new_dev_value);
		sprintf(message,"%s.%s.value.%s",PROPERTY_CHANGED,(*arg_devices)[i].id,(*arg_devices)[i].value);
		mqtt_publish(arg_packet->client,CONTROLER_TOPIC , message, strlen( message) + 1, MQTT_PUBLISH_QOS_0);
		printf(" published : topic:%s; message: %s \n",CONTROLER_TOPIC, message);


		 client_error_check(arg_packet);

             }
         
        }
    }
}



//reads value from GPIO PIN
char* read_pin(int arg_pin )
{      
        srand(time(0)); 
        int value = (rand()%10 + 20);

        static char temp[20];
        sprintf(temp,"%dlr",value);
        return temp;
}
void* update_sensors_value(int arg_index, Device(* arg_devices)[20],struct pub_packet * arg_packet)
{

    char * new_value = read_pin((*arg_devices)[arg_index].gpio_pin);
    char message[400];

    if( strcmp(new_value,(*arg_devices)[arg_index].value) != 0)
    {
        strcpy((*arg_devices)[arg_index].value,new_value);

        sprintf(message,"%s.%s.value.%s",PROPERTY_CHANGED,(*arg_devices)[arg_index].id,(*arg_devices)[arg_index].value);
        mqtt_publish(arg_packet->client, CONTROLER_TOPIC, message, strlen( message) + 1, MQTT_PUBLISH_QOS_0);
        printf(" published : topic:%s; message: %s \n",CONTROLER_TOPIC, message);

        client_error_check(arg_packet);
    }
    return NULL;

}


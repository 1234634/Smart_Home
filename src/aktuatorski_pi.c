#include<time.h> 
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include<string.h>
#include <mqtt.h>
#include "templates/posix_sockets.h"
//#include "mqtt_helper.h"
#include "device.c"
#include"parse.h"

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
#define SENZORSKI_PI_TOPIC "senzorski_pi"
#define AKTUATORSKI_PI_TOPIC "aktuatorski_pi"


static Device Devices[MAX_DEVICES];
static int last_elem_index = 0; 


void* distribute_pub_message(void* );
void publish_callback(void** , struct mqtt_response_publish *); 
//updates states of sensors and publishes change to controler
//void* update_sensors_value(int ); 

//publishes sensors states on every 3 seconds
void * sensors_value_publish(void * );


//sets value of addr,port,topic through command line arguments
void set_arguments(const char**,const char**,const char**,int,const char*[] );





static struct callback_packets callback_packet;
static struct pub_packet packet;


int main(int argc, const char *argv[]) 
{

/********************* INITIALISATION ********************************/
    
    const char* addr= ADDR;
    const char* port = PORT;

    /* add new device*/
    if (argc > 1) {
        addr = argv[1];
    } else {
        addr = "test.mosquitto.org";
    }

    /* open the non-blocking TCP socket (connecting to the broker) */
    int sockfd = open_nb_socket(addr, port);

    if (sockfd == -1) {
        perror("Failed to open socket: ");
        exit_example(EXIT_FAILURE, sockfd, NULL);
    }

    /* setup a client */
    struct mqtt_client client;
    uint8_t sendbuf[2048]; /* sendbuf should be large enough to hold multiple whole mqtt messages */
    uint8_t recvbuf[1024]; /* recvbuf should be large enough any whole mqtt message expected to be received */
    mqtt_init(&client, sockfd, sendbuf, sizeof(sendbuf), recvbuf, sizeof(recvbuf), publish_callback);
    /* Create an anonymous session */
    const char* client_id = NULL;
    /* Ensure we have a clean session */
    uint8_t connect_flags = MQTT_CONNECT_CLEAN_SESSION;
    /* Send connection request to the broker. */
    mqtt_connect(&client, client_id, NULL, NULL, 0, NULL, NULL, connect_flags, 400);

    /* check that we don't have any errors */
    if (client.error != MQTT_OK) {
        fprintf(stderr, "error: %s\n", mqtt_error_str(client.error));
        exit_example(EXIT_FAILURE, sockfd, NULL);
    }

    /* start a thread to refresh the client (handle egress and ingree client traffic) */
    pthread_t client_daemon;
    if(pthread_create(&client_daemon, NULL, client_refresher, &client)) {
        fprintf(stderr, "Failed to start client daemon.\n");
        exit_example(EXIT_FAILURE, sockfd, NULL);

    }
    
    packet.client = &client;
    packet.socket = sockfd;
    packet.client_daemon = &client_daemon;
/********************* END_OF_INITIALISATION**************************/

    
   /*Device temperature_aktuator;
    strcpy(temperature_aktuator.id,"temp_aktuator_lroom");
    strcpy(temperature_aktuator.group,ACTUATORS);
    strcpy(temperature_aktuator.value,"OFF");
    temperature_aktuator.gpio_pin = 15;
    strcpy(temperature_aktuator.topic,"home/living_room/temperature");
    snprintf(temperature_aktuator.info,250,"id: %s; Group: %s Controlable: %s",temperature_aktuator.id,temperature_aktuator.group,"Yes"); 
    temperature_aktuator.condition = 25;
    Devices[last_elem_index++] = temperature_aktuator;
*/
    Device bedroom_temp_actuator;
         strcpy(bedroom_temp_actuator.id, "temp_aktuator_bedroom_1");
         strcpy(bedroom_temp_actuator.group, ACTUATORS);
         strcpy(bedroom_temp_actuator.value, "OFF");
         bedroom_temp_actuator.gpio_pin = 15;
         strcpy(bedroom_temp_actuator.topic, "home/bedroom11/temperature");
         snprintf(bedroom_temp_actuator.info, 250,"id: %s; Group: %s Controlable: %s", bedroom_temp_actuator.id, bedroom_temp_actuator.group, "Yes");
         bedroom_temp_actuator.condition = 25;
         Devices[last_elem_index++] = bedroom_temp_actuator;

    Device bedroom_temp_actuator_2;
         strcpy(bedroom_temp_actuator_2.id, "temp_aktuator_bedroom_2");
         strcpy(bedroom_temp_actuator_2.group, ACTUATORS);
         strcpy(bedroom_temp_actuator_2.value, "OFF");
         bedroom_temp_actuator_2.gpio_pin = 15;
         strcpy(bedroom_temp_actuator_2.topic, "home/bedroom22/temperature");
         snprintf(bedroom_temp_actuator_2.info, 250,"id: %s; Group: %s Controlable: %s", bedroom_temp_actuator_2.id, bedroom_temp_actuator_2.group, "Yes");
         bedroom_temp_actuator_2.condition = 25;
         Devices[last_elem_index++] = bedroom_temp_actuator_2;


    Device livingroom_temp_actuator;
         strcpy(livingroom_temp_actuator.id, "temp_aktuator_livingroom");
         strcpy(livingroom_temp_actuator.group, ACTUATORS);
         strcpy(livingroom_temp_actuator.value, "OFF");
         livingroom_temp_actuator.gpio_pin = 15;
         strcpy(livingroom_temp_actuator.topic, "home/livingroom/temperature");
         snprintf(livingroom_temp_actuator.info, 250,"id: %s; Group: %s Controlable: %s", livingroom_temp_actuator.id, livingroom_temp_actuator.group, "Yes");
         livingroom_temp_actuator.condition = 25;
         Devices[last_elem_index++] = livingroom_temp_actuator;


    Device bathroom_temp_actuator;
         strcpy(bathroom_temp_actuator.id, "temp_aktuator_bathroom");
         strcpy(bathroom_temp_actuator.group, ACTUATORS);
         strcpy(bathroom_temp_actuator.value, "OFF");
         bathroom_temp_actuator.gpio_pin = 15;
         strcpy(bathroom_temp_actuator.topic, "home/bathroom/temperature");
         snprintf(bathroom_temp_actuator.info, 250,"id: %s; Group: %s Controlable: %s", bathroom_temp_actuator.id, bathroom_temp_actuator.group, "Yes");
         bathroom_temp_actuator.condition = 25;
         Devices[last_elem_index++] = bathroom_temp_actuator;



    Device door_opener;
         strcpy(door_opener.id, "door_opener");
         strcpy(door_opener.group, ACTUATORS);
         strcpy(door_opener.value, "OFF");
         door_opener.gpio_pin = 15;
         strcpy(door_opener.topic, "home/hallway/fingerprint");
         snprintf(door_opener.info, 250,"id: %s; Group: %s Controlable: %s", door_opener.id, door_opener.group, "Yes");
         door_opener.condition = 25;
         Devices[last_elem_index++] = door_opener;

    Device light_bulb;
         strcpy(light_bulb.id, "light_bulb");
         strcpy(light_bulb.group, ACTUATORS);
         strcpy(light_bulb.value, "OFF");
         light_bulb.gpio_pin = 15;
         strcpy(light_bulb.topic, "home/hallway/fingerprint");
         snprintf(light_bulb.info, 250,"id: %s; Group: %s Controlable: %s", light_bulb.id, light_bulb.group, "Yes");
         light_bulb.condition = 25;
         Devices[last_elem_index++] = light_bulb;


    
    
    mqtt_subscribe(&client,AKTUATORSKI_PI_TOPIC, 0);
    
    int i;
    for(i=0; i< last_elem_index; i++)
    {
        if(strcmp(Devices[i].group,ACTUATORS)==0)
            mqtt_subscribe(&client, Devices[i].topic, 0);
    }

    mqtt_subscribe(&client,DEVICES_INFO_TOPIC, 0);
    mqtt_subscribe(&client,DEVICES_FUNC_TOPIC, 0);

      
    pthread_t temp_thread;
   pthread_create(&temp_thread,NULL,&sensors_value_publish,&packet);

    pthread_join(temp_thread, NULL);

//    while(fgetc(stdin) != EOF); 
  
    exit(0);



  
    /* disconnect */
    printf("\n%s disconnecting from %s\n", argv[0], addr);
    sleep(1);

    /* exit */ 
    exit_example(EXIT_SUCCESS, sockfd, &client_daemon);
}


void* distribute_pub_message(void* arg)
{
    char** tokens;
    tokens = str_split(callback_packet.mes, DELIMITER);

    char * mes_type = *(tokens + 0);
    if( strcmp(mes_type,SENSOR_MES) == 0)
    {
        automation_control(tokens,callback_packet.topic,&Devices,&packet,last_elem_index); 
    }
    else if(strcmp(mes_type,SET_DEV_INFO) == 0)
    {
     
     set_dev_info(tokens,&Devices,&packet,last_elem_index);
     
    }
    else if(strcmp(mes_type,SET_DEV_VALUE) == 0)
    {
     
     set_dev_value(tokens,&Devices,&packet,last_elem_index);
     
    }
    else
    {
        //    
    }
    free(tokens);

    return NULL;
}

void publish_callback(void** unused, struct mqtt_response_publish *published) 
{
     /* note that published->topic_name is NOT null-terminated (here we'll change it to a c-string) */
   
    pthread_t temp_thread;

    string_cpy(& callback_packet.topic,published->topic_name, published->topic_name_size);  
    string_cpy(& callback_packet.mes,published->application_message, published->application_message_size);

    printf("Received publish('%s'): %s\n",callback_packet.topic , callback_packet.mes);
    
   pthread_create(&temp_thread,NULL,&distribute_pub_message,NULL);

}




void * sensors_value_publish(void * arg)
{
	int i;
    char message[120];

	while(1)
	{
		for ( i = 0; i < last_elem_index; i++)
		{
			if( strcmp(Devices[i].group, SENSORS) == 0)
			{
			
				update_sensors_value(i,&Devices,&packet); // updating state and publishing change to controler

                 sprintf(message,"%s.%s",SENSOR_MES,Devices[i].value);

				mqtt_publish((packet.client), Devices[i].topic, message, strlen( message) + 1, MQTT_PUBLISH_QOS_0);
        		printf(" published : topic:%s; %s\n",Devices[i].topic, message );

                client_error_check(&packet);
			}
		}
		sleep(SENSORS_READ_DELAY);	
	}

	return NULL;
}







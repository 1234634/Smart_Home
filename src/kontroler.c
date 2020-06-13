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
#define ALL_DEVICES "*"
#define SENZORSKI_PI_TOPIC "senzorski_pi"
#define AKTUATORSKI_PI_TOPIC "aktuatorski_pi"
#define DEVICE_NOT_FOUND "Device non existent"
#define UI_TOPIC "ui_topic"


static Device Devices[MAX_DEVICES];
static int last_elem_index = 0;


void* distribute_pub_message(void* );
void publish_callback(void** , struct mqtt_response_publish *);
//updates states of sensors and publishes change to controler
//void* update_sensors_value(int );

//publishes sensors states on every 3 seconds







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


 /*  Device temperature_aktuator;
    strcpy(temperature_aktuator.id,"temp_aktuator_lroom");
    strcpy(temperature_aktuator.group,ACTUATORS);
    strcpy(temperature_aktuator.value,"OFF");
    temperature_aktuator.gpio_pin = 15;
    strcpy(temperature_aktuator.topic,"home/living_room/temperature");
    snprintf(temperature_aktuator.info,250,"id: %s; Group: %s Controlable: %s",temperature_aktuator.id,temperature_aktuator.group,"Yes");
    temperature_aktuator.condition = 25;
    Devices[last_elem_index++] = temperature_aktuator;
*///Malo C, a zamalo i pajton
    Device bedroom_temp_sensor;
         strcpy(bedroom_temp_sensor.id, "temp_sensor_bedroom_1");
         strcpy(bedroom_temp_sensor.group, SENSORS);
         strcpy(bedroom_temp_sensor.value, "OFF");
         bedroom_temp_sensor.gpio_pin = 15;
         strcpy(bedroom_temp_sensor.topic, "home/bedroom1/temperature");
         snprintf(bedroom_temp_sensor.info, 250,"id: %s; Group: %s Controllable: %s", bedroom_temp_sensor.id, bedroom_temp_sensor.group, "No");
         bedroom_temp_sensor.condition = 25;
         Devices[last_elem_index++] = bedroom_temp_sensor;

    Device bedroom_temp_sensor_2;
         strcpy(bedroom_temp_sensor_2.id, "temp_sensor_bedroom_2");
         strcpy(bedroom_temp_sensor_2.group, SENSORS);
         strcpy(bedroom_temp_sensor_2.value, "OFF");
         bedroom_temp_sensor_2.gpio_pin = 15;
         strcpy(bedroom_temp_sensor_2.topic, "home/bedroom2/temperature");
         snprintf(bedroom_temp_sensor_2.info, 250,"id: %s; Group: %s Controllable: %s", bedroom_temp_sensor_2.id, bedroom_temp_sensor_2.group, "No");
         bedroom_temp_sensor_2.condition = 25;
         Devices[last_elem_index++] = bedroom_temp_sensor_2;

    Device bedroom_temp_actuator;
         strcpy(bedroom_temp_actuator.id, "temp_aktuator_bedroom_1");
         strcpy(bedroom_temp_actuator.group, ACTUATORS);
         strcpy(bedroom_temp_actuator.value, "OFF");
         bedroom_temp_actuator.gpio_pin = 15;
         strcpy(bedroom_temp_actuator.topic, "home/bedroom1/temperature");
         snprintf(bedroom_temp_actuator.info, 250,"id: %s; Group: %s Controlable: %s", bedroom_temp_actuator.id, bedroom_temp_actuator.group, "Yes");
         bedroom_temp_actuator.condition = 25;
         Devices[last_elem_index++] = bedroom_temp_actuator;

    Device bedroom_temp_actuator_2;
         strcpy(bedroom_temp_actuator_2.id, "temp_aktuator_bedroom_2");
         strcpy(bedroom_temp_actuator_2.group, ACTUATORS);
         strcpy(bedroom_temp_actuator_2.value, "OFF");
         bedroom_temp_actuator_2.gpio_pin = 15;
         strcpy(bedroom_temp_actuator_2.topic, "home/bedroom2/temperature");
         snprintf(bedroom_temp_actuator_2.info, 250,"id: %s; Group: %s Controlable: %s", bedroom_temp_actuator_2.id, bedroom_temp_actuator_2.group, "Yes");
         bedroom_temp_actuator_2.condition = 25;
         Devices[last_elem_index++] = bedroom_temp_actuator_2;

    Device livingroom_temp_sensor;
         strcpy(livingroom_temp_sensor.id, "temp_sensor_livingroom");
         strcpy(livingroom_temp_sensor.group, SENSORS);
         strcpy(livingroom_temp_sensor.value, "OFF");
         livingroom_temp_sensor.gpio_pin = 15;
         strcpy(livingroom_temp_sensor.topic, "home/livingroom/temperature");
         snprintf(livingroom_temp_sensor.info, 250,"id: %s; Group: %s Controllable: %s", livingroom_temp_sensor.id, livingroom_temp_sensor.group, "No");
         livingroom_temp_sensor.condition = 25;
         Devices[last_elem_index++] = livingroom_temp_sensor;

    Device livingroom_temp_actuator;
         strcpy(livingroom_temp_actuator.id, "temp_aktuator_livingroom");
         strcpy(livingroom_temp_actuator.group, ACTUATORS);
         strcpy(livingroom_temp_actuator.value, "OFF");
         livingroom_temp_actuator.gpio_pin = 15;
         strcpy(livingroom_temp_actuator.topic, "home/livingroom/temperature");
         snprintf(livingroom_temp_actuator.info, 250,"id: %s; Group: %s Controlable: %s", livingroom_temp_actuator.id, livingroom_temp_actuator.group, "Yes");
         livingroom_temp_actuator.condition = 25;
         Devices[last_elem_index++] = livingroom_temp_actuator;

    Device bathroom_temp_sensor;
         strcpy(bathroom_temp_sensor.id, "temp_sensor_bathroom");
         strcpy(bathroom_temp_sensor.group, SENSORS);
         strcpy(bathroom_temp_sensor.value, "OFF");
         livingroom_temp_sensor.gpio_pin = 15;
         strcpy(bathroom_temp_sensor.topic, "home/bathroom/temperature");
         snprintf(bathroom_temp_sensor.info, 250,"id: %s; Group: %s Controllable: %s", bathroom_temp_sensor.id, bathroom_temp_sensor.group, "No");
         livingroom_temp_sensor.condition = 25;
         Devices[last_elem_index++] = bathroom_temp_sensor;

    Device bathroom_temp_actuator;
         strcpy(bathroom_temp_actuator.id, "temp_aktuator_bathroom");
         strcpy(bathroom_temp_actuator.group, ACTUATORS);
         strcpy(bathroom_temp_actuator.value, "OFF");
         bathroom_temp_actuator.gpio_pin = 15;
         strcpy(bathroom_temp_actuator.topic, "home/bathroom/temperature");
         snprintf(bathroom_temp_actuator.info, 250,"id: %s; Group: %s Controlable: %s", bathroom_temp_actuator.id, bathroom_temp_actuator.group, "Yes");
         bathroom_temp_actuator.condition = 25;
         Devices[last_elem_index++] = bathroom_temp_actuator;

    Device oven_sensor;
         strcpy(oven_sensor.id, "temp_sensor_oven");
         strcpy(oven_sensor.group, SENSORS);
         strcpy(oven_sensor.value, "OFF");
         oven_sensor.gpio_pin = 15;
         strcpy(oven_sensor.topic, "home/kitchen/oven");
         snprintf(oven_sensor.info, 250,"id: %s; Group: %s Controllable: %s", oven_sensor.id, oven_sensor.group, "No");
         oven_sensor.condition = 25;
         Devices[last_elem_index++] = oven_sensor;

    Device fingerprint_sensor;
         strcpy(fingerprint_sensor.id, "fingerprint_sensor");
         strcpy(fingerprint_sensor.group, SENSORS);
         strcpy(fingerprint_sensor.value, "OFF");
         fingerprint_sensor.gpio_pin = 15;
         strcpy(fingerprint_sensor.topic, "home/hallway/fingerprint");
         snprintf(fingerprint_sensor.info, 250,"id: %s; Group: %s Controllable: %s", fingerprint_sensor.id, fingerprint_sensor.group, "No");
         fingerprint_sensor.condition = 25;
         Devices[last_elem_index++] = fingerprint_sensor;

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




    mqtt_subscribe(&client,CONTROLER_TOPIC, 0);


    while(fgetc(stdin) != EOF);

    exit(0);




    /* disconnect */
    printf("\n%s disconnecting from %s\n", argv[0], addr);
    sleep(1);

    /* exit */
    exit_example(EXIT_SUCCESS, sockfd, &client_daemon);
}

int findDevice(char* device_name)
{
    int i;
    for(i = 0; i < last_elem_index; i++)
    {
	
        if(strcmp(device_name, Devices[i].id) == 0)
            return i;
    }
    return -1;

}


void* distribute_pub_message(void* arg)
{

    char** tokens;
    tokens = str_split(callback_packet.mes, DELIMITER);

    printf("%s\n",callback_packet.mes);
    char * mes_type = tokens[0];
    char * target_device = tokens[1];
    //printf("%s\n", *target_device);

    printf("----\n");
    if( strcmp(mes_type, GET_DEV_INFO) == 0)
    {
        if(strcmp(target_device, ALL_DEVICES) != 0)
        {
            char message[200];
            strcpy(message, Devices[findDevice(target_device)].info);
            mqtt_publish((packet.client), UI_TOPIC, message, strlen( message) + 1, MQTT_PUBLISH_QOS_0);

        }
        else
        {

        }

        //iz svoje liste deviceova nadje info
        //mqtt_publish( UI_TOPIC, tja info sto je nasao)
        // ne zaboravi da proveris dal je id = * i onda da spakujes
        // sve deviceove u poruku i da mu posaljes

    }
    else if(strcmp(mes_type,GET_DEV_VALUE) == 0)
    {

        //isto kao get dev info
        if(strcmp(target_device, ALL_DEVICES) != 0)
        {
            char message[200];
            strcpy(message, Devices[findDevice(target_device)].value);
             mqtt_publish((packet.client), UI_TOPIC, message, strlen( message) + 1, MQTT_PUBLISH_QOS_0);
        }
        else
        {

        }

    }
    else if(strcmp(mes_type,SET_DEV_VALUE) == 0)
    {

        //ovde kontroler samo prosledjuje a kad mu jave propertyChanged
        //onda menja i u svojoj listi uredjaja
        // ovako ko sto pise , DEVICES_FUNC_TOPIC tema funkcionalnosti
        // na tu temu se pretplacuju aktuatori jer su im values controlabilne
         // mqtt_publish((packet.client), DEVICES_FUNC_TOPIC, message, strlen( message) + 1, MQTT_PUBLISH_QOS_0);
         // printf(" published : topic:%s; %s\n",DEVICES_FUNC_TOPIC, message );

     //mqtt_publish(UI_TOPIC...); odmah javi UI da je setovano

     if(strcmp(target_device, ALL_DEVICES) != 0)
        {
            //strcpy(message, Devices[findDevice(target_device)].info);
        }
        else
        {

        }



    }
    else if(strcmp(mes_type,SET_DEV_INFO) == 0)
    {

        // slicno prethodnoj samo je topic INFO
         // mqtt_publish((packet.client), DEVICES_INFO_TOPIC, message, strlen( message) + 1, MQTT_PUBLISH_QOS_0);
         // printf(" published : topic:%s; %s\n",DEVICES_FUNC_TOPIC, message );

     //mqtt_publish(UI_TOPIC...); odmah javi UI da je setovano

         if(strcmp(target_device, ALL_DEVICES) != 0)
        {
            //strcpy(message, Devices[findDevice(target_device)].info);
        }
        else
        {

        }

    }
    else if(strcmp(mes_type,PROPERTY_CHANGED) == 0)
    {

        // vidi koji se property promenio i promeni ga
        // u svojoj listi device-ova

    }
    else
    {

    //

    }
    free(tokens);

    return NULL;
}

void* publish_no_device(void* arg)
{
	
	char * message = DEVICE_NOT_FOUND;	
	printf("Publishujem:%s\n",message);	
	mqtt_publish((packet.client),UI_TOPIC, message, strlen( message) + 1, MQTT_PUBLISH_QOS_0);

	return NULL;
}


void publish_callback(void** unused, struct mqtt_response_publish *published)
{
     /* note that published->topic_name is NOT null-terminated (here we'll change it to a c-string) */

    pthread_t temp_thread;

    string_cpy(& callback_packet.topic,published->topic_name, published->topic_name_size);
    string_cpy(& callback_packet.mes,published->application_message, published->application_message_size);

    printf("Received publish('%s'): %s\n",callback_packet.topic , callback_packet.mes);
	

    char** tokens;
   tokens = str_split(callback_packet.mes, DELIMITER);


    //char * mes_type = *(tokens + 0);
   char * target_device = *(tokens + 1);

    int device_index = findDevice(target_device);
   

    /*
        ovde prvo proveriti da li device sa tim id postoji
        if(postoji)
            pthread_create
        else
            mqtt_publish(UI_TOPIC, "ne postoji  taj device");
     * */
   if(device_index != -1)
     {
	printf("Distribuira se \n");

        pthread_create(&temp_thread,NULL,&distribute_pub_message,NULL);
	
     }
     else
     {
	pthread_create(&temp_thread,NULL,&publish_no_device,NULL);
        
	
     }

}











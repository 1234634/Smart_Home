
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include<string.h>
#include <mqtt.h>
#include "templates/posix_sockets.h"
#include "mqtt_helper.h"
#include "device.c"
#define SENSORS "sensors"
#define ACTUATORS "actuators"
#define MAX_DEVICES 20
#define SENSORS_READ_DELAY 3
#define CONTROLER_TOPIC "controler"

static Device Devices[MAX_DEVICES];
static int last_elem_index = 0; 

//updates states of sensors and publishes change to controler
void* update_sensors_state(int , void*); 

//publishes sensors states on every 3 seconds
void * sensors_state_publish(void * );

//function does nothing (it is used to remove warnings of unused param)
void foo(const char*,const char*,const char*);

//sets value of addr,port,topic through command line arguments
void set_arguments(const char**,const char**,const char**,int,const char*[] );





//reads value from GPIO PIN
int read_pin(int  );

//daemon that publishes sensors value 
void * publish_daemon(void * );

struct pub_packet
{
	struct mqtt_client* client;
	int socket;
	pthread_t* client_daemon;

};



int main(int argc, const char *argv[]) 
{
    const char* addr;
    const char* port;
    const char* topic;

    /* get address (argv[1] if present) */
    if (argc > 1) {
        addr = argv[1];
    } else {
        addr = "test.mosquitto.org";
    }

    /* get port number (argv[2] if present) */
    if (argc > 2) {
        port = argv[2];
    } else {
        port = "1883";
    }

    /* get the topic name to publish */
    if (argc > 3) {
        topic = argv[3];
    } else {
        topic = "home/living_room/temperature";
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
    Device temperature_sensor;
    strcpy(temperature_sensor.id,"temperature_sensor_living_room");
    strcpy(temperature_sensor.group,SENSORS);
    strcpy(temperature_sensor.value,"25lr");
    strcpy(temperature_sensor.gpio_pin,"13");
    strcpy(temperature_sensor.topic,"home/living_room/temperature");
    
    Devices[last_elem_index++] = temperature_sensor;    


    mqtt_subscribe(&client, "Kontroler", 0);

    struct pub_packet packet;
    packet.client = &client;
    packet.socket = sockfd;
    packet.client_daemon = &client_daemon;
      
    pthread_t temp_thread;
    pthread_create(&temp_thread,NULL,&sensors_state_publish,&packet);

    sleep(3);

    printf("Ja sam izvan treda ");
    pthread_join(temp_thread, NULL);

    exit(0);



  
    /* disconnect */
    printf("\n%s disconnecting from %s\n", argv[0], addr);
    sleep(1);

    /* exit */ 
    exit_example(EXIT_SUCCESS, sockfd, &client_daemon);
}
int read_pin(int arg_pin )
{
        return 25;

}

void* update_sensors_state(int arg_dev_index, void* arg_packet)
{
	

}


void * sensors_state_publish(void * arg_packet)
{
	int i;
   	struct pub_packet *packet = (struct pub_packet*)arg_packet;        
	while(1)
	{

		for ( i = 0; i < last_elem_index; i++)
		{
			if( !strcmp(Devices[i].group, SENSORS))
			{
        			printf(" published : 	 \n" );
				update_sensors_state(i,arg_packet); // updating state and publishing change to controler
				mqtt_publish((packet->client), Devices[0].topic, Devices[0].value, strlen( Devices[0].value) + 1, MQTT_PUBLISH_QOS_0);

       				if ((*(packet->client)).error != MQTT_OK) {
            				fprintf(stderr, "error: %s\n", mqtt_error_str((*(packet->client)).error));
           				exit_example(EXIT_FAILURE, packet->socket, packet->client_daemon);
				}
			}
		}
		sleep(SENSORS_READ_DELAY);	
	}

	return NULL;
}




void * publish_daemon(void * arg_packet)
{

   struct pub_packet *packet = (struct pub_packet*)arg_packet;        
    const char *topic = "home/living_room/temperature";// topic
    while(1) {
        
        char application_message[] = "Iz Senzora";
        printf(" published : \"%s\"   %s \n", application_message,topic);
        sleep(1);
      mqtt_publish((packet->client), Devices[0].topic, Devices[0].value, strlen( Devices[0].value) + 1, MQTT_PUBLISH_QOS_0);

        if ((*(packet->client)).error != MQTT_OK) {
            fprintf(stderr, "error: %s\n", mqtt_error_str((*(packet->client)).error));
            exit_example(EXIT_FAILURE, packet->socket, packet->client_daemon);
        }
     
    }   
        
    return NULL;
}



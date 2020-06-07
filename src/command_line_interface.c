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
#define DEVICES_INFO_TOPIC "devices/info"//device tema bez oznakom funkcionalnosti
#define DEVICES_FUNC_TOPIC "devices/func" //device tema sa oznakom funkcionalnosti
#define GET_DEV_VALUE "Get_Dev_Value"
#define SET_DEV_VALUE "Set_Dev_Value"
#define GET_DEV_INFO "Get_Dev_Info"
#define SET_DEV_INFO "Set_Dev_Info"
#define PROPERTY_CHANGED "PropertyChanged"
#define SENSOR_MES "Sensor_Mes"
#define DELIMITER '.'
#define BLANKSPACE ' '
#define HELP "--help"
#define CONTROLER_TOPIC "controler"
#define SENZORSKI_PI_TOPIC "senzorski_pi"
#define AKTUATORSKI_PI_TOPIC "aktuatorski_pi"
#define UI_TOPIC "aktuatorski_pi"


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
/********************* END_OF_INITIALISATION**************************/




    mqtt_subscribe(&client,UI_TOPIC, 0);

    packet.client = &client;
    packet.socket = sockfd;
    packet.client_daemon = &client_daemon;

    while(1)
    {
        printf("Please enter a command: \n");
        char input[40];
        scanf("%s", input);

        char message[40];
        strcpy(message, input);

        char ** tokens;
        int tokens_length=0;
	
	
        tokens = str_split(input, DELIMITER);
	while(*(tokens+tokens_length)){ tokens_length++;  }
    /**printf("%d\n", tokens_length);
    int i;
    for(i = 0; i < tokens_length; i++)
        {
            printf("%s\n", tokens[i]);
        }*/
        char * command = tokens[0];

        if(strcmp(command, GET_DEV_INFO) == 0 || strcmp(command, GET_DEV_VALUE) == 0)
        {
            if(tokens_length != 2)
            {
                printf("Invalid number of arguments, please input --help for assistance.\n");
                continue;
            }
        }
        else if(strcmp(command, SET_DEV_INFO) == 0 || strcmp(command, SET_DEV_VALUE) == 0)
        {
            if(tokens_length != 3)
            {
                printf("Invalid number of arguments, please input --help for assistance.\n");
                continue;
            }
        }else if(strcmp(command, HELP) == 0)
        {
            help();
        }
        else
        {
            printf("Unrecognized command, please input --help for assistance.\n");
        }

        //char * message = trim(input, BLANKSPACE);
        //printf("%s\n", message);

        mqtt_publish((packet.client), CONTROLER_TOPIC, message, strlen( message) + 1, MQTT_PUBLISH_QOS_0);

        free(tokens);
    }

    //while(fgetc(stdin) != EOF);
   /*
    while(1)
    {

        strcpy(message,get_user_input());
		mqtt_publish((packet.client), CONTROLER_TOPIC, message, strlen( message) + 1, MQTT_PUBLISH_QOS_0);
    }*/

    exit(0);




    /* disconnect */
    printf("\n%s disconnecting from %s\n", argv[0], addr);
    sleep(1);

    /* exit */
    exit_example(EXIT_SUCCESS, sockfd, &client_daemon);
}



void publish_callback(void** unused, struct mqtt_response_publish *published)
{

    string_cpy(& callback_packet.topic,published->topic_name, published->topic_name_size);
    string_cpy(& callback_packet.mes,published->application_message, published->application_message_size);

    printf("Od kontrolera: %s\n" , callback_packet.mes);


}








/**
 Program koji pretstavlja senzorski pi. On publish-uje informacije sa senzora Brokeru. 
 */
#include<unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include<pthread.h>
#include <mqtt.h>
#include "templates/posix_sockets.h"
#include "mqtt_helper.h"


//function does nothing (it is used to remove warnings of unused param)
void foo(const char*,const char*,const char*);
//sets value of addr,port,topic through command line arguments
void set_arguments(const char**,const char**,const char**,int,const char*[] );


//reads value from GPIO PIN
int read_pin(int  );

//daemon that publishes sensors value 
void * publish_daemon(void * );




int main(int argc, const char *argv[]) 
{
    const char* addr = NULL;
    const char* port = NULL;
    const char* topic = NULL;

    set_arguments(&addr,&port,&topic,argc,argv);
  
    /* connect client to broker */
    struct mqtt_client_describer sensor_client;
    sensor_client = initalize_client( addr, port);

    
    struct mqtt_client client = sensor_client.client;
    pthread_t temp_thread;
    pthread_create(&temp_thread,NULL,&publish_daemon,&client);
    
    sleep(3);
    
    printf("Ja sam izvan treda ");
    pthread_join(temp_thread, NULL); 

    exit(0); 
   
}

void foo(const char*addr,const char*port,const char*topic)
{}
void set_arguments(const char** addr,const char** port ,const char** topic ,int argc, const char *argv[])
{
	/* get address (argv[1] if present) */
    if (argc > 1) {
        *addr = argv[1];
    } else {
        *addr = MQTT_ADR;
    }

    /* get port number (argv[2] if present) */
    if (argc > 2) {
        *port = argv[2];
    } else {
        *port = MQTT_PORT;
    }

    /* get the topic name to publish */
    if (argc > 3) {
        *topic = argv[3];
    } else {
        *topic = DEFAULT_TOPIC;
    }

    foo(*addr,*port,*topic);

}


int read_pin(int arg_pin )
{
	return 25;

}

void * publish_daemon(void * arg_client)
{

   struct mqtt_client *client = (struct mqtt_client*)arg_client;	
    const char *topic = "home/living_room/temperature";// topic
    while(1) {
	
        char application_message[256] = " Hello World";
        printf(" published : \"%s\" \n", application_message);
	sleep(1);
     // mqtt_publish(client, topic, application_message, strlen(application_message) + 1, MQTT_PUBLISH_QOS_0);

       /* if (arg_client.error != MQTT_OK) {
            fprintf(stderr, "error: %s\n", mqtt_error_str(client.error));
            exit_example(EXIT_FAILURE, sockfd, &client_daemon);
        }*/
     
    }   
	
    return NULL; 
}



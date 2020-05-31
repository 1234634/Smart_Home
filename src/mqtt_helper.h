#include<mqtt.h>



#define MQTT_PORT "1883"
#define MQTT_ADR "test.mosquitto.org"
#define DEFAULT_TOPIC "home/living_room/temperature"


struct mqtt_client_describer
{
	int socket;
	int error_mes;
	struct mqtt_client client;
};

void exit_example(int status, int sockfd, pthread_t *client_daemon)
{
    if (sockfd != -1) close(sockfd);
    if (client_daemon != NULL) pthread_cancel(*client_daemon);
    exit(status);
}


void publish_callback(void** unused, struct mqtt_response_publish *published) 
{
     /* note that published->topic_name is NOT null-terminated (here we'll change it to a c-string) */
   
    char* topic_name = (char*) malloc(published->topic_name_size + 1);
    memcpy(topic_name, published->topic_name, published->topic_name_size);
    topic_name[published->topic_name_size] = '\0';

    
    printf("Received publish('%s'): %s\n", topic_name, (const char*) published->application_message);

    
    free(topic_name);



}

void* client_refresher(void* client)
{
    while(1) 
    {
        mqtt_sync((struct mqtt_client*) client);
        usleep(100000U);
    }
    return NULL;
}



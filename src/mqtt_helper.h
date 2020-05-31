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
    /* not used in this example */
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

 struct mqtt_client_describer initalize_client(const char* addr, const char* port)
{
	struct mqtt_client_describer ret;
	ret.error_mes = 0;

	ret.socket = open_nb_socket(addr, port);
	if (ret.socket == -1) {
	perror("Failed to open socket: ");
	exit_example(EXIT_FAILURE, ret.socket	, NULL);
	}
	struct mqtt_client client;
    uint8_t sendbuf[2048]; /* sendbuf should be large enough to hold multiple whole mqtt messages */
    uint8_t recvbuf[1024]; /* recvbuf should be large enough any whole mqtt message expected to be received */
    mqtt_init(&client, ret.socket, sendbuf, sizeof(sendbuf), recvbuf, sizeof(recvbuf), publish_callback);
    /* Create an anonymous session */
    const char* client_id = NULL;
    /* Ensure we have a clean session */
    uint8_t connect_flags = MQTT_CONNECT_CLEAN_SESSION;
    /* Send connection request to the broker. */
    mqtt_connect(&client, client_id, NULL, NULL, 0, NULL, NULL, connect_flags, 400);

	  /* check that we don't have any errors */
    if (client.error != MQTT_OK) {
        fprintf(stderr, "error: %s\n", mqtt_error_str(client.error));
        exit_example(EXIT_FAILURE, ret.socket, NULL);
    }

	
    
	 /* start a thread to refresh the client (handle egress and ingree client traffic) */
    pthread_t client_daemon;
    if(pthread_create(&client_daemon, NULL, client_refresher, &client)) {
        fprintf(stderr, "Failed to start client daemon.\n");
        exit_example(EXIT_FAILURE, ret.socket, NULL);

    }	
	ret.client = client;
	

	return ret;


}

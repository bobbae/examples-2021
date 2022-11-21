
/**
 * @file
 * A simple program to that publishes the current time whenever ENTER is pressed.
 */
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include <mqtt.h>
#include "posix_sockets.h"

/**
 * @brief The function that would be called whenever a PUBLISH is received.
 *
 * @note This function is not used in this example.
 */
void publish_callback(void **unused, struct mqtt_response_publish *published);

/**
 * @brief The client's refresher. This function triggers back-end routines to
 *        handle ingress/egress traffic to the broker.
 *
 * @note All this function needs to do is call \ref __mqtt_recv and
 *       \ref __mqtt_send every so often. I've picked 100 ms meaning that
 *       client ingress/egress traffic will be handled every 100 ms.
 */
void *client_refresher(void *client);

/**
 * @brief Safelty closes the \p sockfd and cancels the \p client_daemon before \c exit.
 */
void exit_example(int status, int sockfd);

/**
 * A simple program to that publishes the current time whenever ENTER is pressed.
 */
int main(int argc, const char *argv[])
{
	const char *addr;
	const char *port;
	const char *topic;

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
		topic = "datetime";
	}

	printf("Usage: %s addr port topic\n", argv[0]);
	printf("default addr test.mosquitto.org\n", addr);
	printf("default port 1883\n", port);
	printf("default topic datetime\n", topic);
	
	printf("connect to addr %s at port %d on topic %s\n",
	       addr, port, topic);
	
	setvbuf(stdout, NULL, _IONBF, 0);
	setvbuf(stderr, NULL, _IONBF, 0);

#ifdef WIN32
	WSADATA wsaData;
	int res = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (res != NO_ERROR) {
		printf("WSAStartup failed\n");
		exit(1);
	}
#endif

	/* open the non-blocking TCP socket (connecting to the broker) */
	int sockfd = open_nb_socket(addr, port);

	if (sockfd == -1) {
		perror("Failed to open socket: ");
		exit_example(EXIT_FAILURE, sockfd);
	}

	/* setup a client */
	struct mqtt_client client;
	uint8_t sendbuf[2048];	/* sendbuf should be large enough to hold multiple whole mqtt messages */
	uint8_t recvbuf[1024];	/* recvbuf should be large enough any whole mqtt message expected to be received */
	mqtt_init(&client, sockfd, sendbuf, sizeof(sendbuf), recvbuf,
		  sizeof(recvbuf), publish_callback);
	/* Create an anonymous session */
	const char *client_id = NULL;
	/* Ensure we have a clean session */
	uint8_t connect_flags = MQTT_CONNECT_CLEAN_SESSION;
	/* Send connection request to the broker. */
	mqtt_connect(&client, client_id, NULL, NULL, 0, NULL, NULL,
		     connect_flags, 400);

	
	/* check that we don't have any errors */
	if (client.error != MQTT_OK) {
		fprintf(stderr, "error: %s\n", mqtt_error_str(client.error));
		exit_example(EXIT_FAILURE, sockfd);
	}


    /* subscribe */
	mqtt_subscribe(&client, topic, 0);

	/* start publishing the time */
	printf("%s is ready to begin publishing the time.\n", argv[0]);
	printf("Press ENTER to publish the current time.\n");
	printf("Press CTRL-D (or any other key) to exit.\n\n");
	while (fgetc(stdin) == '\n') {
		/* get the current time */
		time_t timer;
		time(&timer);
		struct tm *tm_info = localtime(&timer);
		char timebuf[26];
		strftime(timebuf, 26, "%Y-%m-%d %H:%M:%S", tm_info);

		/* print a message */
		char application_message[256];
		snprintf(application_message, sizeof(application_message),
			 "The time is %s", timebuf);
		printf("%s published : \"%s\"", argv[0], application_message);

		/* publish the time */
		mqtt_publish(&client, topic, application_message,
			     strlen(application_message) + 1,
			     MQTT_PUBLISH_QOS_0);

		/* check for errors */
		if (client.error != MQTT_OK) {
			fprintf(stderr, "error: %s\n",
				mqtt_error_str(client.error));
			exit_example(EXIT_FAILURE, sockfd);
		}
		mqtt_sync(&client);
	}

	mqtt_sync(&client);

	/* disconnect */
	printf("\n%s disconnecting from %s\n", argv[0], addr);
	sleep(1);

	/* exit */
	exit_example(EXIT_SUCCESS, sockfd);
}

void exit_example(int status, int sockfd)
{
	if (sockfd != -1)
		close(sockfd);
	exit(status);
}

void publish_callback(void **unused, struct mqtt_response_publish *published)
{	/* note that published->topic_name is NOT null-terminated (here we'll change it to a c-string) */
	char *topic_name = (char *)malloc(published->topic_name_size + 1);
	memcpy(topic_name, published->topic_name, published->topic_name_size);
	topic_name[published->topic_name_size] = '\0';

	printf("\nReceived publish('%s'): %s\n", topic_name,
	       (const char *)published->application_message);

	free(topic_name);
}

void *client_refresher(void *client)
{
	while (1) {
		mqtt_sync((struct mqtt_client *)client);
		usleep(100000U);
	}
	return NULL;
}

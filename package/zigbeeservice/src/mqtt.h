
#ifndef _MQTT_H_
#define _MQTT_H_

#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <mosquitto.h>

#define db_host "localhost"
#define db_username "mqtt_log"
#define db_password "password"
#define db_database "mqtt_log"
#define db_port 3306

#define db_query "INSERT INTO mqtt_log (topic, payload) VALUES (?,?)"

#define mqtt_host "139.129.217.3"
#define mqtt_port 8188

#define GREEN 				"\033[0;32;32m" 
#define NONEC 				"\033[m" 




void connect_callback(struct mosquitto *mosq, void *obj, int result);

void disconnect_callback(struct mosquitto *mosq, void *obj, int result);

void publish_callback(struct mosquitto *mosq, void *obj, int result);

void message_callback(struct mosquitto *mosq, void *obj, const struct mosquitto_message *message);

void * mqtt_thread(void *p);
	


#endif

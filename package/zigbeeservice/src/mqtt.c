#include "mqtt.h"
#include "ubus.h"



static int run = 1;

void handle_signal(int s)
{
	run = 0;
}

void connect_callback(struct mosquitto *mosq, void *obj, int result)
{
	debug(DEBUG_ERROR,"connect_callback enter");
	return;
}

void disconnect_callback(struct mosquitto *mosq, void *obj, int result)
{
	debug(DEBUG_ERROR,"disconnect_callback enter");
	return;
}

void publish_callback(struct mosquitto *mosq, void *obj, int result)
{
	debug(DEBUG_ERROR,"publish_callback enter");
	return;
}

void message_callback(struct mosquitto *mosq, void *obj, const struct mosquitto_message *message)
{
	debug(DEBUG_ERROR,"message_callback enter");
	return;
}

void * mqtt_thread(void *p)
{
	struct mosquitto *mosq;
	int rc = 0;

	//signal(SIGINT, handle_signal);
	//signal(SIGTERM, handle_signal);

	if(MOSQ_ERR_SUCCESS == mosquitto_lib_init())
		debug(DEBUG_ERROR,"mosquitto_lib_init success");
	else
		debug(DEBUG_ERROR,"mosquitto_lib_init failure");
	
	mosq = mosquitto_new("helloworld", true, NULL);
	if(mosq)
	{
		debug(DEBUG_ERROR,"mosquitto_new success");
		
		rc = mosquitto_tls_opts_set(mosq, 0, "tlsv1.1", NULL);
		if(rc == MOSQ_ERR_SUCCESS)
		{
			debug(DEBUG_ERROR,"mosquitto_tls_opts_set success");
		}
		else if(rc == MOSQ_ERR_INVAL)
		{
			debug(DEBUG_ERROR,"mosquitto_tls_opts_set MOSQ_ERR_INVAL");
		}
		else if(rc == MOSQ_ERR_NOMEM)
		{
			debug(DEBUG_ERROR,"mosquitto_tls_opts_set MOSQ_ERR_NOMEM");
		}
		else
		{
			debug(DEBUG_ERROR,"mosquitto_tls_opts_set rc[%d]",rc);
		}
		
		rc = mosquitto_tls_set(mosq, "/etc/config/iot.crt",NULL,  NULL, NULL, NULL);
		//rc = mosquitto_tls_set(mosq,  NULL, NULL,NULL, NULL, NULL);
		if(rc == MOSQ_ERR_SUCCESS)
		{
			debug(DEBUG_ERROR,"mosquitto_tls_set success");
		}
		else if(rc == MOSQ_ERR_INVAL)
		{
			debug(DEBUG_ERROR,"mosquitto_tls_set MOSQ_ERR_INVAL");
		}
		else if(rc == MOSQ_ERR_NOMEM)
		{
			debug(DEBUG_ERROR,"mosquitto_tls_set MOSQ_ERR_NOMEM");
		}
		else
		{
			debug(DEBUG_ERROR,"mosquitto_tls_set rc[%d]",rc);
		}
		
		mosquitto_connect_callback_set(mosq,connect_callback);
		mosquitto_disconnect_callback_set(mosq,disconnect_callback);
		mosquitto_publish_callback_set(mosq,publish_callback);
		mosquitto_message_callback_set(mosq, message_callback);

	    rc = mosquitto_connect(mosq, mqtt_host, mqtt_port, 60);
		if(rc == MOSQ_ERR_SUCCESS)
		{
			debug(DEBUG_ERROR,"mosquitto_connect success");
		}
		else if(rc == MOSQ_ERR_INVAL)
		{
			debug(DEBUG_ERROR,"mosquitto_connect MOSQ_ERR_INVAL");
		}
		else if(rc == MOSQ_ERR_ERRNO)
		{
			debug(DEBUG_ERROR,"mosquitto_connect MOSQ_ERR_ERRNO");
		}
		else
		{
			debug(DEBUG_ERROR,"mosquitto_connect rc[%d]",rc);
		}

		rc = mosquitto_subscribe(mosq, NULL, "#", 0);
		if(rc == MOSQ_ERR_SUCCESS)
		{
			debug(DEBUG_ERROR,"mosquitto_subscribe success");
		}
		else if(rc == MOSQ_ERR_INVAL)
		{
			debug(DEBUG_ERROR,"mosquitto_subscribe MOSQ_ERR_INVAL");
		}
		else if(rc == MOSQ_ERR_NOMEM)
		{
			debug(DEBUG_ERROR,"mosquitto_subscribe MOSQ_ERR_NOMEM");
		}
		else if(rc == MOSQ_ERR_NO_CONN)
		{
			debug(DEBUG_ERROR,"mosquitto_subscribe MOSQ_ERR_NO_CONN");
		}
		else
		{
			debug(DEBUG_ERROR,"mosquitto_subscribe rc[%d]",rc);
		}
		
		rc = mosquitto_publish(mosq, NULL, "pub/qos1/test", strlen("message"), "message", 1, false);
		if(rc == MOSQ_ERR_SUCCESS)
		{
			debug(DEBUG_ERROR,"mosquitto_publish success");
		}
		else if(rc == MOSQ_ERR_INVAL)
		{
			debug(DEBUG_ERROR,"mosquitto_publish MOSQ_ERR_INVAL");
		}
		else if(rc == MOSQ_ERR_NOMEM)
		{
			debug(DEBUG_ERROR,"mosquitto_publish MOSQ_ERR_NOMEM");
		}
		else if(rc == MOSQ_ERR_NO_CONN)
		{
			debug(DEBUG_ERROR,"mosquitto_publish MOSQ_ERR_NO_CONN");
		}
		else if(rc == MOSQ_ERR_PROTOCOL)
		{
			debug(DEBUG_ERROR,"mosquitto_publish MOSQ_ERR_PROTOCOL");
		}
		else if(rc == MOSQ_ERR_PAYLOAD_SIZE)
		{
			debug(DEBUG_ERROR,"mosquitto_publish MOSQ_ERR_PAYLOAD_SIZE");
		}
		else
		{
			debug(DEBUG_ERROR,"mosquitto_publish rc[%d]",rc);
		}
		
		while(run)
		{
			rc = mosquitto_loop(mosq,5000, 1);
			if(rc == MOSQ_ERR_SUCCESS)
			{
				debug(DEBUG_ERROR,"mosquitto_loop success");
			}
			else if(rc == MOSQ_ERR_INVAL)
			{
				debug(DEBUG_ERROR,"mosquitto_loop MOSQ_ERR_INVAL");
			}
			else if(rc == MOSQ_ERR_NOMEM)
			{
				debug(DEBUG_ERROR,"mosquitto_loop MOSQ_ERR_NOMEM");
			}
			else if(rc == MOSQ_ERR_NO_CONN)
			{
				debug(DEBUG_ERROR,"mosquitto_loop MOSQ_ERR_NO_CONN");
			}
			else if(rc == MOSQ_ERR_CONN_LOST)
			{
				debug(DEBUG_ERROR,"mosquitto_loop MOSQ_ERR_CONN_LOST");
			}
			else if(rc == MOSQ_ERR_PROTOCOL)
			{
				debug(DEBUG_ERROR,"mosquitto_loop MOSQ_ERR_PROTOCOL");
			}
			else if(rc == MOSQ_ERR_ERRNO)
			{
				debug(DEBUG_ERROR,"mosquitto_loop MOSQ_ERR_ERRNO");
			}
			else
			{
				debug(DEBUG_ERROR,"mosquitto_loop rc[%d]",rc);
			}
			if(run && rc)
			{
				sleep(20);
				rc = mosquitto_reconnect(mosq);
				if(rc == MOSQ_ERR_SUCCESS)
				{
					debug(DEBUG_ERROR,"mosquitto_reconnect success");
				}
				else if(rc == MOSQ_ERR_INVAL)
				{
					debug(DEBUG_ERROR,"mosquitto_reconnect MOSQ_ERR_INVAL");
				}
				else if(rc == MOSQ_ERR_NOMEM)
				{
					debug(DEBUG_ERROR,"mosquitto_reconnect MOSQ_ERR_NOMEM");
				}
				else if(rc == MOSQ_ERR_ERRNO)
				{
					debug(DEBUG_ERROR,"mosquitto_reconnect MOSQ_ERR_NO_CONN");
				}
				else
				{
					debug(DEBUG_ERROR,"mosquitto_reconnect rc[%d]",rc);
				}
			}
		}
		mosquitto_destroy(mosq);
	}
	else
	{
		debug(DEBUG_ERROR,"mosquitto_new failure");
	}

	mosquitto_lib_cleanup();

	return rc;
}


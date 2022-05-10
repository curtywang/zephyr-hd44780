#ifndef MQTT_SETUP_H
#define MQTT_SETUP_H

#ifdef __cplusplus
extern "C" {
#endif

#include <zephyr.h>
#include <net/socket.h>
#include <net/mqtt.h>
#include <random/rand32.h>
#include <string.h>
#include "config.h"

static uint8_t mqtt_buffer_rx[CONFIG_MQTT_BUFFER_SIZE];
static uint8_t mqtt_buffer_tx[CONFIG_MQTT_BUFFER_SIZE];
static struct mqtt_client the_mqtt_client;
static struct sockaddr_storage the_mqtt_broker;

static struct zsock_pollfd fds[1];
static int nfds;
static bool the_mqtt_client_connected = false;

static uint8_t mqtt_topic_default[] = "devices/" CONFIG_MQTT_CLIENT_ID "/messages/devicebound/#";
static struct mqtt_topic mqtt_pub_topic;
// static struct mqtt_subscription_list subs_list;


/**
 * @brief PRIVATE: Clear the Zephyr BSD sockets
 * 
 */
static void clear_fds(void)
{
	nfds = 0;
}


/**
 * @brief PRIVATE: Zephyr MQTT Event Handler (copied from Zephyr samples)
 * 
 * @param client 
 * @param event 
 */
static void mqtt_event_handler(struct mqtt_client *client,
                      const struct mqtt_evt *event)
{
	int err;
	switch (event->type) {
	case MQTT_EVT_CONNACK:
		if (event->result != 0) {
			printk("MQTT Client: MQTT connect failed %d\n", event->result);
			break;
		}
		the_mqtt_client_connected = true;
		printk("MQTT Client: MQTT client connected!\n");
		break;

	case MQTT_EVT_DISCONNECT:
		printk("MQTT Client: MQTT client disconnected %d\n", event->result);
		the_mqtt_client_connected = false;
		clear_fds();
		break;

	case MQTT_EVT_PUBACK:
		if (event->result != 0) {
			printk("MQTT Client: MQTT PUBACK error %d\n", event->result);
			break;
		}
		printk("MQTT Client: PUBACK packet id: %u\n", event->param.puback.message_id);
		break;

	case MQTT_EVT_PUBREC:
		if (event->result != 0) {
			printk("MQTT Client: MQTT PUBREC error %d\n", event->result);
			break;
		}
		printk("MQTT Client: PUBREC packet id: %u\n", event->param.pubrec.message_id);
		const struct mqtt_pubrel_param rel_param = {
			.message_id = event->param.pubrec.message_id
		};
		err = mqtt_publish_qos2_release(client, &rel_param);
		if (err != 0) {
			printk("MQTT Client: Failed to send MQTT PUBREL: %d\n", err);
		}
		break;

	case MQTT_EVT_PUBCOMP:
		if (event->result != 0) {
			printk("MQTT Client: MQTT PUBCOMP error %d\n", event->result);
			break;
		}
		printk("MQTT Client: PUBCOMP packet id: %u\n",
			event->param.pubcomp.message_id);

		break;

	case MQTT_EVT_PINGRESP:
		printk("MQTT Client: PINGRESP packet\n");
		break;

	default:
		break;
	}
}


/**
 * @brief PRIVATE: setup the Zephyr BSD Sockets for the broker
 * 
 */
static void broker_setup() {
    struct sockaddr_in *broker4 = (struct sockaddr_in *)&the_mqtt_broker;
    broker4->sin_family = AF_INET;
    broker4->sin_port = htons(CONFIG_MQTT_SERVER_PORT);  // translate to network byte order
    zsock_inet_pton(AF_INET, CONFIG_MQTT_SERVER_ADDR, &(broker4->sin_addr));  // translate string to sockets order
}


/**
 * @brief PRIVATE: setup the Zephyr MQTT client
 * 
 */
static void mqtt_client_setup() {
    mqtt_client_init(&the_mqtt_client);
    the_mqtt_client.broker = &the_mqtt_broker;

    the_mqtt_client.evt_cb = mqtt_event_handler;

    the_mqtt_client.client_id.utf8 = (uint8_t *)CONFIG_MQTT_CLIENT_ID;
    the_mqtt_client.client_id.size = sizeof(CONFIG_MQTT_CLIENT_ID) -1;
    the_mqtt_client.password = NULL;
    the_mqtt_client.user_name = NULL;
    the_mqtt_client.protocol_version = MQTT_VERSION_3_1_1;
    the_mqtt_client.transport.type = MQTT_TRANSPORT_NON_SECURE;

    the_mqtt_client.rx_buf = mqtt_buffer_rx;
    the_mqtt_client.rx_buf_size = sizeof(mqtt_buffer_rx);
    the_mqtt_client.tx_buf = mqtt_buffer_tx;
    the_mqtt_client.tx_buf_size = sizeof(mqtt_buffer_tx);
}


/**
 * @brief PRIVATE: Poll the MQTT socket for a packet
 * 
 * @return int 
 */
static int mqtt_wait_poll() {
    int rc = zsock_poll(fds, 1, CONFIG_MQTT_CLIENT_TIMEOUT_MS);
    if (rc < 0) {
		printk("MQTT Client poll timed out: %d\n", rc);
	}
    return rc;
}


/**
 * @brief PRIVATE: Connect to the MQTT Broker
 * 
 * @return int 
 */
static int mqtt_client_connect() {
    int rc = mqtt_connect(&the_mqtt_client);
    if (rc != 0) {
        printk("MQTT Client could not connect to the broker!\n");
        return rc;
    }

    // prepare the Zephyr BSD sockets
    fds[0].fd = the_mqtt_client.transport.tcp.sock;
    fds[0].events = ZSOCK_POLLIN;

    // check to see if connection was successful
    rc = mqtt_wait_poll();
    if (rc < 0) {
        mqtt_abort(&the_mqtt_client);
        return rc;
    }
    else {
        mqtt_input(&the_mqtt_client);
    }

    if (!the_mqtt_client_connected) {
        mqtt_abort(&the_mqtt_client);
        return -EINVAL;
    }
    else {
        return 0;
    }
}


/**
 * @brief PUBLIC: function to keep the MQTT client alive (CALL THIS PERIODICALLY!)
 * 
 * @return int 
 */
int mqtt_client_process_keepalive()
{
	int64_t remaining = CONFIG_MQTT_CLIENT_TIMEOUT_MS;
	int64_t start_time = k_uptime_get();
	int rc;

	while (remaining > 0 && connected) {
		if (wait(remaining)) {
			rc = mqtt_input(&the_mqtt_client);
			if (rc != 0) {
				PRINT_RESULT("mqtt_input", rc);
				return rc;
			}
		}

		rc = mqtt_live((&the_mqtt_client);
		if (rc != 0 && rc != -EAGAIN) {
			PRINT_RESULT("mqtt_live", rc);
			return rc;
		} else if (rc == 0) {
			rc = mqtt_input((&the_mqtt_client);
			if (rc != 0) {
				PRINT_RESULT("mqtt_input", rc);
				return rc;
			}
		}

		remaining = timeout + start_time - k_uptime_get();
	}

	return 0;
}


/**
 * @brief PUBLIC: Ask the MQTT client to publish a message
 * 
 * @param payload_data pointer to the data to send out
 * @param payload_size size of the payload
 * @param client pointer to the MQTT client
 * @param qos MQTT Quality of Service enum
 * @return int 
 */
int mqtt_client_publish_message(uint8_t* payload_data, size_t payload_size)
{
	struct mqtt_publish_param param;

	param.message.topic.qos = MQTT_QOS_0_AT_MOST_ONCE;
	param.message.topic.topic.utf8 = mqtt_topic_default;
	param.message.topic.topic.size = strlen((char *)param.message.topic.topic.utf8);
	param.message.payload.data = payload_data;
	param.message.payload.len = payload_size;
	param.message_id = sys_rand32_get();
	param.dup_flag = 0U;
	param.retain_flag = 0U;

	return mqtt_publish(&the_mqtt_client, &param);
}



/**
 * @brief PUBLIC: Setup the MQTT client and Broker
 * 
 */
void mqtt_client_setup_and_connect() {
    int err;
    broker_setup();
    mqtt_client_setup();
    err = mqtt_client_connect();
}






#ifdef __cplusplus
}
#endif

#endif // MQTT_SETUP_H
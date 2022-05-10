#ifndef CONFIG_H
#define CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif


#define CONFIG_WIFI_SSID        "IoT_Devices_Only"
#define CONFIG_WIFI_KEY         "CSULA_EE5450"

#define CONFIG_MQTT_SERVER_ADDR "192.168.88.254"
#define CONFIG_MQTT_SERVER_PORT 8883
#define CONFIG_MQTT_BUFFER_SIZE 1024
#define CONFIG_MQTT_CLIENT_ID "Zephyr_MQTT_Client_9009"
#define CONFIG_MQTT_CLIENT_TIMEOUT_MS 2000

#ifdef __cplusplus
}
#endif

#endif // CONFIG_H

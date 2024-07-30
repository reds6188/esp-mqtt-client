#ifndef MQTT_CLIENT_H_
#define MQTT_CLIENT_H_

#include <Arduino.h>
#include <mqtt_client.h>
#include <console.h>

#define MQTT_T                      "MQTT"
#define MQTT_BUFFER_SIZE			1280
#define NUM_MAX_MQTT_TOPIC          8  
#define TOPIC_LENGTH                100

void initMqttClient(const char * broker_url, const char * client_id, const char * cacert_str, const char * cert_str, const char * prv_str);
void startMqttClient(void);
void mqttAddTopic(const char * topic);
static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data);
void onMqttConnect(void (*callback)(void));
void onMqttData(void (*callback)(char * topic, char * data, int data_length));
bool publishMqtt(const char * topic, String payload);

#endif  /* MQTT_CLIENT_H_ */
#ifndef MQTT_CLIENT_H_
#define MQTT_CLIENT_H_

#include <Arduino.h>
#include <mqtt_client.h>
#include <console.h>
#include "SPIFFS.h"

#define MQTT_T      "MQTT"

typedef struct {
    char * ca_path;
    char * certificate_path;
    char * private_path;
}cert_path_t;

void initMqttClient(const char * client_id, cert_path_t cert);

#endif  /* MQTT_CLIENT_H_ */
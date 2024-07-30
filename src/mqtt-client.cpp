#include "mqtt-client.h"

esp_mqtt_client_handle_t t5_mqtt_client;
char topic_list[NUM_MAX_MQTT_TOPIC][TOPIC_LENGTH];
int8_t topic_index = -1;
bool mqtt_connected;
void (*cbOnMqttConnect)(void);
void (*cbOnMqttData)(char * topic, char * data, int data_length);

void initMqttClient(const char * broker_url, const char * client_id, const char * cacert_str, const char * cert_str, const char * prv_str) {
	console.info(MQTT_T, "Client ID = " + String(client_id));

	const esp_mqtt_client_config_t mqttConf = {
		.uri = broker_url,
		.client_id = client_id,
		//.task_stack = 12000,
		.buffer_size = MQTT_BUFFER_SIZE,
		.cert_pem = cacert_str,
		.client_cert_pem = cert_str,
		.client_key_pem = prv_str,
	};

	t5_mqtt_client = esp_mqtt_client_init(&mqttConf);
	if (t5_mqtt_client == NULL) {
		console.error(MQTT_T, "Error while creating MQTT client");
	}
	console.success(MQTT_T, "Client created");

	esp_mqtt_client_register_event(t5_mqtt_client, MQTT_EVENT_ANY, mqtt_event_handler, nullptr);
	startMqttClient();
}

void startMqttClient(void) {
	esp_err_t err = esp_mqtt_client_start(t5_mqtt_client);
	if(err)
		console.error(MQTT_T, "Error while starting MQTT client :" + String(err));
	else
		console.success(MQTT_T, "Client started");
}

void mqttAddTopic(const char * topic) {
	topic_index++;
	if(topic_index < NUM_MAX_MQTT_TOPIC) {
		console.success(MQTT_T, "Added topic \"" + String(topic) + "\"");
		strcpy(topic_list[topic_index], topic);
	}
	else
		console.error(MQTT_T, "Too much topic");
}

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data) {
	auto event = (esp_mqtt_event_handle_t) event_data;
	esp_mqtt_client_handle_t client = event->client;

	switch ((esp_mqtt_event_id_t) event_id) {
		case MQTT_EVENT_DISCONNECTED:
			mqtt_connected = false;
			console.warning(MQTT_T, "EVENT - Client disconnected!");
			break;
		case MQTT_EVENT_CONNECTED:
			mqtt_connected = true;
			console.success(MQTT_T, "EVENT - Client connected!");
			
			for(uint8_t i=0 ; i<topic_index+1 ; i++) {
				if(esp_mqtt_client_subscribe(client, (const char *)topic_list[i], 1));
					console.success(MQTT_T, "+++ Subscribe successfully done on topic \"" + String(topic_list[i]) + "\"");
			}
			
			if(cbOnMqttConnect != NULL)
				cbOnMqttConnect();
			break;
		case MQTT_EVENT_ERROR:
			console.error(MQTT_T, "EVENT - Error");
			if(event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT) {
 				console.error(MQTT_T, "Last error code reported from esp-tls: " + String(event->error_handle->esp_tls_last_esp_err));
				console.error(MQTT_T, "Last tls stack error number: " + String(event->error_handle->esp_tls_stack_err));
				console.error(MQTT_T, "Last captured errno: " + String(event->error_handle->esp_transport_sock_errno) + " (" + strerror(event->error_handle->esp_transport_sock_errno) + ")");
			} else if (event->error_handle->error_type == MQTT_ERROR_TYPE_CONNECTION_REFUSED) {
				console.error(MQTT_T, "Connection refused error: " + String(event->error_handle->connect_return_code));
			} else {
				console.error(MQTT_T, "Unknown error type: " + String(event->error_handle->error_type));
			}
			break;
		case MQTT_EVENT_DATA:
			console.info(MQTT_T, "Event data received");
			if(event->topic == NULL) {
				console.warning(MQTT_T, "Topic is NULL");
			}
			else {
				cbOnMqttData(event->topic, event->data, event->data_len);
			}
			break;
		default:
			break;
	}
}
void onMqttConnect(void (*callback)(void)) {
	cbOnMqttConnect = callback;
}

void onMqttData(void (*callback)(char * topic, char * data, int data_length)) {
	cbOnMqttData = callback;
}

bool publishMqtt(const char * topic, String payload) {
	//int result = esp_mqtt_client_publish(t5_mqtt_client, url, payload.c_str(), payload.length(), 1, 0);
	int result = esp_mqtt_client_enqueue(t5_mqtt_client, topic, payload.c_str(), payload.length(), 1, 0, true);
	console.info(MQTT_T, "Publishing...");
	if(result == -2)
		console.error(MQTT_T, "Outbox is full");
	if(result >= 0)
		console.success(MQTT_T, "Published on topic \"" + String(topic) + "\"");
	else
		console.error(MQTT_T, "Failed to publish on topic \"" + String(topic) + "\"");
	return (result >= 0);
}
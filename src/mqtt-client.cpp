#include "mqtt-client.h"

esp_mqtt_client_handle_t t5_mqtt_client;

char * getFileAsString(fs::FS &fs, String path) {
	if(fs.exists(path)) {
		File file = fs.open(path, FILE_READ);
		if(!file) {
			console.error(MQTT_T, "Failed to open " + path);
			return NULL;
		}
		char* buffer = (char*)calloc(file.size() + 1, sizeof(char));
		file.readBytes(buffer, file.size());
		file.close();
		return buffer;
	}

	console.error(MQTT_T, "File \"" + path + "\" does not exists!");
	return NULL;
}

void initMqttClient(cert_path_t cert) {
	console.info(MQTT_T, "Reading certificates...");

	fs::SPIFFSFS CERT;

	// Mount CERT partition -----------------------------------------------------------------------
	if(!CERT.begin(false, "/spiffs", 4, "spiffs"))
        console.error(MQTT_T, "An Error has occurred while mounting CERT partition");
	else
		console.success(MQTT_T, "CERT partition has mounted");

	// Read "/device.ca.pem" file ---------------------------------------------
	char * cacert_str = getFileAsString(CERT, cert.ca_path);
	if(cacert_str != NULL)
		console.success(MQTT_T, "\"" + String(cert.ca_path) + "\" was loaded successfully");
	else
		console.error(MQTT_T, "\"" + String(cert.ca_path) + "\" not found!");

	// Read "/device.certificate.pem" file ------------------------------------
	char * cert_str = getFileAsString(CERT, cert.certificate_path);
	if(cert_str != NULL)
		console.success(MQTT_T, "\"" + String(cert.certificate_path) + "\" was loaded successfully");
	else
		console.error(MQTT_T, "\"" + String(cert.certificate_path) + "\" not found!");

	// Read "/device.private.pem" file ------------------------------------
	char * private_str = getFileAsString(CERT, cert.private_path);
	if(private_str != NULL)
		console.success(MQTT_T, "\"" + String(cert.private_path) + "\" was loaded successfully");
	else
		console.error(MQTT_T, "\"" + String(cert.private_path) + "\" not found!");

	CERT.end();
}
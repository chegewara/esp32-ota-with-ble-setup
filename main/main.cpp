/**
 * Main file for running the BLE samples.
 */
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_log.h>


extern void OTAServer(void);
extern "C" {
	void app_main(void);
}

void app_main(void) {
	OTAServer();
} // app_main


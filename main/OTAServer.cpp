#include "BLEDevice.h"
#include "BLEServer.h"
#include "BLEUtils.h"
#include "BLE2902.h"
#include <esp_log.h>
#include <string>
#include <Task.h>
#include "sdkconfig.h"
#include <lwip/dns.h>

extern void ota_app_main(void*, std::string, std::string);

static char LOG_TAG[] = "SampleServer";

typedef struct {
	std::string ip;
	std::string domain;
	std::string port;
	std::string path;
	std::string file_name;
} connection_info_t ;
connection_info_t connection;

typedef struct {
	std::string ssid;
	std::string password;
} wifi_info_t;
wifi_info_t wifi;

class CharacteristicCallback1 : public BLECharacteristicCallbacks { // domain name
	void onWrite(BLECharacteristic* pChar){
		connection.domain = pChar->getValue();
	}
};

class CharacteristicCallback2 : public BLECharacteristicCallbacks { // ip
	void onWrite(BLECharacteristic* pChar){
		connection.ip = pChar->getValue();
	}
};

class CharacteristicCallback3 : public BLECharacteristicCallbacks { // port
	void onWrite(BLECharacteristic* pChar){
		connection.port = pChar->getValue();
	}
};

class CharacteristicCallback4 : public BLECharacteristicCallbacks { // path
	void onWrite(BLECharacteristic* pChar){
		connection.path = pChar->getValue();
	}
};

class CharacteristicCallback5 : public BLECharacteristicCallbacks { // file name
	void onWrite(BLECharacteristic* pChar){
		connection.file_name = pChar->getValue();
		ota_app_main(&connection, wifi.ssid, wifi.password);
	}
};

class CharacteristicCallback6 : public BLECharacteristicCallbacks { // wifi ssid
	void onWrite(BLECharacteristic* pChar){
		wifi.ssid = pChar->getValue();
	}
};

class CharacteristicCallback7 : public BLECharacteristicCallbacks { // wifi password
	void onWrite(BLECharacteristic* pChar){
		wifi.password = pChar->getValue();
	}
};

class MySecurity : public BLESecurityCallbacks {
	uint32_t onPassKeyRequest(){
        ESP_LOGI(LOG_TAG, "PassKeyRequest");
		return 123456;
	}
	void onPassKeyNotify(uint32_t pass_key){
        ESP_LOGI(LOG_TAG, "On passkey Notify number:%d", pass_key);
	}
	bool onConfirmPIN(uint32_t){
		return true;
	}
	bool onSecurityRequest(){
	    ESP_LOGI(LOG_TAG, "On Security Request");
		return true;
	}

	void onAuthenticationComplete(esp_ble_auth_cmpl_t cmpl){
		if(cmpl.success){
			uint16_t length;
			esp_ble_gap_get_whitelist_size(&length);
			ESP_LOGD(LOG_TAG, "size: %d", length);
		}
	}
};

class MainBLEServer: public Task {
	void run(void *data) {

		BLEDevice::init("ESP32");
		BLEServer* pServer = BLEDevice::createServer();
		BLEDevice::setMTU(100);
		BLEDevice::setEncryptionLevel(ESP_BLE_SEC_ENCRYPT);
		BLEDevice::setSecurityCallbacks(new MySecurity());

		BLEService* pService = pServer->createService("00005b60-2745-479b-b006-56f3b4043c2c");
		BLEService* pService2 = pServer->createService("000018b0-f642-42dd-b048-aeb8e76e93de");

		BLECharacteristic* pCharacteristic = pService->createCharacteristic(			// domain name
			BLEUUID("00005b61-2745-479b-b006-56f3b4043c2c"),
			BLECharacteristic::PROPERTY_READ  |
			BLECharacteristic::PROPERTY_WRITE
		);

		pCharacteristic->setAccessPermissions(ESP_GATT_PERM_READ_ENCRYPTED | ESP_GATT_PERM_WRITE_ENCRYPTED);
		pCharacteristic->setCallbacks(new CharacteristicCallback1());

		BLECharacteristic* pCharacteristic1 = pService->createCharacteristic(			// IP
			BLEUUID("00005b62-2745-479b-b006-56f3b4043c2c"),
			BLECharacteristic::PROPERTY_READ  |
			BLECharacteristic::PROPERTY_WRITE
		);

		pCharacteristic1->setAccessPermissions(ESP_GATT_PERM_READ_ENCRYPTED | ESP_GATT_PERM_WRITE_ENCRYPTED);
		pCharacteristic1->setCallbacks(new CharacteristicCallback2());

		BLECharacteristic* pCharacteristic2 = pService->createCharacteristic(			// port
			BLEUUID("00005b63-2745-479b-b006-56f3b4043c2c"),
			BLECharacteristic::PROPERTY_READ  |
			BLECharacteristic::PROPERTY_WRITE
		);

		pCharacteristic2->setAccessPermissions(ESP_GATT_PERM_READ_ENCRYPTED | ESP_GATT_PERM_WRITE_ENCRYPTED);
		pCharacteristic2->setCallbacks(new CharacteristicCallback3());

		BLECharacteristic* pCharacteristic3 = pService->createCharacteristic(			// path
			BLEUUID("00005b64-2745-479b-b006-56f3b4043c2c"),
			BLECharacteristic::PROPERTY_READ  |
			BLECharacteristic::PROPERTY_WRITE
		);

		pCharacteristic3->setAccessPermissions(ESP_GATT_PERM_READ_ENCRYPTED | ESP_GATT_PERM_WRITE_ENCRYPTED);
		pCharacteristic3->setCallbacks(new CharacteristicCallback4());

		BLECharacteristic* pCharacteristic4 = pService->createCharacteristic(			// file name
			BLEUUID("00005b65-2745-479b-b006-56f3b4043c2c"),
			BLECharacteristic::PROPERTY_READ  |
			BLECharacteristic::PROPERTY_WRITE
		);

		pCharacteristic4->setAccessPermissions(ESP_GATT_PERM_READ_ENCRYPTED | ESP_GATT_PERM_WRITE_ENCRYPTED);
		pCharacteristic4->setCallbacks(new CharacteristicCallback5());

		BLECharacteristic* pCharacteristic5 = pService2->createCharacteristic(			// wifi ssid
			BLEUUID("000018b1-f642-42dd-b048-aeb8e76e93de"),
			BLECharacteristic::PROPERTY_READ  |
			BLECharacteristic::PROPERTY_WRITE
		);

		pCharacteristic5->setAccessPermissions(ESP_GATT_PERM_READ_ENCRYPTED | ESP_GATT_PERM_WRITE_ENCRYPTED);
		pCharacteristic5->setCallbacks(new CharacteristicCallback6());

		BLECharacteristic* pCharacteristic6 = pService2->createCharacteristic(			// wifi password
			BLEUUID("000018b2-f642-42dd-b048-aeb8e76e93de"),
			BLECharacteristic::PROPERTY_READ  |
			BLECharacteristic::PROPERTY_WRITE
		);

		pCharacteristic6->setAccessPermissions(ESP_GATT_PERM_READ_ENCRYPTED | ESP_GATT_PERM_WRITE_ENCRYPTED);
		pCharacteristic6->setCallbacks(new CharacteristicCallback7());

		pService->start();
		pService2->start();

		BLEAdvertising* pAdvertising = pServer->getAdvertising();
		pAdvertising->start();
/*
		BLESecurity *pSecurity = new BLESecurity();
		pSecurity->setAuthenticationMode(ESP_LE_AUTH_REQ_SC_BOND);
		pSecurity->setCapability(ESP_IO_CAP_NONE);
		pSecurity->setInitEncryptionKey(ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK);
*/
		ESP_LOGD(LOG_TAG, "Advertising started!");
		delay(portMAX_DELAY);
	}
};


void OTAServer(void)
{
	//esp_log_level_set("*", ESP_LOG_DEBUG);
	MainBLEServer* pMainBleServer = new MainBLEServer();
	pMainBleServer->setStackSize(20000);
	pMainBleServer->start();

} // app_main

#include "BLEDevice.h"
#include "BLEServer.h"
#include "BLEUtils.h"
#include "BLE2902.h"
#include <esp_log.h>
#include <string>
#include <Task.h>
#include "sdkconfig.h"
#include <lwip/dns.h>
#include <nvs_flash.h>
#include "CPPNVS.h"

#define WIFIINFO "wifi_info"
#define OTAINFO "ota_info"

extern void ota_app_main(void*, std::string, std::string);
NVS* otaNVS;

static char LOG_TAG[] = "SampleServer";

typedef struct {
	char ip[16];
	char domain[64];
	char port[16];
	char path[64];
	char file_name[128];
} ota_info_t ;
ota_info_t ota;

typedef struct {
	char ssid[32];
	char password[64];
} wifi_info_t;
wifi_info_t wifi;

class CharacteristicCallback1 : public BLECharacteristicCallbacks { // domain name
	void onWrite(BLECharacteristic* pChar){
		sprintf(ota.domain, "%s", pChar->getValue().c_str());
	}
};

class CharacteristicCallback2 : public BLECharacteristicCallbacks { // ip
	void onWrite(BLECharacteristic* pChar){
		sprintf(ota.ip, "%s", pChar->getValue().c_str());
	}
};

class CharacteristicCallback3 : public BLECharacteristicCallbacks { // port
	void onWrite(BLECharacteristic* pChar){
		sprintf(ota.port, "%s", pChar->getValue().c_str());
	}
};

class CharacteristicCallback4 : public BLECharacteristicCallbacks { // path
	void onWrite(BLECharacteristic* pChar){
		sprintf(ota.path, "%s", pChar->getValue().c_str());
	}
};

class CharacteristicCallback5 : public BLECharacteristicCallbacks { // file name
	void onWrite(BLECharacteristic* pChar){
		sprintf(ota.file_name, "%s", pChar->getValue().c_str());
		otaNVS->set(OTAINFO, (uint8_t*)&ota, sizeof(ota_info_t));
		otaNVS->commit();
		ota_app_main(&ota, wifi.ssid, wifi.password);
	}
};

class CharacteristicCallback6 : public BLECharacteristicCallbacks { // wifi ssid
	void onWrite(BLECharacteristic* pChar){
		sprintf(wifi.ssid, "%s", pChar->getValue().c_str());
	}
};

class CharacteristicCallback7 : public BLECharacteristicCallbacks { // wifi password
	void onWrite(BLECharacteristic* pChar){
		sprintf(wifi.password, "%s", pChar->getValue().c_str());
		otaNVS->set(WIFIINFO, (uint8_t*)&wifi, sizeof(wifi_info_t));
		otaNVS->commit();
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
//		BLEDevice::setMTU(100);
//		BLEDevice::setEncryptionLevel(ESP_BLE_SEC_ENCRYPT);
//		BLEDevice::setSecurityCallbacks(new MySecurity());

		BLEService* pService = pServer->createService("00005b60-2745-479b-b006-56f3b4043c2c");
		BLEService* pService2 = pServer->createService("000018b0-f642-42dd-b048-aeb8e76e93de");
		size_t size = sizeof(ota_info_t);
		otaNVS->get(OTAINFO, (uint8_t*)&ota, size);

		BLECharacteristic* pCharacteristic = pService->createCharacteristic(			// domain name
			BLEUUID("00005b61-2745-479b-b006-56f3b4043c2c"),
			BLECharacteristic::PROPERTY_READ  |
			BLECharacteristic::PROPERTY_WRITE
		);

//		pCharacteristic->setAccessPermissions(ESP_GATT_PERM_READ_ENCRYPTED | ESP_GATT_PERM_WRITE_ENCRYPTED);
		pCharacteristic->setCallbacks(new CharacteristicCallback1());

		BLECharacteristic* pCharacteristic1 = pService->createCharacteristic(			// IP
			BLEUUID("00005b62-2745-479b-b006-56f3b4043c2c"),
			BLECharacteristic::PROPERTY_READ  |
			BLECharacteristic::PROPERTY_WRITE
		);

//		pCharacteristic1->setAccessPermissions(ESP_GATT_PERM_READ_ENCRYPTED | ESP_GATT_PERM_WRITE_ENCRYPTED);
		pCharacteristic1->setCallbacks(new CharacteristicCallback2());

		BLECharacteristic* pCharacteristic2 = pService->createCharacteristic(			// port
			BLEUUID("00005b63-2745-479b-b006-56f3b4043c2c"),
			BLECharacteristic::PROPERTY_READ  |
			BLECharacteristic::PROPERTY_WRITE
		);

//		pCharacteristic2->setAccessPermissions(ESP_GATT_PERM_READ_ENCRYPTED | ESP_GATT_PERM_WRITE_ENCRYPTED);
		pCharacteristic2->setCallbacks(new CharacteristicCallback3());

		BLECharacteristic* pCharacteristic3 = pService->createCharacteristic(			// path
			BLEUUID("00005b64-2745-479b-b006-56f3b4043c2c"),
			BLECharacteristic::PROPERTY_READ  |
			BLECharacteristic::PROPERTY_WRITE
		);

//		pCharacteristic3->setAccessPermissions(ESP_GATT_PERM_READ_ENCRYPTED | ESP_GATT_PERM_WRITE_ENCRYPTED);
		pCharacteristic3->setCallbacks(new CharacteristicCallback4());

		BLECharacteristic* pCharacteristic4 = pService->createCharacteristic(			// file name
			BLEUUID("00005b65-2745-479b-b006-56f3b4043c2c"),
			BLECharacteristic::PROPERTY_READ  |
			BLECharacteristic::PROPERTY_WRITE
		);

//		pCharacteristic4->setAccessPermissions(ESP_GATT_PERM_READ_ENCRYPTED | ESP_GATT_PERM_WRITE_ENCRYPTED);
		pCharacteristic4->setCallbacks(new CharacteristicCallback5());

		size = sizeof(wifi_info_t);
		otaNVS->get(WIFIINFO, (uint8_t*)&wifi, size);
		BLECharacteristic* pCharacteristic5 = pService2->createCharacteristic(			// wifi ssid
			BLEUUID("000018b1-f642-42dd-b048-aeb8e76e93de"),
			BLECharacteristic::PROPERTY_READ  |
			BLECharacteristic::PROPERTY_WRITE
		);

//		pCharacteristic5->setAccessPermissions(ESP_GATT_PERM_READ_ENCRYPTED | ESP_GATT_PERM_WRITE_ENCRYPTED);
		pCharacteristic5->setCallbacks(new CharacteristicCallback6());

		BLECharacteristic* pCharacteristic6 = pService2->createCharacteristic(			// wifi password
			BLEUUID("000018b2-f642-42dd-b048-aeb8e76e93de"),
			BLECharacteristic::PROPERTY_READ  |
			BLECharacteristic::PROPERTY_WRITE
		);

//		pCharacteristic6->setAccessPermissions(ESP_GATT_PERM_READ_ENCRYPTED | ESP_GATT_PERM_WRITE_ENCRYPTED);
		pCharacteristic6->setCallbacks(new CharacteristicCallback7());

		pService->start();
		pService2->start();
		if (strlen(ota.port) != 0) {
			pCharacteristic->setValue(ota.domain);
			pCharacteristic1->setValue(ota.ip);
			pCharacteristic2->setValue(ota.port);
			pCharacteristic3->setValue(ota.path);
			pCharacteristic4->setValue(ota.file_name);
		}
		if (strlen(wifi.ssid) != 0) {
			pCharacteristic5->setValue(wifi.ssid);
			pCharacteristic6->setValue(wifi.password);
		}

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

	otaNVS = new NVS("ota_nvs");
	memset(&ota, 0, sizeof(ota));
	memset(&wifi, 0, sizeof(wifi));

	MainBLEServer* pMainBleServer = new MainBLEServer();
	pMainBleServer->setStackSize(20000);
	pMainBleServer->start();

} // app_main

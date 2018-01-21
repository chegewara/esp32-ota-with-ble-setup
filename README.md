ESP32 OTA update app with ble setup

This app is designed to be installed on smaller factory partition (about 1.2MB). Functionalities provided to end user are:
- setup wifi credentials
- setup informations required to connect to server and download bin file.

All informations can be setup with bluetooth app like nRF connect or with android that is designed to communicate with ota app.
Informations that can be setup to connect to server:
- domain name (http only at the moment),
- IP address (provided to connect with LAN server),
- port,
- path,
- binary file name.

There is still few more things to do.
- Add exceptions when domain name cant be found by dns,
- Add more exceptions when wifi credentials are wrong (ie cant connect to AP),
- Add https server as option
- Add gpio pin that will reset esp32 to boot from factory partition
- Add nvs save and retrive wifi credentials and ota info
- Add default ota server with list of examples to download???
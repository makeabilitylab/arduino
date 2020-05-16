#ifndef _COMMON_H_
#define _COMMON_H_

enum RecoderMode {
  RECORDER_SERIAL,
  RECORDER_BLUETOOTH,
  RECORDER_WIFI
};
// By default we use serial
const RecoderMode CURRENT_MODE = RECORDER_BLUETOOTH;

// -------------------------------------------
// Serial info
const int SERIAL_BAUD_RATE = 115200;

// -------------------------------------------
// Bluetooth settings
const char* ARDUINO_BT_NAME = "ESP32 Gesture Recorder";
// Bluetooth uses the serial baud rate and port index above

// -------------------------------------------
// WIFI
// If using wifi, make sure to put your credentials there if you're using WIFI mode
const char* WIFI_SSID = "";
const char* WIFI_PASS = "";
const char* WIFI_HOST_IP_ADDRESS = ""; // The IP address of the host you want to connect to
const int WIFI_HOST_PORT = 10002; // The IP address of the host you want to connect to

#endif

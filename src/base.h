#ifndef BASE_H_INCLUDED
#define BASE_H_INCLUDED

#include <Arduino.h>
#include <PubSubClient.h>       // MQTT

enum log_t   {LOG_LEVEL_NONE, LOG_LEVEL_ERROR, LOG_LEVEL_INFO, LOG_LEVEL_DEBUG, LOG_LEVEL_DEBUG_MORE, LOG_LEVEL_ALL};

void do_cmnd(char *cmnd);
void mqtt_publish(const char* topic, const char* data, boolean retained);
void addLog(byte loglevel, const char *line);
void setupDevice();
void checkDevice();

const char commands[8][14] PROGMEM = {
  {"reset 1"},        // Hold button for more than 4 seconds
  {"light 2"},        // Press button once
  {"light 2"},        // Press button twice
  {"wificonfig 1"},   // Press button three times
  {"wificonfig 2"},   // Press button four times
  {"wificonfig 3"},   // Press button five times
  {"restart 1"},      // Press button six times
  {"upgrade 1"}};     // Press button seven times

#endif /* BASE_H_INCLUDED */

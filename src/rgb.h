#ifndef LIGHT_SWITCH_H_INCLUDED
#define LIGHT_SWITCH_H_INCLUDED

#include "base.h"
#include "light_switch.h"

#endif /* LIGHT_SWITCH_H_INCLUDED */

void setColor(uint8_t p_red, uint8_t p_green, uint8_t p_blue);
void publishRGBState();
void publishRGBBrightness();
void publishRGBColor();
void publishDeviceState();
boolean deviceCallback(char* p_topic, byte* p_payload, unsigned int p_length);
void mqttConnectedDeviceCallback(PubSubClient mqttClient);

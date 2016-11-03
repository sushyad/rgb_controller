#include "debug.h"
#include "rgb.h"

// MQTT: topics
// state
const PROGMEM char* MQTT_LIGHT_STATE_TOPIC = "rgblight/rgb1/status";
const PROGMEM char* MQTT_LIGHT_COMMAND_TOPIC = "office/rgb1/command";

// brightness
const PROGMEM char* MQTT_LIGHT_BRIGHTNESS_STATE_TOPIC = "rgblight/rgb1/brightness/status";
const PROGMEM char* MQTT_LIGHT_BRIGHTNESS_COMMAND_TOPIC = "rgblight/rgb1/brightness/command";

// colors (rgb)
const PROGMEM char* MQTT_LIGHT_RGB_STATE_TOPIC = "rgblight/rgb1/rgbcolor/status";
const PROGMEM char* MQTT_LIGHT_RGB_COMMAND_TOPIC = "rgblight/rgb1/rgbcolor/command";

// payloads by default (on/off)
const PROGMEM char* LIGHT_ON = "ON";
const PROGMEM char* LIGHT_OFF = "OFF";

const PROGMEM uint16_t MAX_PWM_VALUE = 1023;

// variables used to store the state, the brightness and the color of the light
boolean m_rgb_state = false;
uint16_t m_rgb_brightness = MAX_PWM_VALUE; //0 <= m_rgb_brightness <= 1023
uint16_t m_rgb_red = MAX_PWM_VALUE;
uint16_t m_rgb_green = MAX_PWM_VALUE;
uint16_t m_rgb_blue = MAX_PWM_VALUE;

// pins used for the rgb led (PWM)
const PROGMEM uint8_t RGB_LIGHT_RED_PIN = 13;
const PROGMEM uint8_t RGB_LIGHT_GREEN_PIN = 12;
const PROGMEM uint8_t RGB_LIGHT_BLUE_PIN = 14;

// buffer used to send/receive data with MQTT
const uint8_t MSG_BUFFER_SIZE = 20;
char m_msg_buffer[MSG_BUFFER_SIZE]; 

// function called to adapt the brightness and the color of the led
void setColor(uint8_t p_red, uint8_t p_green, uint8_t p_blue) {
  uint8_t brightness = map(m_rgb_brightness, 0, 1023, 0, MAX_PWM_VALUE);

  analogWrite(RGB_LIGHT_RED_PIN, map(p_red, 0, 255, 0, brightness));
  analogWrite(RGB_LIGHT_GREEN_PIN, map(p_green, 0, 255, 0, brightness));
  analogWrite(RGB_LIGHT_BLUE_PIN, map(p_blue, 0, 255, 0, brightness));
}

// function called to publish the state of the led (on/off)
void publishRGBState() {
  if (m_rgb_state) {
    mqtt_publish(MQTT_LIGHT_STATE_TOPIC, LIGHT_ON, true);
  } else {
    mqtt_publish(MQTT_LIGHT_STATE_TOPIC, LIGHT_OFF, true);
  }
}

// function called to publish the brightness of the led (0-100)
void publishRGBBrightness() {
  snprintf(m_msg_buffer, MSG_BUFFER_SIZE, "%d", m_rgb_brightness);
	  mqtt_publish(MQTT_LIGHT_BRIGHTNESS_STATE_TOPIC, m_msg_buffer, true);
}

// function called to publish the colors of the led (xx(x),xx(x),xx(x))
void publishRGBColor() {
  snprintf(m_msg_buffer, MSG_BUFFER_SIZE, "%d,%d,%d", m_rgb_red, m_rgb_green, m_rgb_blue);
  mqtt_publish(MQTT_LIGHT_RGB_STATE_TOPIC, m_msg_buffer, true);
}

void publishDeviceState() {
  publishRGBState();
  publishRGBBrightness();
  publishRGBColor(); 
}

void setupDevice() {
  analogWriteRange(MAX_PWM_VALUE);

  // init the RGB led
  pinMode(RGB_LIGHT_BLUE_PIN, OUTPUT);
  pinMode(RGB_LIGHT_RED_PIN, OUTPUT);
  pinMode(RGB_LIGHT_GREEN_PIN, OUTPUT);

  setColor(0, 0, 0);
  setupLightSwitch();
}

void mqttConnectedDeviceCallback(PubSubClient mqttClient) {
  mqttClient.subscribe(MQTT_LIGHT_COMMAND_TOPIC);
  mqttClient.subscribe(MQTT_LIGHT_BRIGHTNESS_COMMAND_TOPIC);
  mqttClient.subscribe(MQTT_LIGHT_RGB_COMMAND_TOPIC);
}

void checkDevice() {
  checkLightSwitch();
}

boolean deviceCallback(char* p_topic, byte* p_payload, unsigned int p_length) {
  // concat the payload into a string
  String payload;
  for (uint8_t i = 0; i < p_length; i++) {
    payload.concat((char)p_payload[i]);
  }
  // handle message topic
  if (String(MQTT_LIGHT_COMMAND_TOPIC).equals(p_topic)) {
    // test if the payload is equal to "ON" or "OFF"
    if (payload.equals(String(LIGHT_ON))) {
      if (m_rgb_state != true) {
        m_rgb_state = true;
        setColor(m_rgb_red, m_rgb_green, m_rgb_blue);
        publishRGBState();
      }
    } else if (payload.equals(String(LIGHT_OFF))) {
      if (m_rgb_state != false) {
        m_rgb_state = false;
        setColor(0, 0, 0);
        publishRGBState();
      }
    }
  } else if (String(MQTT_LIGHT_BRIGHTNESS_COMMAND_TOPIC).equals(p_topic)) {
    uint8_t brightness = payload.toInt();
    if (brightness < 0 || brightness > 100) {
      // do nothing...
      return true;
    } else {
      m_rgb_brightness = brightness;
      setColor(m_rgb_red, m_rgb_green, m_rgb_blue);
      publishRGBBrightness();
    }
  } else if (String(MQTT_LIGHT_RGB_COMMAND_TOPIC).equals(p_topic)) {
    // get the position of the first and second commas
    uint8_t firstIndex = payload.indexOf(',');
    uint8_t lastIndex = payload.lastIndexOf(',');

    uint8_t rgb_red = payload.substring(0, firstIndex).toInt();
    if (rgb_red < 0 || rgb_red > 255) {
      return true;
    } else {
      m_rgb_red = rgb_red;
    }

    uint8_t rgb_green = payload.substring(firstIndex + 1, lastIndex).toInt();
    if (rgb_green < 0 || rgb_green > 255) {
      return true;
    } else {
      m_rgb_green = rgb_green;
    }

    uint8_t rgb_blue = payload.substring(lastIndex + 1).toInt();
    if (rgb_blue < 0 || rgb_blue > 255) {
      return true;
    } else {
      m_rgb_blue = rgb_blue;
    }

    setColor(m_rgb_red, m_rgb_green, m_rgb_blue);
    publishRGBColor();
    return true;
  }
  return false;
}

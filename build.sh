#!/bin/bash

source setenv.sh

OTA_PASS_MD5_HASH=($(echo $OTA_PASS | md5sum))
export PLATFORMIO_SRC_BUILD_FLAGS="'-DMODULE=$MODULE' '-DSTA_PASS=\"$WIFI_PASS\"' '-DSTA_SSID=\"$WIFI_SSID\"' '-DMQTT_HOST=\"$MQTT_HOST\"' '-DOTA_HOST=\"$OTA_HOST\"' '-DOTA_PASS=\"$OTA_PASS\"' '-DOTA_PASS_MD5_HASH=\"$OTA_PASS_MD5_HASH\"' '-DPROJECT=\"$PROJECT\"' '-DVERSION=$VERSION_INTERNAL' '-DVERSION_BIN=\"$VERSION\"'"

if [ $# -eq 0 ]
then
    FILENAME=${PROJECT}-${VERSION}.bin
    echo "Compiling version $VERSION as a binary ($FILENAME)."
    platformio run
    retval=$?
    if [ $retval -eq 0 ]; then
      cp .pioenvs/esp8266/firmware.bin $FILENAME
    fi
else
    if [ $1 == "usb" ]
    then
        echo "Compiling version $VERSION and flashing via USB."
        platformio run --target upload
        retval=$?
    else
        echo "Compiling version $VERSION and flashing via OTA to $1."
        export PLATFORMIO_UPLOAD_FLAGS="-a $OTA_PASS"
        platformio run --target upload --upload-port=$1
        retval=$?
    fi
fi

if [ $retval -eq 0 ]; then
    # Auto-increment the version in this script.
    sed -i -re "s/VERSION=\"[0-9]+\"/VERSION=\"$(( ++VERSION ))\"/" setenv.sh
fi

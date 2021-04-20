# ESP32 leak sensor

## Overview

The code is very similar to my [ESP32-room-sensor](https://github.com/msgarbossa/esp32-room-sensor) project.

- Wakes from deep sleep every 15 minutes and sends MQTT messages for liveliness and to report the WiFi signal strength
- Uses ESP32 touch sensor to detect leaks
- The touch sensor is configured to wake the ESP32 from deep sleep if the touch sensor is activated.
- MQTT messages are sent for changes in leak state (dry/wet)
- A piezo buzzer makes noise when leak state changes and periodically while wet
- Over-the-Air (OTA) update is attempted when first powered on

## Parts

- [ESP32 devkit - 30 pin, integrated antenna](https://www.aliexpress.com/item/1005001267643044.html).  I have a few 38-pin [ESP32-WROOM-32U](https://www.aliexpress.com/item/32807887667.html) boards w/ external antennas I want to try since the signal tends to be weak where I have these placed.
- [inexepensive cases (10-pack)](https://www.amazon.com/gp/product/B07HKWNB93/). I'd like to make a small 3D printed case, but these aren't placed where they are visible anyway.
- [thumbtacks](https://www.amazon.com/gp/product/B0019IGM0E/) (200-count)
- [alarm wire](https://www.amazon.com/gp/product/B01CT06M0A/) (500')
- Passive piezo buzzer (ZLFY) or anything similar to make noise.  I salvaged these from some tower fans that I didn't want making noise.

## Diagram

![diagram](/img/diagram.png)

## Assembly

![bottom](/img/leak-sensor-bottom.jpg)

![top](/img/leak-sensor-top.jpg)

## Node-RED

![Node-RED flow](/node-red/node-red-flow.png)
[JSON for above flow](node-red/flow.json)

The beginning of the flow starts out very simple.  Node-RED listens on the MQTT topic for the sensor.  The messages get converted from JSON to payload objects.

To understand the rest of the flow, it's important to go over how the Prometheus node exporter works in Node-RED.  The orange metric boxes to the right-side of the flow generate the prometheus data shown below at \<node_red_url\>:1880/metrics.

```
# HELP leak1 leak1 kitchen
# TYPE leak1 gauge
leak1 0

# HELP signal_leak1 signal leak1
# TYPE signal_leak1 gauge
signal_leak1 -71
```

The challenge has been dealing with sensors that go offline.  The metric node would not receive an updated value and continue to display an old value.  This would show up in the graphs in Grafana as a flat-line.  There might be a more elegant solution, but what I've done for now is to use an inject node to regularly inject a healthcheck message into the flow.  The healthcheck function node uses node context to store the time of the last valid message that came from MQTT.  When the healthcheck message is injected, the time of the inject message is compared to the time of the last valid message.  If the time difference is greater than the threshold, the healthcheck node's status is set to "offline" and a new message is generated and passed along with the metric values set to zero.  The Grafana graphs are set to only display values greater than zero.

The functions to the left of each of the orange metric nodes simply reformat the payload messages to set the metric values as required for the Promtheus metric nodes.

## Over the Air (OTA) Updates

OTA updates seem more commonly used with sensors that do not utilize deep sleep.  To get OTA updates to work and minimize the wake time, the bootcount variable is used with RTC_DATA_ATTR to maintain the number of times the ESP32 wakes from sleep after being powered on.  The OTA update is only checked the first time the ESP32 is powered on.  The OTA update process works the same is it normally would except that the ESP32 needs a hard restart to reset the counter to zero and check for the update.  The IP address of the ESP32 needs to be updated in platform.ini file and the upload_port entry for USB needs to be commented out.
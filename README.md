# Mbed DHT wrapper

## Basic info

This project depends on https://github.com/atoy40/mbed-dht, add it to your Mbed project too

The main purpose is to manage the sensor reading for multiple DHT sensors in a single place periodically on a separate thread so it won't hang the main threat

Tested with Mbed os 6.17.0 and STM32 Nucleo F103RB

Disclaimer: I'm not that familiar with Mbed and also hardly wrote any c++ code in the past few years, the code might contain problems

## Example usage

```c++
#include "mbed.h"
#include "dht_wrapper.h"

int main() {
    DhtWrapper dhtWrapper({{PD_2, DHT::DHT11}}, 5s);

    while (true) {
        printf("Temperature: %.2f\n", dhtWrapper.getTemperature(PD_2));
        printf("Humidity: %.2f\n", dhtWrapper.getHumidity(PD_2));
        ThisThread::sleep_for(1000ms);
    }
}
```

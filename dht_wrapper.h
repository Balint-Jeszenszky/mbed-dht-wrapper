#ifndef DHT_WRAPPER
#define DHT_WRAPPER

#include "mbed.h"
#include "DHT.h"
#include <vector>
#include <map>

class DhtWrapper {
public:
    struct SensorConfig {
        PinName pin;
        DHT::Family dhtType;
    };

    DhtWrapper(const std::vector<SensorConfig>& sensorConfigs);
    DhtWrapper(const std::vector<SensorConfig>& sensorConfigs, const Kernel::Clock::duration_u32 refreshInterval);
    ~DhtWrapper();

    void refresh(const PinName pinName);
    void refreshAll();
    float getTemperature(const PinName pinName);
    float getHumidity(const PinName pinName);

private:
    std::map<PinName, DHT> sensors;
    Thread thread;
    const Kernel::Clock::duration_u32 refreshInterval;
    mutable rtos::Mutex sensorsMutex;
    EventFlags threadFlags;

    void refreshThread();
};

#endif

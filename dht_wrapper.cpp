#include "dht_wrapper.h"

static constexpr uint32_t STOP_FLAG = 0x01;

DhtWrapper::DhtWrapper(const std::vector<SensorConfig>& sensorConfigs) : DhtWrapper(sensorConfigs, 0ms) { }

DhtWrapper::DhtWrapper(const std::vector<SensorConfig>& sensorConfigs, const Kernel::Clock::duration_u32 refreshInterval)
  : sensorsMutex(), threadFlags(), sensors(), refreshInterval(refreshInterval) {
    for (SensorConfig config : sensorConfigs) {
        DHT dht(config.pin, config.dhtType);
        sensors.emplace(
            std::piecewise_construct,
            std::forward_as_tuple(config.pin),
            std::forward_as_tuple(config.pin, config.dhtType)
        );
    }

    if (refreshInterval <= 0ms) {
        return;
    }

    thread.start(callback(this, &DhtWrapper::refreshThread));
}

DhtWrapper::~DhtWrapper() {
    if (refreshInterval > 0ms) {
        threadFlags.set(STOP_FLAG);
        thread.join();
    }
}

void DhtWrapper::refresh(const PinName pinName) {
    ScopedLock<rtos::Mutex> lock(sensorsMutex);
    sensors.at(pinName).read();
}

void DhtWrapper::refreshAll() {
    ScopedLock<rtos::Mutex> lock(sensorsMutex);
    for (auto& sensor : sensors) {
        sensor.second.read();
    }
}

float DhtWrapper::getTemperature(const PinName pinName) {
    ScopedLock<rtos::Mutex> lock(sensorsMutex);
    return sensors.at(pinName).getTemperature();
}

float DhtWrapper::getHumidity(const PinName pinName) {
    ScopedLock<rtos::Mutex> lock(sensorsMutex);
    return sensors.at(pinName).getHumidity();
}

void DhtWrapper::refreshThread() {
    while (true) {
        refreshAll();

        uint32_t flags_set = threadFlags.wait_any_for(STOP_FLAG, refreshInterval);

        if (flags_set & STOP_FLAG) {
            break;
        }
    }
}

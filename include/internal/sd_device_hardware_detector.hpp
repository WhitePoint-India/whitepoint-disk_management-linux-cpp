#ifndef SD_DEVICE_HARDWARE_DETECTOR_HPP
#define SD_DEVICE_HARDWARE_DETECTOR_HPP

#include "hardware_detector.hpp"

class SDDeviceHardwareDetector : public HardwareDetector {
    public :
        [[nodiscard]] std::vector<std::unique_ptr<Disk>> fetchDisks() override;
};
#endif // SD_DEVICE_HARDWARE_DETECTOR_HPP

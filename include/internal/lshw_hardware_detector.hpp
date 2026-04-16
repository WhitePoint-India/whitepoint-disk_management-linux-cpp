
#ifndef LSHW_HARDWARE_DETECTOR_HPP
#define LSHW_HARDWARE_DETECTOR_HPP

#include <hardware_detector.hpp>

class LshwHardwareDetector : public HardwareDetector {
public:
    [[nodiscard]] std::vector<std::unique_ptr<Disk>> fetchDisks() override;
};

#endif // LSHW_HARDWARE_DETECTOR_HPP

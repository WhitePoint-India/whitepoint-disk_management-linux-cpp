
#ifndef HARDWARE_DETECTOR_HPP
#define HARDWARE_DETECTOR_HPP

#include <vector>
#include <memory>

class Disk;

class HardwareDetector {
public:
    virtual ~HardwareDetector() noexcept = default;

    [[nodiscard]] virtual std::vector<std::unique_ptr<Disk>> fetchDisks() = 0;
};

#endif // HARDWARE_DETECTOR_HPP

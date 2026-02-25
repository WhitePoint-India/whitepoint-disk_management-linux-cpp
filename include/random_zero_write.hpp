
#ifndef RANDOM_ZERO_WRITE_HPP
#define RANDOM_ZERO_WRITE_HPP

#include <operations.hpp>

namespace DiskManagement {

class RandomZeroWrite: public SingletonMethod<RandomZeroWrite> {
    friend class SingletonMethod<RandomZeroWrite>;
    RandomZeroWrite();
public:
    const std::string& getTitle() const override;
    const std::string& getDescription() const override;
    const std::vector<Stage>& getStages() const override;
};

} // namespace DiskManagement

#endif // RANDOM_ZERO_WRITE_HPP

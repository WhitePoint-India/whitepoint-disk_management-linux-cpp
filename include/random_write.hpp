
#ifndef RANDOM_WRITE_HPP
#define RANDOM_WRITE_HPP

#include <operations.hpp>

namespace DiskManagement {

class RandomWrite: public SingletonMethod<RandomWrite> {
    friend class SingletonMethod<RandomWrite>;
    RandomWrite();
public:
    const std::string& getTitle() const override;
    const std::string& getDescription() const override;
    const std::vector<Stage>& getStages() const override;
};

} // namespace DiskManagement

#endif // RANDOM_WRITE_HPP

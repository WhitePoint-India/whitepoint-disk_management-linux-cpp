
#ifndef GUTMANN_HPP
#define GUTMANN_HPP

#include <operations.hpp>

namespace DiskManagement {

class Gutmann: public SingletonMethod<Gutmann> {
    friend class SingletonMethod<Gutmann>;
    Gutmann();
public:
    const std::string& getTitle() const override;
    const std::string& getDescription() const override;
    const std::vector<Stage>& getStages() const override;
};

} // namespace DiskManagement

#endif // GUTMANN_HPP

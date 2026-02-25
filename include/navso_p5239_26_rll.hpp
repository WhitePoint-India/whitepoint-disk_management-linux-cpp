
#ifndef NAVSO_P5239_26_RLL_HPP
#define NAVSO_P5239_26_RLL_HPP

#include <operations.hpp>

namespace DiskManagement {

class NAVSOP523926RLL: public SingletonMethod<NAVSOP523926RLL> {
    friend class SingletonMethod<NAVSOP523926RLL>;
    NAVSOP523926RLL();
public:
    const std::string& getTitle() const override;
    const std::string& getDescription() const override;
    const std::vector<Stage>& getStages() const override;
};

} // namespace DiskManagement

#endif // NAVSO_P5239_26_RLL_HPP

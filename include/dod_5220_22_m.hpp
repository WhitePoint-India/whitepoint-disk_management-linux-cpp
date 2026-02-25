
#ifndef DOD_5220_22_M_HPP
#define DOD_5220_22_M_HPP

#include <operations.hpp>

namespace DiskManagement {

class DoD522022M: public SingletonMethod<DoD522022M> {
    friend class SingletonMethod<DoD522022M>;
    DoD522022M();
public:
    const std::string& getTitle() const override;
    const std::string& getDescription() const override;
    const std::vector<Stage>& getStages() const override;
};

} // namespace DiskManagement

#endif // DOD_5220_22_M_HPP

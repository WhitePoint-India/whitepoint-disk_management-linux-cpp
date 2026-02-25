
#ifndef BSI_VSITR_HPP
#define BSI_VSITR_HPP

#include <operations.hpp>

namespace DiskManagement {

class BSIVSITR: public SingletonMethod<BSIVSITR> {
    friend class SingletonMethod<BSIVSITR>;
    BSIVSITR();
public:
    const std::string& getTitle() const override;
    const std::string& getDescription() const override;
    const std::vector<Stage>& getStages() const override;
};

} // namespace DiskManagement

#endif // BSI_VSITR_HPP

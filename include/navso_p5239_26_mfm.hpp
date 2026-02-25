
#ifndef NAVSO_P5239_26_MFM_HPP
#define NAVSO_P5239_26_MFM_HPP

#include <operations.hpp>

namespace DiskManagement {

class NAVSOP523926MFM: public SingletonMethod<NAVSOP523926MFM> {
    friend class SingletonMethod<NAVSOP523926MFM>;
    NAVSOP523926MFM();
public:
    const std::string& getTitle() const override;
    const std::string& getDescription() const override;
    const std::vector<Stage>& getStages() const override;
};

} // namespace DiskManagement

#endif // NAVSO_P5239_26_MFM_HPP


#ifndef AFSSI_5020_HPP
#define AFSSI_5020_HPP

#include <operations.hpp>

namespace DiskManagement {

class AFSSI5020: public SingletonMethod<AFSSI5020> {
    friend class SingletonMethod<AFSSI5020>;
    AFSSI5020();
public:
    const std::string& getTitle() const override;
    const std::string& getDescription() const override;
    const std::vector<Stage>& getStages() const override;
};

} // namespace DiskManagement

#endif // AFSSI_5020_HPP

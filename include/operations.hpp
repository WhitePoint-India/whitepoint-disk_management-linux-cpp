
#ifndef OPERATIONS_HPP
#define OPERATIONS_HPP

#include <string>
#include <string_view>
#include <vector>
#include <disks.hpp>

namespace DiskManagement {

struct Stage {
    std::string_view id;
    std::string_view name;
    int index;
    int total;
};

class Progress {
private:
    const unsigned long long _bytesProcessed;
    const unsigned long long _totalBytes;
public:
    Progress(unsigned long long bytesProcessed, unsigned long long totalBytes);
    [[nodiscard]] unsigned long long bytesProcessed() const;
    [[nodiscard]] unsigned long long totalBytes() const;
    [[nodiscard]] double fractionCompleted() const;
    [[nodiscard]] double percentageCompleted() const;
};

class DiskDeleteMethod {
private:
    const std::string id;
protected:
    DiskDeleteMethod(const std::string& id);
public:
    class Delegate {
        public:
        virtual void onStageStarted(const Stage& stage) = 0;
        virtual void onProgress(const Stage& stage, const Progress& progress) = 0;
        virtual void onStageCompleted(const Stage& stage) = 0;
        virtual void onCompleted() = 0;
        virtual void onError(const std::string& message) = 0;
        [[nodiscard]] virtual bool shouldCancel();
        virtual ~Delegate() = default;
    };

    [[nodiscard]] const std::string& getID() const;
    [[nodiscard]] virtual const std::string& getTitle() const = 0;
    [[nodiscard]] virtual const std::string& getDescription() const = 0;

    [[nodiscard]] virtual const std::vector<Stage>& getStages() const = 0;

    virtual void deleteDisk(Disks::ATADisk& disk, Delegate& delegate);
    virtual void deleteDisk(Disks::NVMeDisk& disk, Delegate& delegate);
    virtual void deleteDisk(Disks::USBDisk& disk, Delegate& delegate);

    virtual ~DiskDeleteMethod() = default;
};

template<typename Derived>
class SingletonMethod : public DiskDeleteMethod {
protected:
    using DiskDeleteMethod::DiskDeleteMethod;
public:
    SingletonMethod(const SingletonMethod&) = delete;
    SingletonMethod& operator=(const SingletonMethod&) = delete;
    SingletonMethod(SingletonMethod&&) = delete;
    SingletonMethod& operator=(SingletonMethod&&) = delete;

    [[nodiscard]] static Derived& shared() {
        static Derived instance;
        return instance;
    }
};

} // namespace DiskManagement

#endif // OPERATIONS_HPP
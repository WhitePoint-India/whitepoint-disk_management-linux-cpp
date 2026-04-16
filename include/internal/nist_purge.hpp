
#ifndef NIST_PURGE_HPP
#define NIST_PURGE_HPP

#include <disk_sanitization_interface.hpp>
#include <sanitization_method_registry.hpp>

class NISTPurge: public DiskSanitizationInterface, private AutoRegisterMethod<NISTPurge> {
    public:
        [[nodiscard]] static NISTPurge& shared();

        NISTPurge(const NISTPurge&) = delete;
        NISTPurge(NISTPurge&&) = delete;
        NISTPurge& operator=(const NISTPurge&) = delete;
        NISTPurge& operator=(NISTPurge&&) = delete;

        void sanitize(Disk& disk, Callback callback) override;

    private:
        NISTPurge();
};

#endif // NIST_PURGE_HPP

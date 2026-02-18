#ifndef DISKS_TPP
#define DISKS_TPP

namespace DiskManagement {

template <typename Self, typename DeleteOperation, typename Delegate>
void Deletable::deleteDisk(this Self& self, const DeleteOperation& operation, Delegate& delegate) {
    operation.deleteDisk(self, delegate);
}

} // namespace DiskManagement

#endif // DISKS_TPP

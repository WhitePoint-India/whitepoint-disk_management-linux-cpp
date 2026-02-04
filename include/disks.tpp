#ifndef DISKS_TPP
#define DISKS_TPP

template <typename Derived>
template <typename DeleteMethod, typename Callback>
void DiskManagement::Deletable<Derived>::deleteDisk(const DeleteMethod& method, Callback callback) {
    method.deleteDisk(static_cast<Derived&>(*this), callback);
}

#endif // DISKS_TPP

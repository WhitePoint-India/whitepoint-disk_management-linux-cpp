
#include <iostream>
#include <iomanip>
#include <disk-management/delete-methods/secure-erase.h>
#include <disk-management/delete-methods/gutmann-method.h>
#include <disk-management/details.h>

using namespace std;

int main() {
     DiskManagement::Disk disk = DiskManagement::Disk();

     
     SecureErase method = SecureErase();
     disk.deleteDisk(method);

     GutmannMethod methodx = GutmannMethod();

     disk.deleteDisk(methodx);

    return 0;
}
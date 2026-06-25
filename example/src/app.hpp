#ifndef EXAMPLE_APP_HPP
#define EXAMPLE_APP_HPP

// Interactive, menu-driven disk-sanitization CLI built on the disk_management
// library.
namespace app {

// Runs the full flow (choose method -> choose disks -> confirm -> erase ->
// summary). Returns a process exit code (0 = all selected disks succeeded).
[[nodiscard]] int run();

}  // namespace app

#endif  // EXAMPLE_APP_HPP

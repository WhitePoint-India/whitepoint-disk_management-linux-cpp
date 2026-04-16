
#ifndef SANITIZATION_CALLBACK_HPP
#define SANITIZATION_CALLBACK_HPP

#include <functional>
#include <sanitization_progress.hpp>

using SanitizationCallback = std::function<void(const SanitizationProgress progress)>;

#endif // SANITIZATION_CALLBACK_HPP
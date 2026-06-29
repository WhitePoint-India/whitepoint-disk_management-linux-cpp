
#ifndef VERIFICATION_METHOD
#define VERIFICATION_METHOD

#include <verifiable.hpp>

class VerificationMethod {

private:
    double sampling_;

public:

    VerificationMethod(double sampling = 1.0);

    using Callback = std::function<void(double fractionCompleted)>;

    void verify(Verifiable& verifiable, Callback callback);
};

#endif // VERIFICATION_METHOD
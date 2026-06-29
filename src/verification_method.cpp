
#include <verification_method.hpp>


VerificationMethod::VerificationMethod(double sampling) : sampling_(sampling) {

}


void VerificationMethod::verify(Verifiable& verifiable, Callback callback) {
    verifiable.verifySampling(sampling_, std::nullopt, callback);
}
#include "mirants/util/base64/base64.h"
#include "mirants/util/base64/modp_base64/modp_b64.h"
#include "mirants/util/stl_util.h"


namespace mirants {

bool Base64Encode(const std::string& input, std::string* output) {
  std::string x(modp_b64_encode_len(input.size()), '\0');
  int d = modp_b64_encode(string_as_array(&x),
                          input.data(),
                          static_cast<int>(input.size()));
  if (d < 0) {
    return false;
  }
  x.erase(d, std::string::npos);
  output->swap(x);
  return true;
}
  
bool Base64Decode(const std::string& input, std::string* output) {
  std::string x(modp_b64_decode_len(input.size()), '\0');
  int d = modp_b64_decode(string_as_array(&x),
                          input.data(),
                          static_cast<int>(input.size()));
  if (d < 0) {
    return false;
  }
  x.erase(d, std::string::npos);
  output->swap(x);
  return true;
}
  
}  // namespace mirants
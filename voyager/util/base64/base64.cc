// Copyright (c) 2016 Mirants Lu. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "voyager/util/base64/base64.h"
#include "voyager/util/base64/modp_base64/modp_b64.h"


namespace voyager {

bool Base64Encode(const std::string& input, std::string* output) {
  std::string x(modp_b64_encode_len(input.size()), '\0');
  int d = modp_b64_encode(&*x.begin(),
                          input.data(),
                          static_cast<int>(input.size()));
  if (d < 0) {
    return false;
  }
  x.erase(static_cast<size_t>(d), std::string::npos);
  output->swap(x);
  return true;
}

bool Base64Decode(const std::string& input, std::string* output) {
  std::string x(modp_b64_decode_len(input.size()), '\0');
  int d = modp_b64_decode(&*x.begin(),
                          input.data(),
                          static_cast<int>(input.size()));
  if (d < 0) {
    return false;
  }
  x.erase(static_cast<size_t>(d), std::string::npos);
  output->swap(x);
  return true;
}

}  // namespace voyager

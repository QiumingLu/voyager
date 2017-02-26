// Copyright (c) 2016 Mirants Lu. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "voyager/util/testharness.h"
#include "voyager/util/slice.h"
#include "voyager/util/logging.h"
#include "voyager/util/string_util.h"
#include "voyager/util/stringprintf.h"
#include "voyager/util/timeops.h"

namespace voyager {

class SliceTest  { };
class StatusTest { };
class StringUtilTest { };
class StringPrintfTest { };
class TimeopsTest { };

TEST(SliceTest, TestSlice) {
  Slice slice;
  ASSERT_EQ(slice.empty(), true);
  char p[] = "test slice";
  char* q = p;
  Slice slice2(p, strlen(p));
  Slice slice3(q);

  ASSERT_EQ(slice2.size(), strlen(p));
  ASSERT_EQ(slice3.data(), q);
  ASSERT_EQ(slice3[0], 't');
  ASSERT_EQ(slice2 == slice3, true);
  slice3.clear();
  ASSERT_EQ(slice2 == slice3, false);
  ASSERT_LT(slice3.compare(slice2), 0);
  ASSERT_EQ(slice2 != slice3, true); 

  std::string str = "string slice";
  Slice slice4(str);
  ASSERT_EQ(slice4.starts_with("string"), true);
  slice4.remove_prefix(7);
  ASSERT_EQ(slice4.ToString(), "slice");
  ASSERT_EQ(slice4[2], 'i');
}

TEST(StatusTest, TestStatus) {
  Status st(Status::OK());
  ASSERT_EQ(st.ok(), true);
  Status st2 = st;
  st2 = Status::NotFound("test status");
  ASSERT_EQ(st2.IsNotFound(), true);
  Status st3(Status::Corruption("test status"));
  VOYAGER_LOG(INFO) << st3.ToString();
  Status st4 = st3;
  ASSERT_EQ(st4.IsCorruption(), true);
}

TEST(StringUtilTest, TestStringUtil) {
  ASSERT_EQ(ascii_isalnum('1'), true);
  ASSERT_EQ(ascii_isalnum('a'), true);
  ASSERT_EQ(ascii_isalnum('A'), true);
  ASSERT_EQ(ascii_isdigit('2'), true);
  ASSERT_EQ(ascii_isspace(' '), true);
  ASSERT_EQ(ascii_isupper('A'), true);
  ASSERT_EQ(ascii_islower('z'), true);
  ASSERT_EQ(ascii_tolower('A'), 'a');
  ASSERT_EQ(ascii_toupper('a'), 'A');
  ASSERT_EQ(ascii_toupper('A'), 'A');
  ASSERT_EQ(hex_digit_to_int('a'), 10);
  ASSERT_EQ(hex_digit_to_int('A'), 10);
  ASSERT_EQ(hex_digit_to_int('8'), 8);
  ASSERT_EQ(HasPrefixString("string", "str"), true);
  ASSERT_EQ(StripPrefixString("string test", "string"), " test");
  ASSERT_EQ(StripPrefixString("string", "char"), "string");
  ASSERT_EQ(HasSuffixString("string", "ing"), true);
  ASSERT_EQ(HasSuffixString("string", "ok"), false);
  ASSERT_EQ(StripSuffixString("string test", "test"), "string ");
  ASSERT_EQ(StripSuffixString("string", "test"), "string");
  std::string s = "okOKokOKok";
  LowerString(&s);
  ASSERT_EQ(s, "okokokokok");
  UpperString(&s);
  ASSERT_EQ(s, "OKOKOKOKOK");
  ASSERT_EQ(ToLower(s), "okokokokok");
  ASSERT_EQ(ToUpper(s), "OKOKOKOKOK");

  std::string s1 = "ok ok ok ok ok";
  std::vector<std::string> v = Split(s1, " ");
  std::string s2;
  JoinStrings(v, " ", &s2);
  ASSERT_EQ(s1, s2);
  std::string s3 = StringReplace(s2, "ok", "OK", true);
  std::string s4 = "OK OK OK OK OK";
  ASSERT_EQ(s3, s4);
  std::string s5 = "   hhhh";
  StripWhitespace(&s5);
  ASSERT_EQ(s5, "hhhh");
}

TEST(StringPrintfTest, TestStringPrintf) {
  ASSERT_EQ("", StringPrintf("%s", std::string().c_str()));
  ASSERT_EQ("", StringPrintf("%s", ""));
  std::string value("Hello");
  const char* empty = "";
  StringAppendF(&value, "%s", empty);
  ASSERT_EQ("Hello", value);
  StringAppendF(&value, "%s", " word");
  ASSERT_EQ("Hello word", value);
  ASSERT_EQ("123", StringPrintf("%d", 123));
}

TEST(TimeopsTest, TestTimeops) {
  VOYAGER_LOG(INFO) << timeops::NowMicros();
  VOYAGER_LOG(INFO) << timeops::FormatTimestamp(timeops::NowMicros());
}

}  // namespace voyager

int main(int argc, char** argv) {
  return voyager::test::RunAllTests();
}

// Copyright (c) 2013, Cloudera, inc.

#include <boost/foreach.hpp>
#include <gtest/gtest.h>
#include <string>

#include "util/trace.h"
#include "util/test_util.h"

namespace kudu {

class TraceTest : public KuduTest {
};

// Replace all digits in 's' with the character 'X'.
static string XOutDigits(const string& s) {
  string ret;
  ret.reserve(s.size());
  BOOST_FOREACH(char c, s) {
    if (isdigit(c)) {
      ret.push_back('X');
    } else {
      ret.push_back(c);
    }
  }
  return ret;
}

TEST_F(TraceTest, TestBasic) {
  scoped_refptr<Trace> t(new Trace);
  TRACE_TO(t, "hello $0, $1", "world", 12345);
  TRACE_TO(t, "goodbye $0, $1", "cruel world", 54321);

  string result = XOutDigits(t->DumpToString());
  ASSERT_EQ("XXXX XX:XX:XX.XXXXXX trace-test.cc:XX] hello world, XXXXX\n"
            "XXXX XX:XX:XX.XXXXXX trace-test.cc:XX] goodbye cruel world, XXXXX\n",
            result);
}

TEST_F(TraceTest, TestAttach) {
  scoped_refptr<Trace> traceA(new Trace);
  scoped_refptr<Trace> traceB(new Trace);
  {
    ADOPT_TRACE(traceA.get());
    EXPECT_EQ(traceA.get(), Trace::CurrentTrace());
    {
      ADOPT_TRACE(traceB.get());
      EXPECT_EQ(traceB.get(), Trace::CurrentTrace());
      TRACE("hello from traceB");
    }
    EXPECT_EQ(traceA.get(), Trace::CurrentTrace());
    TRACE("hello from traceA");
  }
  EXPECT_TRUE(Trace::CurrentTrace() == NULL);
  TRACE("this goes nowhere");

  EXPECT_EQ(XOutDigits(traceA->DumpToString()),
            "XXXX XX:XX:XX.XXXXXX trace-test.cc:XX] hello from traceA\n");
  EXPECT_EQ(XOutDigits(traceB->DumpToString()),
            "XXXX XX:XX:XX.XXXXXX trace-test.cc:XX] hello from traceB\n");
}
} // namespace kudu

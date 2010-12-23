// Copyright 2010, Takuya Akiba
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//     * Neither the name of Takuya Akiba nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include "minimal_perfect_hash.h"
#include <algorithm>
#include <cstdlib>
#include <sstream>
#include <vector>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <gtest/gtest.h>

using testing::Types;

namespace {
template<typename Hash, typename Key>
void CheckHashValues(const Hash &hash, const std::vector<Key> &keys) {
  std::vector<uint32_t> h(keys.size());
  for (size_t i = 0; i < keys.size(); ++i) {
    h[i] = hash.GetHash(keys[i]);
    EXPECT_LE(0, h[i]);
    EXPECT_GT(hash.GetRange(), h[i]);
  }
  std::sort(h.begin(), h.end());
  EXPECT_TRUE(std::adjacent_find(h.begin(), h.end()) == h.end());
}

template<typename Key, typename Generator>
class PerfectHashTester {
public:
  static void Test(size_t size) {
    std::vector<Key> keys;
    Generator::Generate(size, &keys);
    minimal_perfect_hash::PerfectHash<Key> ph;
    EXPECT_EQ(0, ph.Build(keys));
    CheckHashValues(ph, keys);
  }

  static void TestSerialization(size_t size) {
    std::vector<Key> keys;
    Generator::Generate(size, &keys);
    std::string data;
    // Save
    {
      minimal_perfect_hash::PerfectHash<Key> mph;
      ASSERT_EQ(0, mph.Build(keys));
      std::ostringstream oss;
      boost::archive::text_oarchive oa(oss);
      oa << mph;
      data = oss.str();
    }
    // Load and test
    {
      std::istringstream iss(data);
      boost::archive::text_iarchive ia(iss);
      minimal_perfect_hash::PerfectHash<Key> mph;
      ia >> mph;
      CheckHashValues(mph, keys);
    }
  }
};

template<typename Key, typename Generator>
class MinimalPerfectHashTester {
public:
  static void Test(size_t size) {
    std::vector<Key> keys;
    Generator::Generate(size, &keys);
    minimal_perfect_hash::MinimalPerfectHash<Key> mph;
    EXPECT_EQ(0, mph.Build(keys));
    CheckHashValues(mph, keys);
    EXPECT_EQ(keys.size(), mph.GetRange());
  }

  static void TestSerialization(size_t size) {
    std::vector<Key> keys;
    Generator::Generate(size, &keys);
    std::string data;
    // Save
    {
      minimal_perfect_hash::MinimalPerfectHash<Key> mph;
      ASSERT_EQ(0, mph.Build(keys)) << keys.size();
      std::ostringstream oss;
      boost::archive::text_oarchive oa(oss);
      oa << mph;
      data = oss.str();
    }
    // Load and test
    {
      std::istringstream iss(data);
      boost::archive::text_iarchive ia(iss);
      minimal_perfect_hash::MinimalPerfectHash<Key> mph;
      ia >> mph;
      CheckHashValues(mph, keys);
      EXPECT_EQ(keys.size(), mph.GetRange());
    }
  }
};

template<typename T> class RandomElementGenerator;

template<> class RandomElementGenerator<int> {
public:
  static int Generate() {
    return rand();
  }
};

template<typename T, typename U>
class RandomElementGenerator<std::pair<T, U> > {
public:
  static std::pair<T, U> Generate() {
    return std::make_pair(RandomElementGenerator<T>::Generate(),
                          RandomElementGenerator<U>::Generate());
  }
};

template<typename T> class RandomElementGenerator<std::vector<T> > {
  static const double kLastingProbability = 0.9;
public:
  static std::vector<T> Generate() {
    std::vector<T> res;
    while (rand() < RAND_MAX * kLastingProbability) {
      res.push_back(RandomElementGenerator<T>::Generate());
    }
    return res;
  }
};

template<> class RandomElementGenerator<std::string> {
  static const double kLastingProbability = 0.9;
public:
  static std::string Generate() {
    std::vector<int> t = RandomElementGenerator<std::vector<int> >::Generate();
    std::string s(t.size(), ' ');
    for (size_t i = 0; i < t.size(); ++i) {
      s[i] = 'a' + t[i] % 26;
    }
    return s;
  }
};

template<typename T> class RandomArrayGenerator {
public:
  static void Generate(size_t s, std::vector<T> *v) {
    std::set<T> se;
    while (se.size() < s) {
      se.insert(RandomElementGenerator<T>::Generate());
    }
    v->resize(s);
    std::copy(se.begin(), se.end(), v->begin());
  }
};

template<typename T> class OrderedArrayGenerator;

template<> class OrderedArrayGenerator<int> {
public:
  static void Generate(size_t s, std::vector<int> *v) {
    v->resize(s);
    for (size_t i = 0; i < s; ++i) {
      v->at(i) = i;
    }
  }
};

template<> class OrderedArrayGenerator<std::pair<int, int> > {
public:
  static void Generate(size_t s, std::vector<std::pair<int, int> > *v) {
    v->clear();
    for (size_t t = 0; ; ++t) {
      for (size_t a = 0; a <= t; ++a) {
        if (v->size() >= s) return;
        size_t b = t - a;
        v->push_back(std::make_pair(a, b));
      }
    }
  }
};

template<> class OrderedArrayGenerator<std::vector<int> > {
public:
  static void Generate(size_t s, std::vector<std::vector<int> > *v) {
    int base = 2 + rand() % 4;  // [2, 5]

    for (int t = 1; ; ++t) {
      std::vector<int> a(t);
      for (;;) {
        if (v->size() >= s) {
          return;
        }
        v->push_back(a);
        int c = 1;
        for (int i = t - 1; i >= 0; --i) {
          a[i] += c;
          c = a[i] / base;
          a[i] %= base;
        }
        if (c > 0) break;
      }
    }
  }
};

template<> class OrderedArrayGenerator<std::string> {
public:
  static void Generate(size_t s, std::vector<std::string> *v) {
    std::vector<std::vector<int> > tv;
    OrderedArrayGenerator<std::vector<int> >::Generate(s, &tv);
    v->resize(s);
    for (size_t i = 0; i < s; ++i) {
      v->at(i).resize(tv[i].size());
      for (size_t j = 0; j < tv[i].size(); ++j) {
        v->at(i)[j] = 'a' + (tv[i][j] % 26);
      }
    }
  }
};
}  // namespace

typedef Types<
  // Test |PerfectHash| with basic types
  PerfectHashTester<int, RandomArrayGenerator<int> >,
  PerfectHashTester<int, OrderedArrayGenerator<int> >,
  PerfectHashTester<std::pair<int, int>, RandomArrayGenerator<std::pair<int, int> > >,
  PerfectHashTester<std::pair<int, int>, OrderedArrayGenerator<std::pair<int, int> > >,
  PerfectHashTester<std::string, RandomArrayGenerator<std::string> >,
  PerfectHashTester<std::string, OrderedArrayGenerator<std::string> >,
  PerfectHashTester<std::vector<int>, RandomArrayGenerator<std::vector<int> > >,
  PerfectHashTester<std::vector<int>, OrderedArrayGenerator<std::vector<int> > >,
  // Test |MinimalPerfectHash| with basic types
  MinimalPerfectHashTester<int, RandomArrayGenerator<int> >,
  MinimalPerfectHashTester<int, OrderedArrayGenerator<int> >,
  MinimalPerfectHashTester<std::pair<int, int>, RandomArrayGenerator<std::pair<int, int> > >,
  MinimalPerfectHashTester<std::pair<int, int>, OrderedArrayGenerator<std::pair<int, int> > >,
  MinimalPerfectHashTester<std::string, RandomArrayGenerator<std::string> >,
  MinimalPerfectHashTester<std::string, OrderedArrayGenerator<std::string> >,
  MinimalPerfectHashTester<std::vector<int>, RandomArrayGenerator<std::vector<int> > >,
  MinimalPerfectHashTester<std::vector<int>, OrderedArrayGenerator<std::vector<int> > >,
  // Test with some complex types (heavy...)
  MinimalPerfectHashTester<std::vector<std::string>, RandomArrayGenerator<std::vector<std::string> > >,
  MinimalPerfectHashTester<std::vector<std::pair<int, std::string> >,
                           RandomArrayGenerator<std::vector<std::pair<int, std::string> > > >
> HashTesters;

template<typename T> class HashTest : public testing::Test {};
TYPED_TEST_CASE(HashTest, HashTesters);

TYPED_TEST(HashTest, small) {
  const int kNumTrial = 10;
  for (size_t s = 0; s <= 100; ++s) {
    for (int t = 0; t < kNumTrial; ++t) {
      TypeParam::Test(s);
    }
  }
}

TYPED_TEST(HashTest, large) {
  const int kNumTrial = 10;
  const int kMaxSize = 100000;
  for (int t = 0; t < kNumTrial; ++t) {
    size_t s = rand() % kMaxSize;
    TypeParam::Test(s);
  }
}

TYPED_TEST(HashTest, serialization) {
  const int kNumTrial = 10;
  const int kMaxSize = 100;
  for (int t = 0; t < kNumTrial; ++t) {
    size_t s = rand() % kMaxSize;
    TypeParam::TestSerialization(s);
  }
}

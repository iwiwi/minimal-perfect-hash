#include "minimal_perfect_hash.h"
#include <cassert>
#include <iostream>
#include <string>

int main() {
  // int
  {
    // Prepare the keys
    std::vector<int> t;
    t.push_back(1);
    t.push_back(11);
    t.push_back(111);
    t.push_back(1111);
    t.push_back(11111);
    t.push_back(111111);

    // Build
    minimal_perfect_hash::MinimalPerfectHash<int> mph;
    int r = mph.Build(t);
    assert(r == 0);

    // GetHash
    for (size_t i = 0; i < t.size(); ++i) {
      std::cout << t[i] << "\t" << mph.GetHash(t[i]) << std::endl;
    }
    std::cout << std::endl;
  }

  // string
  {
    // Prepare the keys
    std::vector<std::string> t;
    t.push_back("hoge");
    t.push_back("piyo");
    t.push_back("fuga");
    t.push_back("foo");
    t.push_back("bar");

    // Build
    minimal_perfect_hash::MinimalPerfectHash<std::string> mph;
    int r = mph.Build(t);
    assert(r == 0);

    // GetHash
    for (size_t i = 0; i < t.size(); ++i) {
      std::cout << t[i] << "\t" << mph.GetHash(t[i]) << std::endl;
    }
    std::cout << std::endl;
  }

  return 0;
}

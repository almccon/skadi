#include "test.h"
#include "skadi/sample.h"
using namespace valhalla;

#include <cmath>
#include <list>

namespace {

void load() {
  //load non existent datasource
  try {
    skadi::sample("blahblah");
    throw std::logic_error("Should be no datasource exception");
  }
  catch(const std::runtime_error& e) {
  }

  //check the no data value
  skadi::sample s("test/data/appalachian.vrt");
}

void no_data() {
  //check the no data value
  skadi::sample s("test/data/appalachian.vrt");
  if(s.get_no_data_value() != -32768)
    throw std::logic_error("No data value should be -32768");

  if(s.get(std::make_pair(0.0,0.0)) != s.get_no_data_value())
    throw std::logic_error("Asked for point with no data should be no data value");
}

void get() {
  //check a single point
  skadi::sample s("test/data/appalachian.vrt");
  if(std::fabs(489 - s.get(std::make_pair(-76.503915, 40.678783))) > 1.0)
    throw std::runtime_error("Wrong value at location");

  //check a bunch
  std::list<std::pair<double, double> > postings =  {
    {-76.537011, 40.723872},  // 300m
    {-76.537011, 40.726872},  // 350m
    {-76.537011, 40.729872},  // 450m
    {-76.537011, 40.732872},  // 325m
    {-76.537011, 40.735872}   // 250m
  };
  double riemann_sum = 0;
  auto heights = s.get_all(postings);
  for(const auto height : heights) {
    if(height == s.get_no_data_value())
      throw std::runtime_error("Should have heights for all of these points");
    riemann_sum += height;
  }
  if(std::fabs(riemann_sum - 1675) > 100)
    throw std::runtime_error("Area under discretized curve isn't right");
}

}

int main() {
  test::suite suite("sample");

  suite.test(TEST_CASE(load));

  suite.test(TEST_CASE(no_data));

  suite.test(TEST_CASE(get));

  return suite.tear_down();
}
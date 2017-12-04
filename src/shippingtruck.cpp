#include "truck.h"
#include <random>

using namespace std;

int main(void)
{
  ShippingTruck st1;
  
  std::default_random_engine rnd((unsigned int)std::chrono::system_clock::now().time_since_epoch().count());
  std::uniform_int_distribution<size_t> dist(0, 1);
  
  int dockIdx = dist(rnd);
  st1.goToDock(dockIdx);
  
  return 0;
}

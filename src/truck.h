#include "warehouse_common.h"

#include <cpen333/process/shared_memory.h>
#include <cpen333/process/mutex.h>

using namespace std;

class Truck
{
protected:
  // Properties
  cpen333::process::shared_object<SharedData> memory_;
  cpen333::process::mutex mutex_;
  
  // local copy of warehouse
  WarehouseInfo winfo_;
  TruckInfo tinfo_;
  
  vector<Item> items;
  int idx_;
  int x_;
  int y_;
  
public:
  // Constructor
  Truck() : memory_(MEMORY_NAME), mutex_(MUTEX_NAME),
  winfo_(), idx_(0), x_(0), y_(0) {
    // copy maze contents
    tinfo_ = memory_->tinfo;
    
    {
      // protect access of number of runners
      std::lock_guard<decltype(mutex_)> lock(mutex_);
      idx_ = memory_->tinfo.ntrucks;
      memory_->tinfo.ntrucks++;
      mutex_.unlock();
    }
    
    // get current location
    x_ = memory_->tinfo.tloc[idx_][COL_IDX];
    y_ = memory_->tinfo.tloc[idx_][ROW_IDX];
    
  }
  
  // Methods
  void notifyArrival();
  void notifyDeparture();
};

inline void notifyArrival()
{
  
}

inline void notifyDeparture()
{
  
}

class ReceivingTruck : public Truck
{
public:
  void unloadItems();
};

class ShippingTruck : public Truck
{
  int capacity;
  
public:
  int getCapacity();
  void loadItems(vector<Item> items);
};

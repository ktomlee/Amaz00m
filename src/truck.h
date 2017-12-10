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
  DockInfo dinfo_;
  
  vector<Item> items;
  int idx_;
  int x_;
  int y_;
  
public:
  // Constructor
  Truck() : memory_(WAREHOUSE_MEMORY_NAME), mutex_(MUTEX_NAME),
  winfo_(), idx_(0), x_(0), y_(0) {
    // copy maze contents
    tinfo_ = memory_->tinfo;
    dinfo_ = memory_->dinfo;
    
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
  void strLoc(int c, int r);
  void ldLoc();
  void notifyArrival();
  void notifyDeparture();
  void goToDock(int dIdx);
};

inline void Truck::strLoc(int c, int r)
{
  x_ = c;
  y_ = r;
  mutex_.lock();
  memory_->tinfo.tloc[idx_][COL_IDX] = x_;
  memory_->tinfo.tloc[idx_][ROW_IDX] = y_;
  mutex_.unlock();
}

inline void Truck::ldLoc()
{
  mutex_.lock();
  x_ = memory_->tinfo.tloc[idx_][COL_IDX];
  y_ = memory_->tinfo.tloc[idx_][ROW_IDX];
  mutex_.unlock();
}

inline void Truck::notifyArrival()
{
  
}

inline void Truck::notifyDeparture()
{
  
}

inline void Truck::goToDock(int dIdx)
{
  while(x_ > dinfo_.dloc[dIdx][COL_IDX])
  {
    x_--;
    strLoc(x_, y_);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
  
  while(y_ > dinfo_.dloc[dIdx][ROW_IDX])
  {
    y_--;
    strLoc(x_, y_);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
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

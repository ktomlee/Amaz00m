#include "warehouse_common.h"
#include "Central_computer.h"

#include <cpen333/process/condition_variable.h>
#include <cpen333/process/shared_memory.h>
#include <cpen333/process/mutex.h>

using namespace std;

class Truck : public cpen333::thread::thread_object
{
protected:
  // Properties
  Central_computer& cc_;
  
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
  Truck(Central_computer &cc) : cc_(cc), memory_(WAREHOUSE_MEMORY_NAME), mutex_(MUTEX_NAME),
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
    void goAway(void);
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

inline void Truck::goAway(void)
{
    for(int i=0; i<5; i++)
    {
        y_++;
        strLoc(x_, y_);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

class ReceivingTruck : public Truck
{
  int type = RECEIVING_TYPE;
public:
  ReceivingTruck(Central_computer &cc) : Truck(cc) {}
  
  void unloadItems();
    
  
  int main(void)
  {
      std::default_random_engine rnd((unsigned int)std::chrono::system_clock::now().time_since_epoch().count());
      std::uniform_int_distribution<size_t> dist(0, 10);
      
      std::vector<std::pair<Item, int>> contents;
      
      for(int i=0; i<CATALOGUE_SIZE; i++)
      {
          std::pair<Item, int> tmp;
          tmp.first = getItem(getItemName(i));
          tmp.second = dist(rnd);
          
          contents.push_back(tmp);
      }
      
      /*
      //tmp
      std::pair<Item, int> tmp;
      tmp.first = getItem(getItemName(0));
      tmp.second = 1;
      contents.push_back(tmp);
      //tmp
       */
      
      cpen333::process::semaphore sem_docking(DOCK_SEM_NAME);
      
      int dock = INVALID_DOCK;
      do
      {
          // wait until a dock is free
          sem_docking.wait();
          
          // ask central computer if we are needed right now
          dock = cc_.allowReceivingTruck(contents);
      } while(dock == INVALID_DOCK);
      
      goToDock(dock);
      cc_.setTruckPresent(dock);
      
      cpen333::process::mutex mutex(DOCK_MUTEX_NAME + std::to_string(dock));
      cpen333::process::condition_variable cv_dock(DOCK_CV_NAME + std::to_string(dock));
      
      cc_.unloadTruckToReceivingQ(contents, dock);
      
      {
          std::unique_lock<decltype(mutex)> lock(mutex);
          cv_dock.wait(lock);
      }
      
      goAway();
      
      cc_.freeDock(dock);
      
      sem_docking.notify();
    return 0;
  }
};



class ShippingTruck : public Truck
{
    int capacity = TRUCK_CAPACITY;
    int type = SHIPPING_TYPE;
    
public:
    ShippingTruck(Central_computer &cc) : Truck(cc) {}
    
    int getCapacity();
    
    int main(void)
    {
        std::default_random_engine rnd((unsigned int)std::chrono::system_clock::now().time_since_epoch().count());
        std::uniform_int_distribution<size_t> dist(0, 1);
        
        cpen333::process::semaphore sem_docking(DOCK_SEM_NAME);
        
        int dock = INVALID_DOCK;
        do
        {
            // wait until a dock is free
            sem_docking.wait();
            
            // ask central computer if we are needed right now
            dock = cc_.allowShippingTruck();
        } while(dock == INVALID_DOCK);
        
        goToDock(dock);
        cc_.setTruckPresent(dock);
        
        cpen333::process::mutex mutex(DOCK_MUTEX_NAME + std::to_string(dock));
        cpen333::process::condition_variable cv_dock(DOCK_CV_NAME + std::to_string(dock));
        
        {
            std::unique_lock<decltype(mutex)> lock(mutex);
            cv_dock.wait(lock);
        }
        
        goAway();
        
        cc_.freeDock(dock);
        
        sem_docking.notify();
        
        return 0;
    }
};

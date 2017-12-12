#include "warehouse_common.h"

#include <cpen333/process/shared_memory.h>
#include <cpen333/process/mutex.h>
#include <cpen333/thread/thread_object.h>
#include <cstring>
#include <chrono>
#include <thread>
#include "CircularQueue.h"
#include "ItemQueue.h"
#include "safe_printf.h"
#include "Central_computer.h"

cpen333::process::mutex mutex(MUTEX_NAME);

#define ROBOT_CAPACITY 10

class Robot : public cpen333::thread::thread_object {
    cpen333::process::shared_object<SharedData> memory_;
    cpen333::process::mutex mutex_;

    // local copy of warehouse
    WarehouseInfo winfo_;

    // robot info
    size_t idx_;   // robot index
    int x_;
    int y_;
    int destx_;
    int desty_;
    bool atDest_;
    int weight;
    CircularOrderQueue& ShippingQ_;
    ItemQueue& ReceivingQ_;
    Central_computer& cc_;
    int status;
    
    std::vector<Item> receivingBucket;

 public:
    
    Robot(int id, CircularOrderQueue& ShippingQ, ItemQueue& ReceivingQ, Central_computer& cc) : memory_(WAREHOUSE_MEMORY_NAME), mutex_(MUTEX_NAME),
    winfo_(), idx_(0), x_(0), y_(0), ShippingQ_(ShippingQ), ReceivingQ_(ReceivingQ), cc_(cc) {
        
        // copy maze contents
        winfo_ = memory_->winfo;
        
        {
            // protect access of number of runners
            std::lock_guard<decltype(mutex_)> lock(mutex_);
            idx_ = memory_->rinfo.nrobots;
            memory_->rinfo.nrobots++;
            mutex_.unlock();
        }
        
        // get current location
        x_ = memory_->rinfo.rloc[idx_][COL_IDX];
        y_ = memory_->rinfo.rloc[idx_][ROW_IDX];
        
    }
    
  
  void strLoc(int c, int r)
  {
    x_ = c;
    y_ = r;
    mutex_.lock();
    memory_->rinfo.rloc[idx_][COL_IDX] = x_;
    memory_->rinfo.rloc[idx_][ROW_IDX] = y_;
    mutex_.unlock();
  }
  
  void ldLoc()
  {
    mutex_.lock();
    x_ = memory_->rinfo.rloc[idx_][COL_IDX];
    y_ = memory_->rinfo.rloc[idx_][ROW_IDX];
    mutex_.unlock();
  }
  
  int goToDest() {
    /*ldLoc();
    destx_ = 13;
    desty_ = 1;
    return go(x_, y_);*/
    /*
     if(memory_->magic == 80085) {
     return go(loc_[COL_IDX], loc_[ROW_IDX]);
     }
     else {
     std::cout << "Shared memory not initialized" << std::endl;
     //std::cin.get();
     return 0;
     }
     */
      return 0;
  }
    
    
  /**
   * Solves the maze, taking time between each step so we can actually see progress in the UI
   * @return 1 for success, 0 for failure, -1 to quit
   */
  void go(int c, int r) {
      if(x_==c && y_==r) return;
      
    // If we're going to a dock, go down first then go laterally
      if(r == winfo_.rows)
      {
          // move right until not above shelves
          while(DOWN(x_, y_) != EMPTY_CHAR && y_ < winfo_.rows-1)
          {
              x_++;
              strLoc(x_, y_);
              std::this_thread::sleep_for(std::chrono::milliseconds(100));
          }
          
          // move to bottom wall
          while(y_ < winfo_.rows-1)
          {
              y_++;
              strLoc(x_, y_);
              std::this_thread::sleep_for(std::chrono::milliseconds(100));
          }
          
          // move left to dock if we are right of it
          while(x_ > c && LEFT(x_,y_) == EMPTY_CHAR)
          {
              x_--;
              strLoc(x_, y_);
              std::this_thread::sleep_for(std::chrono::milliseconds(100));
          }
          
          // move right to dock if we are left of it
          while(x_ < c)
          {
              x_++;
              strLoc(x_, y_);
              std::this_thread::sleep_for(std::chrono::milliseconds(100));
          }
      }
      
      else
      {
          // move right until not above shelves
          while(UP(x_, y_) != EMPTY_CHAR && y_ > 1)
          {
              x_++;
              strLoc(x_, y_);
              std::this_thread::sleep_for(std::chrono::milliseconds(100));
          }
          
          // move to top wall
          while(y_ > 1)
          {
              y_--;
              strLoc(x_, y_);
              std::this_thread::sleep_for(std::chrono::milliseconds(100));
          }
          
          // move right to goal if we are left of it
          while(x_ < c)
          {
              x_++;
              strLoc(x_, y_);
              std::this_thread::sleep_for(std::chrono::milliseconds(100));
          }
          
          // move left to goal if we are right of it
          while(x_ > c && LEFT(x_,y_) == EMPTY_CHAR)
          {
              x_--;
              strLoc(x_, y_);
              std::this_thread::sleep_for(std::chrono::milliseconds(100));
          }
          
          // move down to goal
          while(y_ < r)
          {
              y_++;
              strLoc(x_, y_);
              std::this_thread::sleep_for(std::chrono::milliseconds(100));
          }
      }
  }
    
    int getShippingDock()
    {
        return cc_.getValidDock(SHIPPING_TYPE);
    }
    
    int getReceivingDock()
    {
        return cc_.getValidDock(RECEIVING_TYPE);
    }
    
    void ship_random_orders()
    {
        cpen333::process::mutex whmutex(MUTEX_NAME);
        cpen333::process::shared_object<SharedData> whmemory(WAREHOUSE_MEMORY_NAME);
        
        status = RSTATUS_LOADING;
        whmutex.lock();
        whmemory->rinfo.rstatus[idx_] = status;
        whmutex.unlock();
        
        std::default_random_engine rnd((unsigned int)std::chrono::system_clock::now().time_since_epoch().count());
        std::uniform_int_distribution<size_t> dist(0, 10);
        
        int dock = getShippingDock();
        
        // There are no shipping trucks here!
        if(dock == INVALID_DOCK) return;
        
        Order order;
        
        for(int i=0; i<CATALOGUE_SIZE; i++)
        {
            Item tmp;
            order.items[i] = getItem(getItemName(i));
            order.quantity[i] = dist(rnd);
            order.nitems++;
        }
        
        for(int i=0; i<order.nitems; i++)
        {
            if(order.items[i].shelf.s == S_LEFT)
            {
                go(order.items[i].shelf.x-1, order.items[i].shelf.y);
            }
            else // if(dummy.items[0].shelf.s == S_RIGHT)
            {
                go(order.items[i].shelf.x+1, order.items[i].shelf.y);
            }
        }
        
        whmutex.lock();
        int dock_x = whmemory->dinfo.dloc[dock][COL_IDX];
        int dock_y = whmemory->dinfo.dloc[dock][ROW_IDX];
        whmutex.unlock();
        
        go(dock_x, dock_y-1);
        
        cc_.loadOrderOnTruck(order, dock);
    }
    
    void ship()
    {
        cpen333::process::mutex whmutex(MUTEX_NAME);
        cpen333::process::shared_object<SharedData> whmemory(WAREHOUSE_MEMORY_NAME);
        
        status = RSTATUS_LOADING;
        whmutex.lock();
        whmemory->rinfo.rstatus[idx_] = status;
        whmutex.unlock();
        
        int dock = getShippingDock();
        
        // There are no shipping trucks here!
        if(dock == INVALID_DOCK) return;
        
        Order order;
        
        //get order from shippingQ
        for(int i=0; i<order.nitems; i++)
        {
            if(order.items[i].shelf.s == S_LEFT)
            {
                go(order.items[i].shelf.x-1, order.items[i].shelf.y);
            }
            else // if(dummy.items[0].shelf.s == S_RIGHT)
            {
                go(order.items[i].shelf.x+1, order.items[i].shelf.y);
            }
        }
        
        // Go to a dock with a shipping truck

        
        whmutex.lock();
        int dock_x = whmemory->dinfo.dloc[dock][COL_IDX];
        int dock_y = whmemory->dinfo.dloc[dock][ROW_IDX];
        whmutex.unlock();
        
        go(dock_x, dock_y-1);
        
        cc_.loadOrderOnTruck(order, dock);
    }
    
    void receive()
    {
        cpen333::process::mutex whmutex(MUTEX_NAME);
        cpen333::process::shared_object<SharedData> whmemory(WAREHOUSE_MEMORY_NAME);
        
        status = RSTATUS_UNLOADING;
        whmutex.lock();
        whmemory->rinfo.rstatus[idx_] = status;
        whmutex.unlock();
        
        
        // No items to receive!
        if(ReceivingQ_.isEmpty()) return;
        
        weight = 0;
        receivingBucket.clear();
        int dock = INVALID_DOCK;
        
        // Collect items from receivingQ
        while(weight < ROBOT_CAPACITY)
        {
            std::pair<Item, int> tmp = (ReceivingQ_.get());
            dock = tmp.second;
            
            whmutex.lock();
            int dock_x = whmemory->dinfo.dloc[dock][COL_IDX];
            int dock_y = whmemory->dinfo.dloc[dock][ROW_IDX];
            whmutex.unlock();
            
            go(dock_x, dock_y-1);
            
            weight += tmp.first.weight;
            
            receivingBucket.push_back(tmp.first);
            cc_.unloadItemFromTruck(tmp.first, dock);
        }
        
        // put them on the shelves
        for(auto &item : receivingBucket)
        {
            if(item.shelf.s == S_LEFT)
            {
                go(item.shelf.x-1, item.shelf.y);
            }
            else // if(dummy.items[0].shelf.s == S_RIGHT)
            {
                go(item.shelf.x+1, item.shelf.y);
            }
            
            cc_.addToInventory(item);
        }
    }
    
    int getStatus()
    {
        return status;
    }
    
    int main() {
        while(true)
        {
            ship_random_orders();
            receive();
        }

        return 0;
    }

};



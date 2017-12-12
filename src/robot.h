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
    CircularOrderQueue& ShippingQ_;
    ItemQueue& ReceivingQ_;
    Central_computer& cc_;

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
          while(x_ > c)
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
          
          // move left to goal if we are right of it
          while(x_ > c)
          {
              x_--;
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
    
    int main() {
        /*
        CircularOrderQueue OQ;
        ItemQueue IQ;
        int id = 0;
        Robot robot(id, OQ, IQ);
        */
        cpen333::process::mutex whmutex(MUTEX_NAME);
        cpen333::process::shared_object<SharedData> whmemory(WAREHOUSE_MEMORY_NAME);
        
        Order dummy;
        dummy.items[0] = getItem("Broom");
        dummy.quantity[0] = 2;
        dummy.orderId = 1;
        dummy.nitems = 1;
        
        while(true) {
            //std::this_thread::sleep_for(std::chrono::seconds(10));
            
            //Order order = ShippingQ_.get();
            //std::cout << order.items[0].name << ": " << order.quantity[0] << std::endl;
            
            // Collect the items
            for(int i=0; i<dummy.nitems; i++)
            {
                if(dummy.items[0].shelf.s == S_LEFT)
                {
                    go(dummy.items[0].shelf.x-1, dummy.items[0].shelf.y);
                }
                else // if(dummy.items[0].shelf.s == S_RIGHT)
                {
                    go(dummy.items[0].shelf.x+1, dummy.items[0].shelf.y);
                }
            }
            
            // Go to a dock with a shipping truck
            int dock = INVALID_DOCK;
            do
            {
                dock = getShippingDock();
            } while(dock == INVALID_DOCK);
            
            whmutex.lock();
            int dock_x = whmemory->dinfo.dloc[dock][COL_IDX];
            int dock_y = whmemory->dinfo.dloc[dock][ROW_IDX];
            whmutex.unlock();
            
            go(dock_x, dock_y-1);
            
            cc_.loadOrderOnTruck(dummy, dock);
        }
        
        
        //robot.goToDest();

        return 0;
    }

};



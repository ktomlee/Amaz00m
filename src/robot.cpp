#include "warehouse_common.h"

#include <cpen333/process/shared_memory.h>
#include <cpen333/process/mutex.h>
#include <cstring>
#include <chrono>
#include <thread>

class Robot {

  cpen333::process::shared_object<SharedData> memory_;
  cpen333::process::mutex mutex_;

  // local copy of maze
  WarehouseInfo winfo_;

  // robot info
  size_t idx_;   // robot index
  int loc_[2];   // current location
  int x_;
  int y_;

 public:

  Robot() : memory_(MEMORY_NAME), mutex_(MUTEX_NAME),
                 winfo_(), idx_(0), loc_(), x_(0), y_(0) {

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
    int escape() {
    	return go(5, 5);
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
    }
    
    
  /**
   * Solves the maze, taking time between each step so we can actually see progress in the UI
   * @return 1 for success, 0 for failure, -1 to quit
   */
  int go(int col, int row) {
    
      mutex_.lock();
      x_ = memory_->rinfo.rloc[idx_][COL_IDX];
      y_ = memory_->rinfo.rloc[idx_][ROW_IDX];
      mutex_.unlock();
      
      while((x_ != col) || (y_ != row)) {
          
          if(x_ < col) {
              mutex_.lock();
              memory_->rinfo.rloc[idx_][COL_IDX] += 1;
              mutex_.unlock();
          }
          else if(x_ > col) {
              mutex_.lock();
              memory_->rinfo.rloc[idx_][COL_IDX] -= 1;
              mutex_.unlock();
          }
          
          else if(y_ < row) {
              mutex_.lock();
              memory_->rinfo.rloc[idx_][ROW_IDX] += 1;
              mutex_.unlock();
          }
          else if(y_ > row) {
              mutex_.lock();
              memory_->rinfo.rloc[idx_][ROW_IDX] -= 1;
              mutex_.unlock();
          }
          
          std::this_thread::sleep_for(std::chrono::milliseconds(100));
          mutex_.lock();
          x_ = memory_->rinfo.rloc[idx_][COL_IDX];
          y_ = memory_->rinfo.rloc[idx_][ROW_IDX];
          mutex_.unlock();
      }
      
          return 1;
  }

};

int main() {
    Robot robot;
    robot.escape();

  return 0;
}

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

  // runner info
  size_t idx_;   // runner index
  int loc_[2];   // current location

 public:

  Robot() : memory_(MEMORY_NAME), mutex_(MUTEX_NAME),
                 winfo_(), idx_(0), loc_() {

    // copy maze contents
    winfo_ = memory_->winfo;

    {
      // protect access of number of runners
      std::lock_guard<decltype(mutex_)> lock(mutex_);
      idx_ = memory_->rinfo.nrobots;
      memory_->rinfo.nrobots++;
    }

    // get current location
    loc_[COL_IDX] = memory_->rinfo.rloc[idx_][COL_IDX];
    loc_[ROW_IDX] = memory_->rinfo.rloc[idx_][ROW_IDX];

  }
    int escape() {
    	return go(2, 2);
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
          memory_->rinfo.rloc[idx_][COL_IDX] = col;
          memory_->rinfo.rloc[idx_][ROW_IDX] = row;
          mutex_.unlock();
      
          std::this_thread::sleep_for(std::chrono::milliseconds(100));
      
          return 1;
  }

};

int main() {
    Robot robot;
    robot.escape();

  return 0;
}

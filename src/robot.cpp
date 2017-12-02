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
  int x_;
  int y_;
  int destx_;
  int desty_;
  bool atDest_;

 public:

  Robot() : memory_(MEMORY_NAME), mutex_(MUTEX_NAME),
                 winfo_(), idx_(0), x_(0), y_(0) {

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
    ldLoc();
    destx_ = 13;
    desty_ = 1;
    return go(x_, y_);
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
  int go(int c, int r) {
    
    // Store current location
    strLoc(c, r);
    
    // If we found exit or have been told to quit, return early.
    // These class prperties will persist all the way up the
    // recursion tree and return the result back to escape().
    if(atDest_) return SUCCESS;
    if(memory_->quit) return QUIT;
    
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    int result;
    
    //==========================================================
    // NAVIGATE MAZE
    //==========================================================
    
    // Base cases begin
    
    // If we are at the exit we win! Return success.
    if(c==destx_ && r==desty_)
    {
      atDest_ = true;
      return SUCCESS;
    }
    
    // If we are adjacent to the exit, move to it.
    if     (c==destx_ && (r-1)==desty_) result = go(c,r-1);
    else if(c==destx_ && (r+1)==desty_) result = go(c,r+1);
    else if((c-1)==destx_ && r==desty_) result = go(c-1,r);
    else if((c+1)==destx_ && r==desty_) result = go(c+1,r);
    
    // If we are at a dead end, return failure.
    if(UP(c,r) == 'X' && DOWN(c,r) == 'X' &&
       LEFT(c,r) == 'X' && RIGHT(c,r) == 'X')
    {
      return FAILURE;
    }
    
    // Base cases end
    
    
    // Mark current location as visited in local copy of maze info
    winfo_.warehouse[c][r] = 'X';
    
    // If there are any empty spaces, explore them.
    // After returning from those trees, set position back to loc we had before call.
    if(UP(c,r) == ' ')
    {
      result = go(c,r-1);
      if(result == SUCCESS) return SUCCESS;
      strLoc(c, r);
    }
    if(LEFT(c,r) == ' ')
    {
      result = go(c-1,r);
      if(result == SUCCESS) return SUCCESS;
      strLoc(c, r);
    }
    if(DOWN(c,r) == ' ')
    {
      result = go(c,r+1);
      if(result == SUCCESS) return SUCCESS;
      strLoc(c, r);
    }
    if(RIGHT(c,r) == ' ')
    {
      result = go(c+1,r);
      if(result == SUCCESS) return SUCCESS;
      strLoc(c, r);
    }
    
    // Mark current location back to unvisited.
    winfo_.warehouse[c][r] = ' ';
    
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    // If we end up here, result should be 0 (dead end).
    return result;
  }


};

int main() {
    Robot robot;
    robot.goToDest();

  return 0;
}

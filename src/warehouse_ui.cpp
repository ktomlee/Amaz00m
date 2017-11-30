#include "warehouse_common.h"

#include <cpen333/process/shared_memory.h>
#include <cpen333/process/mutex.h>
#include <cpen333/console.h>
#include <cstdio>
#include <thread>
#include <chrono>

class MazeUI {
  // display offset for better visibility
  static const int XOFF = 2;
  static const int YOFF = 1;

  cpen333::console display_;
  cpen333::process::shared_object<SharedData> memory_;
  cpen333::process::mutex mutex_;

  // previous positions of runners
  int lastpos_[MAX_ROBOTS][2];
  int exit_[2];   // exit location

 public:

  MazeUI() : display_(), memory_(MEMORY_NAME), mutex_(MUTEX_NAME){

    // clear display and hide cursor
    display_.clear_all();
    display_.set_cursor_visible(false);

    // initialize last known runner positions
    for (size_t i=0; i<MAX_ROBOTS; ++i) {
      lastpos_[i][COL_IDX] = -1;
      lastpos_[i][ROW_IDX] = -1;
    }

    // initialize exit location
    //exit_[COL_IDX] = -1;
    //exit_[ROW_IDX] = -1;

    //===========================================================
    // TODO: SEARCH MAZE FOR EXIT LOCATION
    //===========================================================

  }

  /**
   * Draws the maze itself
   */
  void draw_maze() {
      
    static const char WALL = 'X';  // WALL character, or change to 'X' if trouble printing
    static const char DOCK = 'D';  // EXIT character, or change to 'E' if trouble printing
    static const char SHELF = 'S';

    WarehouseInfo& winfo = memory_->winfo;
    RobotInfo& rinfo = memory_->rinfo;

    // clear display
    display_.clear_display();

    // draw maze
    for (int r = 0; r < winfo.rows; ++r) {
      display_.set_cursor_position(YOFF+r, XOFF);
      for (int c = 0; c < winfo.cols; ++c) {
        char ch = winfo.warehouse[c][r];
        if (ch == WALL_CHAR) {
          std::printf("%c", WALL);
        } else if (ch == SHELF_CHAR){
          std::printf("%c", SHELF);
        } else if (ch == DOCK_CHAR){
          std::printf("%c", DOCK);
        } else {
          std::printf("%c", EMPTY_CHAR);
        }
      }
    }
    fflush(stdout);  // force output buffer to flush 
  }

  /**
   * Draws all runners in the maze
   */
  void draw_robots() {

    RobotInfo& rinfo = memory_->rinfo;

    // draw all runner locations
    for (size_t i=0; i<rinfo.nrobots; ++i) {
      char me = 'A'+i;
      int newr = rinfo.rloc[i][ROW_IDX];
      int newc = rinfo.rloc[i][COL_IDX];

      // if not already at the exit...
      if (newc != exit_[COL_IDX] || newr != exit_[ROW_IDX]) {
        if (newc != lastpos_[i][COL_IDX]
            || newr != lastpos_[i][ROW_IDX]) {

          // zero out last spot and update known location
          display_.set_cursor_position(YOFF+lastpos_[i][ROW_IDX], XOFF+lastpos_[i][COL_IDX]);
          std::printf("%c", EMPTY_CHAR);
          lastpos_[i][COL_IDX] = newc;
          lastpos_[i][ROW_IDX] = newr;
        }

        // print runner at new location
        display_.set_cursor_position(YOFF+newr, XOFF+newc);
        std::printf("%c", me);
      } else {

        // erase old position if now finished
          
    /*
        if (lastpos_[i][COL_IDX] != exit_[COL_IDX] || lastpos_[i][ROW_IDX] != exit_[ROW_IDX]) {
          display_.set_cursor_position(YOFF+lastpos_[i][ROW_IDX], XOFF+lastpos_[i][COL_IDX]);
          std::printf("%c", EMPTY_CHAR);
          lastpos_[i][COL_IDX] = newc;
          lastpos_[i][ROW_IDX] = newr;

          // display a completion message
          display_.set_cursor_position(YOFF, XOFF+memory_->winfo.cols+2);
          std::printf("runner %c escaped!!", me);
        }
     */
      }
    }
    fflush(stdout);  // force output buffer to flush
    
  }

  /**
   * Checks if we are supposed to quit
   * @return true if memory tells us to quit
   */
  bool quit() {
    // check if we need to quit
    return memory_->quit;
  }

  ~MazeUI(){
    // reset console settings
    display_.clear_all();
    display_.reset();
  }
};

int main() {
    cpen333::process::shared_object<SharedData> memory(MEMORY_NAME);
  // initialize previous locations of characters

  MazeUI ui;
  ui.draw_maze();

  // continue looping until main program has quit
  
  while(!ui.quit()) {
    ui.draw_robots();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
  

  return 0;
}

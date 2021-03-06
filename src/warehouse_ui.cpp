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
  static const int TOFF = 0;
  static const int ROFF = MAX_TRUCKS;

  cpen333::console display_;
  cpen333::process::shared_object<SharedData> memory_;
  cpen333::process::mutex mutex_;

  // previous positions of robota
  int lastpos_[MAX_ROBOTS + MAX_TRUCKS][2];
  int exit_[2];   // exit location

 public:

  MazeUI() : display_(), memory_(WAREHOUSE_MEMORY_NAME), mutex_(MUTEX_NAME){

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
    DockInfo& dinfo = memory_->dinfo;
    TruckInfo& tinfo = memory_->tinfo;

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
          //dinfo.ndocks+=1;
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
    TruckInfo& tinfo = memory_->tinfo;
    
    // draw all runner locations
    for (size_t i=0; i<tinfo.ntrucks; ++i) {
      char me = 'A'+i;
      int newr = tinfo.tloc[i][ROW_IDX];
      int newc = tinfo.tloc[i][COL_IDX];
      
      // if not already at the exit...
      //if (newc != exit_[COL_IDX] || newr != exit_[ROW_IDX]) {
        //if (newc != lastpos_[i][COL_IDX]
          //  || newr != lastpos_[i][ROW_IDX]) {
          
          // zero out last spot and update known location
          display_.set_cursor_position(YOFF+lastpos_[TOFF+i][ROW_IDX], XOFF+lastpos_[TOFF+i][COL_IDX]);
          std::printf("%c", EMPTY_CHAR);
          lastpos_[TOFF+i][COL_IDX] = newc;
          lastpos_[TOFF+i][ROW_IDX] = newr;
       // }
        
        // print runner at new location
        display_.set_cursor_position(YOFF+newr, XOFF+newc);
        std::printf("%c", me);
      //}
    }
    
    
    RobotInfo& rinfo = memory_->rinfo;

    // draw all runner locations
    for (size_t i=0; i<rinfo.nrobots; ++i) {
      char me = 'A'+i;
      int newr = rinfo.rloc[i][ROW_IDX];
      int newc = rinfo.rloc[i][COL_IDX];

      // if not already at the exit...
      //if (newc != exit_[COL_IDX] || newr != exit_[ROW_IDX]) {
        //if (newc != lastpos_[i][COL_IDX]
          //  || newr != lastpos_[i][ROW_IDX]) {

          // zero out last spot and update known location
          display_.set_cursor_position(YOFF+lastpos_[ROFF+i][ROW_IDX], XOFF+lastpos_[ROFF+i][COL_IDX]);
          std::printf("%c", EMPTY_CHAR);
          lastpos_[ROFF+i][COL_IDX] = newc;
          lastpos_[ROFF+i][ROW_IDX] = newr;
       // }

        // print runner at new location
        display_.set_cursor_position(YOFF+newr, XOFF+newc);
        std::printf("%c", me);
     // }
    }
   
    
    fflush(stdout);  // force output buffer to flush
  }
  
  /**
   * Draws all runners in the maze
   */
  void draw_trucks() {
    
    TruckInfo& tinfo = memory_->tinfo;
    
    // draw all runner locations
    for (size_t i=0; i<tinfo.ntrucks; ++i) {
      char me = 'A'+i;
      int newr = tinfo.tloc[i][ROW_IDX];
      int newc = tinfo.tloc[i][COL_IDX];
      
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
    cpen333::process::shared_object<SharedData> memory(WAREHOUSE_MEMORY_NAME);
  // initialize previous locations of characters

  MazeUI ui;
  ui.draw_maze();

  // continue looping until main program has quit
  
  while(!ui.quit()) {
    ui.draw_robots();
    //ui.draw_trucks();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
  

  return 0;
}

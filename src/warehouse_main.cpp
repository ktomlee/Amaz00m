#include "warehouse_common.h"

#include <string>
#include <fstream>
#include <thread>
#include <random>
#include <cpen333/process/shared_memory.h>
#include "CircularQueue.h"
#include "ItemQueue.h"
#include "robot.h"


/**
 * Reads a maze from a filename and populates the maze
 * @param filename file to load maze from
 * @param minfo maze info to populate
 */
void load_warehouse(const std::string& filename, WarehouseInfo& winfo) {

  // initialize number of rows and columns
  winfo.rows = 0;
  winfo.cols = 0;

  std::ifstream fin(filename);
  std::string line;

  // read maze file
  if (fin.is_open()) {
    int row = 0;  // zeroeth row
    while (std::getline(fin, line)) {
      int cols = line.length();
      if (cols > 0) {
        // longest row defines columns
        if (cols > winfo.cols) {
          winfo.cols = cols;
        }
        for (size_t col=0; col<cols; ++col) {
          winfo.warehouse[col][row] = line[col];
        }
        ++row;
      }
    }
    winfo.rows = row;
    fin.close();
  }

}

/**
 * Randomly places all possible maze runners on an empty
 * square in the maze
 * @param minfo maze input
 * @param rinfo runner info to populate
 */
void init_robots(const WarehouseInfo& winfo, RobotInfo& rinfo) {
  rinfo.nrobots = 0;

  // fill in random placements for future runners
  std::default_random_engine rnd(
      (unsigned int)std::chrono::system_clock::now().time_since_epoch().count());
  std::uniform_int_distribution<size_t> rdist(0, winfo.rows);
  std::uniform_int_distribution<size_t> cdist(0, winfo.cols);
  for (size_t i=0; i<MAX_ROBOTS; i++) {
    // generate until on an empty space
    size_t r,c;
    do {
      r = rdist(rnd);
      c = cdist(rnd);
    } while (winfo.warehouse[c][r] != EMPTY_CHAR);
    rinfo.rloc[i][COL_IDX] = c;
    rinfo.rloc[i][ROW_IDX] = r;
  }
}

/**
 * Randomly places all possible trucks on an empty
 * square outside the warehouse
 * @param minfo maze input
 * @param tinfo truck info to populate
 */
void init_trucks(const WarehouseInfo& winfo, TruckInfo& tinfo) {
  tinfo.ntrucks = 0;
  
  // fill in random placements for future runners
  std::default_random_engine rnd(
                                 (unsigned int)std::chrono::system_clock::now().time_since_epoch().count());
  std::uniform_int_distribution<size_t> rdist(winfo.rows, winfo.rows+10);
  std::uniform_int_distribution<size_t> cdist(winfo.cols, winfo.cols+10);
  for (size_t i=0; i<MAX_TRUCKS; i++) {
    // generate until on an empty space
    size_t r,c;
    
    r = rdist(rnd);
    c = cdist(rnd);
    tinfo.tloc[i][COL_IDX] = c;
    tinfo.tloc[i][ROW_IDX] = r;
  }
}

/**
 * Randomly places all possible maze runners on an empty
 * square in the maze
 * @param minfo maze input
 * @param rinfo runner info to populate
 */
void init_docks(const WarehouseInfo& winfo, DockInfo& dinfo) {
  dinfo.ndocks = 0;
  
  for (size_t c=0; c<winfo.cols;c++) {
    for (size_t r=0; r<winfo.rows; r++) {
      if(winfo.warehouse[c][r] == DOCK_CHAR)
      {
        dinfo.dloc[dinfo.ndocks][COL_IDX] = c;
        dinfo.dloc[dinfo.ndocks][ROW_IDX] = r;
        dinfo.ndocks++;
      }
    }
  }
}

int main(int argc, char* argv[]) {

  // read maze from command-line, default to maze0
    std::string warehouse = "data/warehouse.txt";
    if (argc > 1) {
        warehouse = argv[1];
    }
  
    cpen333::process::shared_object<SharedData> memory(WAREHOUSE_MEMORY_NAME);
  
    WarehouseInfo winfo;
    RobotInfo rinfo;
    DockInfo dinfo;
    TruckInfo tinfo;
  
    load_warehouse(warehouse, winfo);
    init_robots(winfo, rinfo);
    init_trucks(winfo, tinfo);
    init_docks(winfo, dinfo);
  
    memory->winfo = winfo;
    memory->rinfo = rinfo;
    memory->dinfo = dinfo;
    memory->tinfo = tinfo;
    memory->quit = false;
  
    memory->newOrderIdx_start = memory->newOrderIdx_end = 0;
    memory->nOrders = 0;
    
    CircularOrderQueue OQ;
    ItemQueue IQ;
    
    static int nrobots = 1;
    
    std::vector<Robot*> robots;
    
    for(int i = 0; i<nrobots; i++) {
        robots.push_back(new Robot(i, OQ, IQ));
    }
    
    /*
    
    std::vector<std::string> robot_args;
    robot_args.push_back("./robot");
    
    const int nRobots = 1;
    for (int i=0; i<nRobots; ++i) {
        cpen333::process::subprocess robot(robot_args, true, false);
    }
    */
    
    
    
    std::cout << "Keep this running until you are done with the program." << std::endl << std::endl;
    std::cout << "Press ENTER to quit." << std::endl;
    std::cin.get();
  
    memory->quit = true;
  
  
  return 0;
}

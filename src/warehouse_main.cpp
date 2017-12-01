#include "warehouse_common.h"

#include <string>
#include <fstream>
#include <thread>
#include <random>
#include <cpen333/process/shared_memory.h>

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

int main(int argc, char* argv[]) {

  // read maze from command-line, default to maze0
  std::string warehouse = "data/warehouse.txt";
  if (argc > 1) {
    warehouse = argv[1];
  }
  
  cpen333::process::shared_object<SharedData> memory(MEMORY_NAME);
  
  WarehouseInfo winfo;
  RobotInfo rinfo;
  
  load_warehouse(warehouse, winfo);
  init_robots(winfo, rinfo);
  
  memory->winfo = winfo;
  memory->rinfo = rinfo;
  memory->quit = false;

  std::cout << "Keep this running until you are done with the program." << std::endl << std::endl;
  std::cout << "Press ENTER to quit." << std::endl;
  std::cin.get();
  
  memory->quit = true;
  
  
  return 0;
}

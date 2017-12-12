#include "warehouse_common.h"

#include <string>
#include <fstream>
#include <thread>
#include <random>
#include <cpen333/process/shared_memory.h>
#include "CircularQueue.h"
#include "ItemQueue.h"
#include "robot.h"
#include "Central_computer.h"
#include "truck.h"

static const char ORDER_STATUS = '1';
static const char INVENTORY_STATUS = '2';
static const char CHECK_LOW = '3';
static const char CLIENT_QUIT = '4';

void print_menu() {
    
    std::cout << "=========================================" << std::endl;
    std::cout << "=                  MENU                 =" << std::endl;
    std::cout << "=========================================" << std::endl;
    std::cout << " (1) Check Order Status" << std::endl;
    std::cout << " (2) Check Inventory" << std::endl;
    std::cout << " (3) Check Items with Low Stock" << std::endl;
    std::cout << " (4) Quit"  << std::endl;
    std::cout << "=========================================" << std::endl;
    std::cout << "Enter number: ";
    std::cout.flush();
    
}


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
                dinfo.truck_type[dinfo.ndocks] = INVALID_TYPE;
                dinfo.truck_present[dinfo.ndocks] = false;
                dinfo.ndocks++;
            }
        }
    }
}

void getOrderStatus() {
    
    cpen333::process::mutex whmutex(MUTEX_NAME);
    cpen333::process::shared_object<SharedData> whmemory(WAREHOUSE_MEMORY_NAME);
    
    bool found = false;
    
    std::string orderId = "";
    int intID;
    std::cout << "Checking Order Status" << std::endl;
    std::cout << "Enter Order ID: ";
    std::getline(std::cin, orderId);
    intID = std::stoi(orderId);
    
    whmutex.lock();
    
    for(int i=whmemory->newOrderIdx_start; i<=whmemory->newOrderIdx_end; i++)
    {
        if(whmemory->newOrderQ[i].orderId == intID)
        {
            found = true;
            whmutex.unlock();
            Order order = whmemory->newOrderQ[i];
            std::cout << "Order Found: " << std::endl;
            std::cout << "Order Status: " << order.status << std::endl;
            
            for(int j=0; j<order.nitems; j++)
            {
                std::cout << "Item " << j + 1 << ": " << order.items[j].name << "    Quantity: " << order.quantity[j] << std::endl;
            }
            
        }
    }
    
    if(!found) {
        std::cout << "Order Not Found" << std::endl;
        whmutex.unlock();
    }
    
    
    
}

void getInventoryStatus(Central_computer cc) {
    
}

void getLowStock(Central_computer cc) {
    
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
    
    for(int i = 0; i < ORDERQ_SIZE; i++) {
        memory->newOrderQ[i].items[i].name = "";
    }
    
    CircularOrderQueue OQ;
    ItemQueue IQ;
    
    cpen333::process::semaphore sem_docking(DOCK_SEM_NAME);
    
    for(int i=0; i<dinfo.ndocks; i++)
    {
        sem_docking.notify();
    }
    
    static int nrobots = 1;
    static int nstrucks = 10;
    static int nrtrucks = 10;
    
    std::vector<Robot*> robots;
    std::vector<ShippingTruck*> strucks;
    std::vector<ReceivingTruck*> rtrucks;
    
    for(int i = 0; i<nrobots; i++) {
        robots.push_back(new Robot(i, OQ, IQ));
    }
  
    Central_computer cc(OQ, IQ);
    cc.start();

  for(int i = 0; i<nrtrucks; i++) {
    rtrucks.push_back(new ReceivingTruck(cc));
  }
  
  for(int i = 0; i<nstrucks; i++) {
    strucks.push_back(new ShippingTruck(cc));
  }
    
    for (auto& robot : robots) {
        robot->start();
    }
  
  for (auto& rtruck : rtrucks)
  {
    rtruck->start();
  }
  
  for (auto& struck : strucks)
  {
    struck->start();
  }
    
    char cmd = 0;
    while (cmd != CLIENT_QUIT) {
        print_menu();
        
        // get menu entry
        std::cin >> cmd;
        // gobble newline
        std::cin.ignore (std::numeric_limits<std::streamsize>::max(), '\n');
        
        switch(cmd) {
            case ORDER_STATUS:
                getOrderStatus();
                break;
            case INVENTORY_STATUS:
                //getInventoryStatus(cc);
                cc.reportInventory();
                break;
            case CHECK_LOW:
                //getLowStock(cc);
                cc.lowStock();
                break;
            case CLIENT_QUIT:
                std::cout << "Goodbye." << std::endl;
                memory->quit = true;
                return 0;
                break;
            default:
                std::cout << "Invalid command number " << cmd << std::endl << std::endl;
        }
        
        cpen333::pause();
    }
    
    /*
    std::cout << "Keep this running until you are done with the program." << std::endl << std::endl;
    std::cout << "Press ENTER to quit." << std::endl;
    std::cin.get();
     */
  
    //memory->quit = true;
  
    // kill cc, robots, strucks, rtrucks
    /*cc.join();
    for (auto& r : robots) {
        r->join();
    }
    for (auto& st : strucks) {
        st->join();
    }
    for (auto& rt : rtrucks) {
        rt->join();
    }
  */
  return 0;
}

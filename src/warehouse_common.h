#ifndef WAREHOUSE_COMMON_H
#define WAREHOUSE_COMMON_H

#include <string>
#include <map>
#include <vector>

#include <cpen333/process/mutex.h>
#include <cpen333/process/shared_memory.h>
#include <cpen333/process/subprocess.h>


#define WAREHOUSE_MEMORY_NAME "warehouse_memory"
#define MUTEX_NAME "warehouse_mutex"

#define WALL_CHAR 'X'
#define EMPTY_CHAR ' '
#define DOCK_CHAR 'D'
#define SHELF_CHAR 'S'

#define COL_IDX 0
#define ROW_IDX 1
#define SIDE_IDX 2
#define HEIGHT_IDX 3

#define UP(c,r)    (r > 0             ? winfo_.warehouse[c][r-1] : 'X')
#define DOWN(c,r)  (r < winfo_.rows-1 ? winfo_.warehouse[c][r+1] : 'X')
#define LEFT(c,r)  (c > 0             ? winfo_.warehouse[c-1][r] : 'X')
#define RIGHT(c,r) (c < winfo_.cols-1 ? winfo_.warehouse[c+1][r] : 'X')

#define SUCCESS 1
#define FAILURE 0
#define QUIT -1

#define MAX_ROBOTS 10
#define MAX_WAREHOUSE_SIZE 20
#define MAX_DOCKS 5
#define INVALID_DOCK MAX_DOCKS
#define MAX_TRUCKS 50

#define ORDERQ_SIZE 256
#define SHIPPINGQ_SIZE 256
#define RECEIVINGQ_SIZE 256
#define CATALOGUE_SIZE 4
#define MAX_ITEM_QUANTITY 100

#define SHELF_CAPACITY 1000

#define SHIPPING_TYPE 0
#define RECEIVING_TYPE 1
#define INVALID_TYPE 2

#define DOCK_MUTEX_NAME "dock_mutex"
#define DOCK_SEM_NAME "dock_semaphore"

#define  DOCK_CV_NAME "dock_cv"

#define S_LEFT 0
#define S_RIGHT 1

#define TRUCK_CAPACITY 100
#define TRUCK_THRESHOLD 50

bool validItem(std::string name) {
    bool valid = false;
    if(name == "Broom") {
        valid = true;
    }
    else if(name == "Cup") {
        valid = true;
    }
    else if(name == "Banana") {
        valid = true;
    }
    else if(name == "Hat") {
        valid = true;
    }
    else {
        valid = false;
    }
    return valid;
}

std::string getItemName(int id) {
    switch(id) {
        case 0: {
            return "Broom";
        }
        case 1: {
            return "Cup";
        }
        case 2: {
            return "Banana";
        }
        case 3: {
            return "Hat";
        }
    }
    return "Not Found";
}




struct WarehouseInfo {
  int rows;           // rows in warehouse
  int cols;           // columns in warehouse
  char warehouse[MAX_WAREHOUSE_SIZE][MAX_WAREHOUSE_SIZE];  // warehouse storage
};

struct RobotInfo {
  int nrobots;      // number of robots
  int rloc[MAX_ROBOTS][2];   // robot locations [col][row]
};

struct TruckInfo {
  int ntrucks;      // number of robots
  int tloc[MAX_TRUCKS][2];   // robot locations [col][row]
};

struct Shelf {
    int capacity;
    int x; // Ex. (4, 3, Right, 6);
    int y;
    int s;
    int h;
};

struct Item {
	int itemId;
	std::string name;
	int weight;
	Shelf shelf;
};

struct Catalogue {
    std::map<int, std::string> catalogue[CATALOGUE_SIZE]; // Item ID to item name
};

#define ORDER_SIZE 256
struct Order {
    int orderId;
    int weight;
    //std::vector<std::pair<Item, int>> items;
    // replace vector above with fixed size arrays below:
    Item items[ORDER_SIZE];
    int quantity[ORDER_SIZE];
    //int items[CATALOGUE_SIZE][MAX_ITEM_QUANTITY];
    int nitems;
    std::string status = "";
    
};

struct DockInfo {
    int dloc[MAX_DOCKS][2];
    int truck_type[MAX_DOCKS];
    bool truck_present[MAX_DOCKS];
    int ndocks;
};

struct SharedData {
	RobotInfo rinfo;
	WarehouseInfo winfo;
    DockInfo dinfo;
    TruckInfo tinfo;
	bool quit;
  
    int nOrders;
  
	Order newOrderQ[ORDERQ_SIZE];
    int newOrderIdx_start;
    int newOrderIdx_end;
    int consumerIdx_start;
    int consumerIdx_end;
  
	Order shippingQ[SHIPPINGQ_SIZE];
	Item receivingQ[RECEIVINGQ_SIZE];
    int itemloc[CATALOGUE_SIZE][4];
};

Item getItem(std::string name)
{
  Item out;
  out.name = name;
  
  cpen333::process::mutex whmutex(MUTEX_NAME);
  cpen333::process::shared_object<SharedData> whmemory(WAREHOUSE_MEMORY_NAME);
  
  if(name == "Broom") {
    out.weight = 5;
    out.itemId = 0;
  }
  else if(name == "Cup") {
    out.weight = 1;
    out.itemId = 1;
  }
  else if(name == "Banana") {
    out.weight = 3;
    out.itemId = 2;
  }
  else if(name == "Hat") {
    out.weight = 10;
    out.itemId = 3;
  }
  
  whmutex.lock();
  int x = whmemory->itemloc[out.itemId][COL_IDX];
  int y = whmemory->itemloc[out.itemId][ROW_IDX];
  int s = whmemory->itemloc[out.itemId][SIDE_IDX];
  int h = whmemory->itemloc[out.itemId][HEIGHT_IDX];
  whmutex.unlock();
  
  std::string side = (s==0) ? "Left" : "Right";
  
    out.shelf.x = x;
    out.shelf.y = y;
    out.shelf.s = s;
    out.shelf.h = h;
  out.shelf.capacity = SHELF_CAPACITY;
  
  return out;
}


#endif

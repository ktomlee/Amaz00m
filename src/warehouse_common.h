#ifndef WAREHOUSE_COMMON_H
#define WAREHOUSE_COMMON_H

#include <string>
#include <map>
#include <vector>

#define WAREHOUSE_MEMORY_NAME "warehouse_memory"
#define MUTEX_NAME "warehouse_mutex"

#define WALL_CHAR 'X'
#define EMPTY_CHAR ' '
#define DOCK_CHAR 'D'
#define SHELF_CHAR 'S'

#define COL_IDX 0
#define ROW_IDX 1

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
#define MAX_TRUCKS 50

#define ORDERQ_SIZE 256
#define SHIPPINGQ_SIZE 256
#define RECEIVINGQ_SIZE 256
#define CATALOGUE_SIZE 256
#define MAX_ITEM_QUANTITY 100

std::string getItemName(int id) {
    switch(id) {
        case 001: {
            return "broom";
        }
        case 002: {
            return "cup";
        }
        case 003: {
            return "banana";
        }
        case 004: {
            return "hat";
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
    std::vector<std::string> location; // Ex. (A, 3, Right, 6);
};

struct Item {
	int itemId;
	std::string name;
	int weight;
	Shelf shelf;
};

Item getItem(std::string name)
{
  Item out;
  out.name = name;
  
  if(name == "Broom") {
    out.weight = 5;
    out.itemId = 001;
    //out.shelf
  }
  else if(name == "Cup") {
    out.weight = 1;
    out.itemId = 002;
  }
  else if(name == "Banana") {
    out.weight = 3;
    out.itemId = 003;
  }
  else if(name == "Hat") {
    out.weight = 10;
    out.itemId = 004;
  }
  
  return out;
}

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
};

struct DockInfo {
    int dloc[MAX_DOCKS][2];
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
  
	Order shippingQ[SHIPPINGQ_SIZE];
	Item receivingQ[RECEIVINGQ_SIZE];
    int inventory[CATALOGUE_SIZE][MAX_ITEM_QUANTITY];
	//std::map<std::string, int> inventory;
};



#endif

#ifndef WAREHOUSE_COMMON_H
#define WAREHOUSE_COMMON_H

#include <string>
#include <map>
#include <vector>

#define MEMORY_NAME "warehouse_memory"
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

using namespace std;

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

struct Order {
  int orderId;
  int weight;
  std::vector<pair<Item, int>> items;
};

struct DockInfo {
    int dloc[MAX_DOCKS][2];
    int ndocks;
};

#define ORDERQ_SIZE 256
#define SHIPPINGQ_SIZE 256
#define RECEIVINGQ_SIZE 256

struct SharedData {
	RobotInfo rinfo;
	WarehouseInfo winfo;
  DockInfo dinfo;
  TruckInfo tinfo;
  
	bool quit;
	Order newOrderQ[ORDERQ_SIZE];
	Order shippingQ[SHIPPINGQ_SIZE];
	Item receivingQ[RECEIVINGQ_SIZE];
	//std::map<std::string, int> inventory;
};



#endif

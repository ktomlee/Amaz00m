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

#define MAX_ROBOTS 4
#define MAX_WAREHOUSE_SIZE 50

struct WarehouseInfo {
  int rows;           // rows in warehouse
  int cols;           // columns in warehouse
  char warehouse[MAX_WAREHOUSE_SIZE][MAX_WAREHOUSE_SIZE];  // warehouse storage
};

struct RobotInfo {
  int nrobots;      // number of robots
  int rloc[MAX_ROBOTS][2];   // robot locations [col][row]
};

struct Order {
	int orderId;
	int weight;
    std::map<std::string, int> order;
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

struct SharedData {
	RobotInfo rinfo;
	WarehouseInfo winfo;
	bool quit;
	std::vector<Order> newOrderQ;
	std::vector<Order> shippingQ;
	std::vector<Item> receivingQ;
	std::map<std::string, int> inventory;
};



#endif

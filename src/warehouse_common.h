#ifndef WAREHOUSE_COMMON_H
#define WAREHOUSE_COMMON_H

using namespace std;

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
  char maze[MAX_WAREHOUSE_SIZE][MAX_WAREHOUSE_SIZE];  // warehouse storage
};

struct RobotInfo {
  int nrobots;      // number of robots
  int rloc[MAX_ROBOTS][2];   // robot locations [col][row]
};

struct Order {
	int orderId;
	int weight;
	map<string, int> order;
};

struct Item {
	int itemId;
	string name;
	int weight;
	Shelf shelf;
};

struct Shelf {
	int capacity;
	vector<string> location; // Ex. (A, 3, Right, 6);
};

struct SharedData {
	Robotinfo rinfo;
	WarehouseInfo winfo;
	vector<Order> newOrderQ;
	vector<Order> shippingQ;
	vector<Item> receivingQ;
	map<string, int> inventory;
};



#endif
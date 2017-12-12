#ifndef CENTRALCOMPUTER_H
#define CENTRALCOMPUTER_H

#include <iostream>
#include <random>

#include "warehouse_common.h"

#include <cpen333/process/shared_memory.h>
#include <cpen333/process/mutex.h>
#include "CircularQueue.h"
#include "ItemQueue.h"

/**
 * Central computer class
 */
class Central_computer {
    std::map<std::string, int> inventory;
    CircularOrderQueue& ShippingQ_;
    ItemQueue& ReceivingQ_;


 public:
  /**
   * Creates a queue with provided circular buffer size
   * @param buffsize size of circular buffer
   */
    Central_computer(CircularOrderQueue& ShippingQ, ItemQueue& ReceivingQ) : ShippingQ_(ShippingQ), ReceivingQ_(ReceivingQ) {
    inventory["Broom"] = 5;
    inventory["Cup"] = 5;
    inventory["Hat"] = 5;
    inventory["Banana"] = 5;
    
    cpen333::process::mutex whmutex(MUTEX_NAME);
    cpen333::process::shared_object<SharedData> whmemory(WAREHOUSE_MEMORY_NAME);
    WarehouseInfo winfo;
    
    whmutex.lock();
    winfo = whmemory->winfo;
    whmutex.unlock();
    
    std::default_random_engine rnd(
                                   (unsigned int)std::chrono::system_clock::now().time_since_epoch().count());
    std::uniform_int_distribution<size_t> xdist(0, winfo.rows);
    std::uniform_int_distribution<size_t> ydist(0, winfo.cols);
    for (size_t itemid=0; itemid<CATALOGUE_SIZE; itemid++) {
      // generate until on a shelf
      size_t x,y;
      do {
        x = xdist(rnd);
        y = ydist(rnd);
      } while (winfo.warehouse[x][y] != SHELF_CHAR);
    
      std::uniform_int_distribution<size_t> sdist(0, 1);
      std::uniform_int_distribution<size_t> hdist(0, 6);
      
      size_t s = sdist(rnd);
      size_t h = hdist(rnd);
      
      whmutex.lock();
      whmemory->itemloc[itemid][COL_IDX] = x;
      whmemory->itemloc[itemid][ROW_IDX] = y;
      whmemory->itemloc[itemid][SIDE_IDX] = s;
      whmemory->itemloc[itemid][HEIGHT_IDX] = h;
      whmutex.unlock();
      
    }
  }

  void reportInventory() {
    std::cout << "Printing Inventory" << std::endl << std::endl;
    std::cout << "Item     Quantity" << std::endl;
    for(auto iter = inventory.begin(); iter != inventory.end(); ++iter)
    {
      std::cout << iter->first << " " << iter->second << std::endl;
    }
  }
  
  int checkShelfWeight(int x, int y, int s)
  {
    cpen333::process::mutex whmutex(MUTEX_NAME);
    cpen333::process::shared_object<SharedData> whmemory(WAREHOUSE_MEMORY_NAME);
    
    int weight = 0;
    
    for(int i=0; i<CATALOGUE_SIZE; i++)
    {
      int itemWeight = getItem(getItemName(i)).weight;
      int itemQuantity = inventory[getItemName(i)];
      whmutex.lock();
      if(whmemory->itemloc[i][COL_IDX] == x &&
         whmemory->itemloc[i][ROW_IDX] == y &&
         whmemory->itemloc[i][SIDE_IDX] == s)
      {
        weight += (itemWeight * itemQuantity);
      }
       whmutex.unlock();
    }
    
    return weight;
  }

};

#endif //CENTRALCOMPUTER_H

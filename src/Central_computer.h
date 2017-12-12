#ifndef CENTRALCOMPUTER_H
#define CENTRALCOMPUTER_H

#include <iostream>
#include <random>

#include "warehouse_common.h"

#include <cpen333/process/shared_memory.h>
#include <cpen333/process/mutex.h>
#include "CircularQueue.h"
#include "ItemQueue.h"
#include <cpen333/thread/thread_object.h>
#include <cpen333/process/condition_variable.h>

/**
 * Central computer class
 */
class Central_computer : public cpen333::thread::thread_object {
    
    std::map<std::string, int> inventory;
    CircularOrderQueue& ShippingQ_;
    ItemQueue& ReceivingQ_;
    int truckWeightsByDock[MAX_DOCKS];
    


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
      
        for(int i=0; i<MAX_DOCKS; i++)
        {
            truckWeightsByDock[i] = 0;
        }
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
  
    void unloadTruckToReceivingQ(std::vector< std::pair< Item, int > > truckContents, int dock)
    {
        cpen333::process::mutex whmutex(MUTEX_NAME);
        cpen333::process::shared_object<SharedData> whmemory(WAREHOUSE_MEMORY_NAME);
        cpen333::process::semaphore sem_docking(DOCK_SEM_NAME);
        
        truckWeightsByDock[dock] = 0;
        
        for(auto &p : truckContents)
        {
            for(int i=0; i<p.second; i++)
            {
                ReceivingQ_.add(p.first);
                truckWeightsByDock[dock] += p.first.weight;
            }
        }
    }
    
    void unloadItemFromTruck(Item item, int dock)
    {
        cpen333::process::condition_variable cv_dock(DOCK_CV_NAME + std::to_string(dock));

        truckWeightsByDock[dock] -= item.weight;
        
        if(truckWeightsByDock[dock] == 0)
        {
            // if truck is empty it can leave
            cv_dock.notify_one();
        }
    }
    
    void addToInventory(Item item)
    {
        (inventory[item.name])++;
    }
    
    void loadOrderOnTruck(Order order, int dock)
    {
        cpen333::process::condition_variable cv_dock(DOCK_CV_NAME + std::to_string(dock));

        for(int i=0; i<order.nitems; i++)
        {
            truckWeightsByDock[dock] += (order.items[i].weight * order.quantity[i]);
        }
        
        if(truckWeightsByDock[dock] >= TRUCK_THRESHOLD)
        {
            // if truck is full enough it can leave
            cv_dock.notify_one();
        }
        
    }
    
    int getValidDock(int type)
    {
        cpen333::process::mutex whmutex(MUTEX_NAME);
        cpen333::process::shared_object<SharedData> whmemory(WAREHOUSE_MEMORY_NAME);
        
        whmutex.lock();
        for(int i=0; i<whmemory->dinfo.ndocks; i++)
        {
            if(whmemory->dinfo.truck_type[i] == type && whmemory->dinfo.truck_present[i] == true)
            {
                whmutex.unlock();
                return i;
            }
        }
        whmutex.unlock();
        return INVALID_DOCK;
    }
    
    void setTruckPresent(int dockId)
    {
        cpen333::process::mutex whmutex(MUTEX_NAME);
        cpen333::process::shared_object<SharedData> whmemory(WAREHOUSE_MEMORY_NAME);
        
        whmutex.lock();
        whmemory->dinfo.truck_present[dockId] = true;
        whmutex.unlock();
    }
    
    void freeDock(int dockId)
    {
        cpen333::process::mutex whmutex(MUTEX_NAME);
        cpen333::process::shared_object<SharedData> whmemory(WAREHOUSE_MEMORY_NAME);
        
        whmutex.lock();
        whmemory->dinfo.truck_type[dockId] = INVALID_TYPE;
        whmemory->dinfo.truck_present[dockId] = false;
        whmutex.unlock();
    }
    
    int getFreeDockId(int type)
    {
        cpen333::process::mutex whmutex(MUTEX_NAME);
        cpen333::process::shared_object<SharedData> whmemory(WAREHOUSE_MEMORY_NAME);
        DockInfo dinfo;
        
        whmutex.lock();
        dinfo = whmemory->dinfo;
        whmutex.unlock();
        
        for(int i=0; i<dinfo.ndocks; i++)
        {
            whmutex.lock();
            if(whmemory->dinfo.truck_type[i] == INVALID_TYPE)
            {
                whmemory->dinfo.truck_type[i] = type;
                whmutex.unlock();
                return i;
            }
            whmutex.unlock();
        }
        
        return INVALID_DOCK;
    }
    
    int allowReceivingTruck(std::vector< std::pair< Item, int > > truckContents)
    {
        cpen333::process::mutex whmutex(MUTEX_NAME);
        cpen333::process::shared_object<SharedData> whmemory(WAREHOUSE_MEMORY_NAME);
        cpen333::process::semaphore sem_docking(DOCK_SEM_NAME);
        
        for(auto &p : truckContents)
        {
            whmutex.lock();
            int x = whmemory->itemloc[p.first.itemId][COL_IDX];
            int y = whmemory->itemloc[p.first.itemId][ROW_IDX];
            int s = whmemory->itemloc[p.first.itemId][SIDE_IDX];
            whmutex.unlock();
            
            if(checkShelfWeight(x, y, s) + (p.second*p.first.weight) > SHELF_CAPACITY)
            {
                sem_docking.notify();
                return INVALID_DOCK;
            }
        }
        
        return getFreeDockId(RECEIVING_TYPE);
    }
  
    int allowShippingTruck()
    {
        cpen333::process::semaphore sem_docking(DOCK_SEM_NAME);
        
        //
        //tmp:
        int dock = getFreeDockId(SHIPPING_TYPE);
        truckWeightsByDock[dock] = 0;
        return dock;
        //
        //
        
        if(ShippingQ_.isEmpty())
        {
            sem_docking.notify();
            return INVALID_DOCK;
        }
        else
        {
            int dock = getFreeDockId(SHIPPING_TYPE);
            truckWeightsByDock[dock] = 0;
            return dock;
        }
    }
  
    int main() {
        
        cpen333::process::mutex whmutex(MUTEX_NAME);
        cpen333::process::shared_object<SharedData> whmemory(WAREHOUSE_MEMORY_NAME);
        
        std::cout << "Central Computer started" << std::endl;
        
        while(true) {
            for(int i = 0; i < ORDERQ_SIZE; i++) {
                whmutex.lock();
                if((whmemory->newOrderQ[i].status == "Submitted") || (whmemory->newOrderQ[i].status == "Unavailable")) {
                    bool canComplete = true;
                    Order order = whmemory->newOrderQ[i];
                    whmutex.unlock();
                    
                    for(int j = 0; j < order.nitems; j++) {
                        if(inventory[order.items[j].name] >= order.quantity[j]) {
                            //inventory[order.items[j].name] -= order.quantity[j];
                        }
                        else {
                            canComplete = false;
                            whmutex.lock();
                            whmemory->newOrderQ[i].status = "Unavailable";
                            whmutex.unlock();
                        }
                    }
                    
                    if(canComplete) {
                        for(int j = 0; j < order.nitems; j++) {
                            inventory[order.items[j].name] -= order.quantity[j];
                            whmutex.lock();
                            whmemory->newOrderQ[i].status = "Processing";
                            whmutex.unlock();
                            
                            ShippingQ_.add(order);
                        }
                    }
                }
                else {
                    whmutex.unlock();
                }
            }
        }
        
        
        return 0;
    }

};

#endif //CENTRALCOMPUTER_H

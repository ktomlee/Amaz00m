#include "warehouse_common.h"
#include "Central_computer.h"
#include "CircularQueue.h"
#include "ItemQueue.h"

#include <iostream>


int main()
{
  /*
  cpen333::process::mutex whmutex(MUTEX_NAME);
  cpen333::process::shared_object<SharedData> whmemory(WAREHOUSE_MEMORY_NAME);
  
  while(1)
  {
    whmutex.lock();
    
    for(int i=whmemory->newOrderIdx_start; i<=whmemory->newOrderIdx_end; i++)
    {
      std::cout << "Order Id: " << whmemory->newOrderQ[i].orderId << std::endl;
      std::cout << "first item: " << whmemory->newOrderQ[i].items[0].name << " x" << whmemory->newOrderQ[i].quantity[0] << std::endl << std::endl;
    }
    
    whmutex.unlock();
    
    std::cout << "Press ENTER to go again." << std::endl;
    std::cin.get();
  }
  */
    
    
  
  Central_computer cc;
  
  cc.reportInventory();
  

  return 0;
}

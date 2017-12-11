#ifndef CENTRALCOMPUTER_H
#define CENTRALCOMPUTER_H

#include <iostream>

/**
 * Central computer class
 */
class Central_computer {
  std::map<std::string, int> inventory;


 public:
  /**
   * Creates a queue with provided circular buffer size
   * @param buffsize size of circular buffer
   */
  Central_computer() {
    inventory["Broom"] = 5;
    inventory["Cup"] = 5;
    inventory["Hat"] = 5;
    inventory["Banana"] = 5;
  }

  void reportInventory() {
    std::cout << "Printing Inventory" << std::endl << std::endl;
    std::cout << "Item     Quantity" << std::endl;
    for(auto iter = inventory.begin(); iter != inventory.end(); ++iter)
    {
      std::cout << iter->first << " " << iter->second << std::endl;
    }
  }

};

#endif //CENTRALCOMPUTER_H

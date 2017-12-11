/**
 * @file
 *
 * This contains the data structure for storing the music library locally in memory.
 *
 */
#ifndef LAB4_MUSIC_LIBRARY_H
#define LAB4_MUSIC_LIBRARY_H

#include "Song.h"
#include <vector>
#include <set>
#include <regex>
#include "warehouse_common.h"
#include "CartItem.h"

#include <cpen333/process/shared_memory.h>
#include <cpen333/process/mutex.h>

#define SERVER_MUTEX "server_mutex"

class Cart {
  //std::map<std::string, int> cartInfo;
  //Order order;
  std::set<CartItem> cartItems;
  
public:
  
  //Adds an item to the cart
  bool add(const CartItem& cartItem) {
    // try to add item to the cart
    
    auto elem = cartItems.insert(cartItem);
    
    //if(!elem.second) elem.first->quantity = std::to_string(std::stoi(elem.first->quantity) + std::stoi(cartItem.quantity));
    
    return elem.second;
  }
  
  bool remove(const CartItem& cartItem) {
    for(auto &item : cartItems)
    {
      if(item.item == cartItem.item)
      {
        cartItems.erase(item);
        return true;
      }
    //auto elem = cartItems.erase(cartItem);
    }
    
    return false;
  }
    
  std::set<CartItem> show() {
    return cartItems;
  }
  
  int submit()
  {
    // add something to new order q in warehouse common
    cpen333::process::mutex whmutex(MUTEX_NAME);
    cpen333::process::shared_object<SharedData> whmemory(WAREHOUSE_MEMORY_NAME);
    
    Order order_to_add;
    order_to_add.nitems = 0;
    
    int order_id;
    
    for(auto &ci : cartItems)
    {
      Item item_to_add = getItem(ci.item);
      order_to_add.items[order_to_add.nitems] = item_to_add;
      order_to_add.quantity[order_to_add.nitems] = std::stoi(ci.quantity);
      order_to_add.nitems++;
      order_to_add.weight += (item_to_add.weight * std::stoi(ci.quantity));
    }
    
    whmutex.lock();
    
    order_to_add.orderId = whmemory->nOrders + 1;
    order_id = order_to_add.orderId;
    whmemory->nOrders++;
    
    whmemory->newOrderIdx_end++;
    whmemory->newOrderQ[whmemory->newOrderIdx_end] = order_to_add;
    
    whmutex.unlock();
        
    // clear cartItems
    cartItems.clear();
    
    return order_id;
  }
  
  Order check(int checkid)
  {
    cpen333::process::mutex whmutex(MUTEX_NAME);
    cpen333::process::shared_object<SharedData> whmemory(WAREHOUSE_MEMORY_NAME);
    Order result;
    result.orderId = 0;
    
    whmutex.lock();
    
    for(int i=whmemory->newOrderIdx_start; i<=whmemory->newOrderIdx_end; i++)
    {
      if(whmemory->newOrderQ[i].orderId == checkid)
      {
        result = whmemory->newOrderQ[i];
        whmutex.unlock();
        return result;
      }
    }
    
    whmutex.unlock();
    
    return result;
  }
  
};

// Stores a list of songs
class MusicLibrary {
  // private vector
  std::set<Song> songs_;

 public:

  /**
   * Finds songs in the database matching title and artist expressions
   * @param artist_regex artist regular expression
   * @param title_regex title regular expression
   * @return set of songs matching expressions
   */
  std::vector<Song> find(const std::string& artist_regex,
                         const std::string& title_regex) const {
std::vector<Song> out;

    //=====================================================
    // TODO: Modify to also include title_regex in search
    //=====================================================
    
    // compile regex for title
    std::regex tregex(title_regex);

    // compile regular expressions
    std::regex aregex(artist_regex);

    // search through songs for titles and artists matching search expressions
    for (const auto& song : songs_) {
      if (std::regex_search(song.artist, aregex) &&
          std::regex_search(song.title, tregex))
      {
        out.push_back(song);
      }
    }

    return out;
  }

  /**
   * Retrieves the unmodifiable list of songs
   * @return internal set of songs
   */
  const std::set<Song>& songs() const {
    return songs_;
  }
};


#endif //LAB4_MUSIC_LIBRARY_H

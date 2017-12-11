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

#define SERVER_MUTEX "server_mutex"

class Cart {
    std::map<std::string, int> cartInfo;
    //Order order;
    std::set<CartItem> cartItems;
    
    public:
    
    //Adds an item to the cart
    bool add(const CartItem& cartItem) {
        // try to add item to the cart
        
        auto elem = cartItems.insert(cartItem);
        return elem.second;
        //return true;
    }
    
    bool remove(const CartItem& cartItem) {
        
        auto elem = cartItems.erase(cartItem);
        return elem;

    }
    
    std::map<std::string, int> show() {
        return cartInfo;
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

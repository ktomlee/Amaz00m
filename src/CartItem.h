/**
 *@file
 *
 * This file contains the definition of a CartItem in our Web Server
 *
 */

#ifndef CART_ITEM_H
#define CART_ITEM_H

#include <string>
#include <iostream>

// This "CartItem" class is immutable
class CartItem {
 public:
  const std::string item;
  const std::string quantity;

  CartItem(const std::string item, const std::string& quantity) : item(item), quantity(quantity) {}

  std::string toString() const {
    std::string out = item;
    out.append(": ");
    out.append(quantity);
    return out;
  }

  // less-than operator for comparisons, sort by artist then by song
  friend bool operator<(const CartItem& a, const CartItem& b) {
    if (a.item < b.item) {
      return true;
    } else if (a.item > b.item) {
      return false;
    }
    return a.quantity < b.quantity;
  }

  // equal-to operator for comparisons, both artist and title must match
  friend bool operator==(const CartItem& a, const CartItem& b) {
    return (a.item == b.item);
  }

  // not-equal-to operator for comparisons
  friend bool operator!=(const CartItem& a, const CartItem& b) {
    return !(a.item == b.item);
  }

  // overloaded stream operator for printing
  //    std::cout << song
  friend std::ostream& operator<<(std::ostream& os, const CartItem& s) {
    os << s.toString();
    return os;
  }

};

#endif //LAB4_MUSIC_LIBRARY_SONG_H

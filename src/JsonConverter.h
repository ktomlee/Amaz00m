/**
 * @file
 *
 * This file provides all the JSON encoding/decoding functionality
 *
 */

#ifndef LAB4_MUSIC_LIBRARY_JSON_H
#define LAB4_MUSIC_LIBRARY_JSON_H

#include "Song.h"
#include "CartItem.h"
#include "Message.h"
#include "warehouse_common.h"
#include "json.hpp"   // json parsing
#include <vector>
#include <memory>     // for std::unique_ptr
#include <set>

// convenience alias for json
using JSON = nlohmann::json;

// types of messages
#define MESSAGE_ADD "add"
#define MESSAGE_ADD_RESPONSE "add_response"
#define MESSAGE_REMOVE "remove"
#define MESSAGE_REMOVE_RESPONSE "remove_response"
#define MESSAGE_SUBMIT "submit"
#define MESSAGE_SUBMIT_RESPONSE "submit_response"
#define MESSAGE_CHECK "check"
#define MESSAGE_CHECK_RESPONSE "check_response"
#define MESSAGE_SHOW "show"
#define MESSAGE_SHOW_RESPONSE "show_response"
#define MESSAGE_GOODBYE "goodbye"

// other keys
#define MESSAGE_TYPE "msg"
#define MESSAGE_STATUS "status"
#define MESSAGE_INFO "info"

#define MESSAGE_SHOW_RESULT "result"
#define MESSAGE_CHECK_RESULT "result"
#define MESSAGE_SEARCH_RESULTS "results"
#define MESSAGE_ORDER "order"
#define MESSAGE_ORDER_ID "orderID"
#define MESSAGE_ORDER_INFO "orderInfo"
#define MESSAGE_ITEM_REGEX "item_regex"
#define MESSAGE_ITEM "item"
#define MESSAGE_QUANTITY "quantity"
#define MESSAGE_CART_ITEM "cart_item"
#define MESSAGE_CART_QUANTITY "cart_quantity"
#define MESSAGE_ORDER_STATUS "order_status"


/**
 * Handles all conversions to and from JSON
 */
class JsonConverter {
 public:
  
  /**
   * Converts the item to a JSON object
   * @param song song to jsonify
   * @return JSON object representation
   */
  static JSON toJSON(const CartItem & cartItem) {
    JSON j;
    j[MESSAGE_CART_ITEM] = cartItem.item;
    j[MESSAGE_CART_QUANTITY] = cartItem.quantity;
    return j;
  }

  /**
   * Converts a vector of items to a JSON array of items
   * @param songs vector of songs to jsonify
   * @return JSON array representation
   */
    static JSON toJSON(const std::set<CartItem> &cartItems) {
    JSON j;
        for (const auto& cartItem : cartItems) {
            j.push_back(toJSON(cartItem));
    }
    return j;
  }
  
  /**
   * Converts the item to a JSON object
   * @param song song to jsonify
   * @return JSON object representation
   */
    
  static JSON toJSON(const std::pair<Item, int> &p) {
    JSON j;
    j[MESSAGE_ITEM] = p.first.name;
    j[MESSAGE_QUANTITY] = p.second;
    
    return j;
  }
    
  
  /**
   * Converts a vector of items to a JSON array of items
   * @param songs vector of songs to jsonify
   * @return JSON array representation
   */
    
  static JSON toJSON(const std::vector<std::pair<Item, int>> &result) {
    JSON j;
    for (const auto& r : result) {
      j.push_back(toJSON(r));
    }
    return j;
  }
    
  
  /**
   * Converts a vector of items to a JSON array of items
   * @param songs vector of songs to jsonify
   * @return JSON array representation
   */
    
  static JSON toJSON(const Order& order) {
    JSON j;
    
    j[MESSAGE_ORDER_ID] = order.orderId;
    j[MESSAGE_ORDER_INFO] = toJSON((Item *)order.items, order.nitems, (int *)order.quantity);
    j[MESSAGE_ORDER_STATUS] = order.status;
    
    return j;
  }
    
    static JSON toJSON(const Item &item, int quantity) {
        JSON j;
        j[MESSAGE_ITEM] = item.name;
        j[MESSAGE_QUANTITY] = quantity;
        return j;
    }
    
    static JSON toJSON(const Item *items, int n, int *quantity) {
        JSON j;
        for(int i=0; i<n; i++) {
            j.push_back(toJSON(items[i], quantity[i]));
        }
        //j[MESSAGE_ORDER_STATUS] = status;
        return j;
    }
    
  /**
   * Converts an "add" message to a JSON object
   * @param add message
   * @return JSON object representation
   */
  static JSON toJSON(const AddMessage &add) {
    JSON j;
    j[MESSAGE_TYPE] = MESSAGE_ADD;
    j[MESSAGE_CART_ITEM] = toJSON(add.cartItem);
    return j;
  }

  /**
   * Converts an "add" response message to a JSON object
   * @param add_response message
   * @return JSON object representation
   */
  static JSON toJSON(const AddResponseMessage &add_response) {
    JSON j;
    j[MESSAGE_TYPE] = MESSAGE_ADD_RESPONSE;
    j[MESSAGE_STATUS] = add_response.status;
    j[MESSAGE_INFO] = add_response.info;
    j[MESSAGE_ADD] = toJSON(add_response.add);
    return j;
  }

  /**
   * Converts a "remove" message to a JSON object
   * @param remove message
   * @return JSON object representation
   */
  static JSON toJSON(const RemoveMessage &remove) {
    JSON j;
    j[MESSAGE_TYPE] = MESSAGE_REMOVE;
    j[MESSAGE_CART_ITEM] = toJSON(remove.cartItem);

    return j;
  }
  
  /**
   * Converts a "remove" response message to a JSON object
   * @param remove_response message
   * @return JSON object representation
   */
  static JSON toJSON(const RemoveResponseMessage &remove_response) {
    JSON j;
    j[MESSAGE_TYPE] = MESSAGE_REMOVE_RESPONSE;
    j[MESSAGE_STATUS] = remove_response.status;
    j[MESSAGE_INFO] = remove_response.info;
    j[MESSAGE_REMOVE] = toJSON(remove_response.remove);
    return j;
  }
  
  /**
   * Converts a "search" message to a JSON object
   * @param search message
   * @return JSON object representation
   */
    /*
  static JSON toJSON(const SearchMessage &search) {
    JSON j;
    j[MESSAGE_TYPE] = MESSAGE_SEARCH;
    j[MESSAGE_ITEM_REGEX] = search.item_regex;
    return j;
  }
     */

  /**
   * Converts a "search" response message to a JSON object
   * @param search_response message
   * @return JSON object representation
   */
    /*
  static JSON toJSON(const SearchResponseMessage &search_response) {
    JSON j;
    j[MESSAGE_TYPE] = MESSAGE_SEARCH_RESPONSE;
    j[MESSAGE_STATUS] = search_response.status;
    j[MESSAGE_INFO] = search_response.info;
    j[MESSAGE_SEARCH] = toJSON(search_response.search);
    j[MESSAGE_SEARCH_RESULTS] = toJSON(search_response.results);
    return j;
  }
     */

  /**
   * Converts a "remove" message to a JSON object
   * @param remove message
   * @return JSON object representation
   */
  static JSON toJSON(const SubmitMessage &submit) {
    JSON j;
    j[MESSAGE_TYPE] = MESSAGE_SUBMIT;
    return j;
  }
  
  /**
   * Converts a "remove" response message to a JSON object
   * @param remove_response message
   * @return JSON object representation
   */
  static JSON toJSON(const SubmitResponseMessage &submit_response) {
    JSON j;
    j[MESSAGE_TYPE] = MESSAGE_SUBMIT_RESPONSE;
    j[MESSAGE_STATUS] = submit_response.status;
    j[MESSAGE_INFO] = submit_response.info;
    j[MESSAGE_SUBMIT] = toJSON(submit_response.submit);
    
    return j;
  }
  
  /**
   * Converts a "remove" message to a JSON object
   * @param remove message
   * @return JSON object representation
   */
  static JSON toJSON(const CheckMessage &check) {
    JSON j;
    j[MESSAGE_TYPE] = MESSAGE_CHECK;
    j[MESSAGE_ORDER_ID] = check.orderId;
    
    return j;
  }
  
  /**
   * Converts a "remove" response message to a JSON object
   * @param remove_response message
   * @return JSON object representation
   */
  static JSON toJSON(const CheckResponseMessage &check_response) {
    JSON j;
    j[MESSAGE_TYPE] = MESSAGE_CHECK_RESPONSE;
    j[MESSAGE_STATUS] = check_response.status;
    j[MESSAGE_INFO] = check_response.info;
    j[MESSAGE_CHECK_RESULT] = toJSON(check_response.result);
    j[MESSAGE_CHECK] = toJSON(check_response.check);
    
    return j;
  }
  
  /**
   * Converts a "remove" message to a JSON object
   * @param remove message
   * @return JSON object representation
   */
  static JSON toJSON(const ShowMessage &show) {
    JSON j;
    j[MESSAGE_TYPE] = MESSAGE_SHOW;
    
    return j;
  }
  
  /**
   * Converts a "remove" response message to a JSON object
   * @param remove_response message
   * @return JSON object representation
   */
  static JSON toJSON(const ShowResponseMessage &show_response) {
    JSON j;
    j[MESSAGE_TYPE] = MESSAGE_SHOW_RESPONSE;
    j[MESSAGE_STATUS] = show_response.status;
    j[MESSAGE_INFO] = show_response.info;
    j[MESSAGE_SHOW] = toJSON(show_response.show);
    j[MESSAGE_SHOW_RESULT] = toJSON(show_response.results);
    
    return j;
  }
  
  /**
   * Converts a "goodbye" message to a JSON object
   * @param goodbye message
   * @return JSON object representation
   */
  static JSON toJSON(const GoodbyeMessage &goodbye) {
    JSON j;
    j[MESSAGE_TYPE] = MESSAGE_GOODBYE;
    return j;
  }

  /**
   * Converts a message to a JSON object, automatically detecting the type
   * @param message
   * @return JSON object representation, {"status"="ERROR", "info"=...} if not recognized
   */
  static JSON toJSON(const Message &msg) {

    switch(msg.type()) {
      case ADD: {
        return toJSON((AddMessage &) msg);
      }
      case ADD_RESPONSE: {
        return toJSON((AddResponseMessage &) msg);
      }
      case REMOVE: {
        return toJSON((RemoveMessage &) msg);
      }
      case REMOVE_RESPONSE: {
        return toJSON((RemoveResponseMessage &) msg);
      }
    /*
      case SEARCH: {
        return toJSON((SearchMessage &) msg);
      }
      case SEARCH_RESPONSE: {
        return toJSON((SearchResponseMessage &) msg);
      }
    */
      case SUBMIT: {
        return toJSON((SubmitMessage &) msg);
      }
      case SUBMIT_RESPONSE: {
        return toJSON((SubmitResponseMessage &) msg);
      }
      case CHECK: {
        return toJSON((CheckMessage &) msg);
      }
      case CHECK_RESPONSE: {
        return toJSON((CheckResponseMessage &) msg);
      }
      case SHOW: {
        return toJSON((ShowMessage &) msg);
      }
      case SHOW_RESPONSE: {
        return toJSON((ShowResponseMessage &) msg);
      }
      case GOODBYE: {
        return toJSON((GoodbyeMessage &) msg);
      }
      default: {

      }
    }

    // unknown message type
    JSON err;
    err[MESSAGE_STATUS] = MESSAGE_STATUS_ERROR;
    err[MESSAGE_INFO] = std::string("Unknown message type");
    return err;
  }
  
  /**
   * Converts a JSON object representing a song to a Song object
   * @param j JSON object
   * @return Song
   */
  static void parseItems(const JSON &jitems, Order &order) {
    int i=0;
    order.nitems = 0;
    
    for (const auto& res : jitems) {
        order.items[i].name = res[MESSAGE_ITEM];
        order.quantity[i] = res[MESSAGE_QUANTITY];
        order.nitems++;
        //order.status = res[MESSAGE_ORDER_STATUS];
    
      i++;
    }
  }
  
  /**
   * Converts a JSON object representing a song to a Song object
   * @param j JSON object
   * @return Song
   */
  static Order parseOrder(const JSON &jorder) {
    Order order;
    
    order.orderId = jorder[MESSAGE_ORDER_ID];
    order.status = jorder[MESSAGE_ORDER_STATUS];
    parseItems(jorder[MESSAGE_ORDER_INFO], order);
    
    return order;
  }
    
    static CartItem parseCartItem(const JSON &j) {
      return CartItem(j[MESSAGE_CART_ITEM], j[MESSAGE_CART_QUANTITY]);
    }

  /**
   * Converts a JSON array representing a list of songs to a
   * vector of Song objects
   * @param jsongs JSON array
   * @return resulting vector of Song
*/
  static std::set<CartItem> parseCartItems(const JSON &jcartItems) {
    std::set<CartItem> out;

    for (const auto& cartItem : jcartItems) {
      out.insert(parseCartItem(cartItem));
    }

    return out;
  }
  
  /**
   * Converts a JSON object representing an AddMessage to a AddMessage object
   * @param j JSON object
   * @return AddMessage
   */
  static AddMessage parseAdd(const JSON &jadd) {
    CartItem cartItem = parseCartItem(jadd[MESSAGE_CART_ITEM]);
    return AddMessage(cartItem);
  }

  /**
   * Converts a JSON object representing an AddResponseMessage to an AddResponseMessage object
   * @param j JSON object
   * @return AddResponseMessage
   */
  static AddResponseMessage parseAddResponse(const JSON &jaddr) {
    AddMessage add = parseAdd(jaddr[MESSAGE_ADD]);
    std::string status = jaddr[MESSAGE_STATUS];
    std::string info = jaddr[MESSAGE_INFO];
    return AddResponseMessage(add, status, info);
  }

  
  /**
   * Converts a JSON object representing a RemoveMessage to a RemoveMessage object
   * @param j JSON object
   * @return RemoveMessage
   */
  static RemoveMessage parseRemove(const JSON &jrmv) {
    CartItem cartItem = parseCartItem(jrmv[MESSAGE_CART_ITEM]);
    return RemoveMessage(cartItem);
  }
  
  /**
   * Converts a JSON object representing an AddResponseMessage to an AddResponseMessage object
   * @param j JSON object
   * @return AddResponseMessage
   */
  static RemoveResponseMessage parseRemoveResponse(const JSON &jaddr) {
    RemoveMessage remove = parseRemove(jaddr[MESSAGE_REMOVE]);
    std::string status = jaddr[MESSAGE_STATUS];
    std::string info = jaddr[MESSAGE_INFO];
    return RemoveResponseMessage(remove, status, info);
  }
  
  /**
   * Converts a JSON object representing a SearchMessage to a SearchMessage object
   * @param j JSON object
   * @return SearchMessage
   */
    /*
  static SearchMessage parseSearch(const JSON &jsearch) {
    std::string item_regex = jsearch[MESSAGE_ITEM_REGEX];
    return SearchMessage(item_regex);
  }
     */

  /**
   * Converts a JSON object representing a SearchResponseMessage to a SearchResponseMessage object
   * @param j JSON object
   * @return SearchResponseMessage
   */
    /*
  static SearchResponseMessage parseSearchResponse(const JSON &jsearchr) {
    SearchMessage search = parseSearch(jsearchr[MESSAGE_SEARCH]);
    Order results;
    parseItems(jsearchr[MESSAGE_SEARCH_RESULTS], results);
    std::string status = jsearchr[MESSAGE_STATUS];
    std::string info = jsearchr[MESSAGE_INFO];
    return SearchResponseMessage(search, results, status, info);
  }
     */

  /**
   * Converts a JSON object representing an AddMessage to a AddMessage object
   * @param j JSON object
   * @return AddMessage
   */
  static SubmitMessage parseSubmit(const JSON &jsub) {
    return SubmitMessage();
  }
  
  /**
   * Converts a JSON object representing an AddResponseMessage to an AddResponseMessage object
   * @param j JSON object
   * @return AddResponseMessage
   */
  static SubmitResponseMessage parseSubmitResponse(const JSON &jsubr) {
    SubmitMessage sub = parseSubmit(jsubr[MESSAGE_SUBMIT]);
    std::string status = jsubr[MESSAGE_STATUS];
    std::string info = jsubr[MESSAGE_INFO];
    return SubmitResponseMessage(sub, status, info);
  }
  
  /**
   * Converts a JSON object representing an AddMessage to a AddMessage object
   * @param j JSON object
   * @return AddMessage
   */
  static CheckMessage parseCheck(const JSON &jcheck) {
    std::string orderId = jcheck[MESSAGE_ORDER_ID];
    return CheckMessage(orderId);
  }
  
  /**
   * Converts a JSON object representing an AddResponseMessage to an AddResponseMessage object
   * @param j JSON object
   * @return AddResponseMessage
   */
  static CheckResponseMessage parseCheckResponse(const JSON &jcheckr) {
    CheckMessage check = parseCheck(jcheckr[MESSAGE_CHECK]);
    std::string status = jcheckr[MESSAGE_STATUS];
    std::string info = jcheckr[MESSAGE_INFO];
    Order result = parseOrder(jcheckr[MESSAGE_CHECK_RESULT]);
    return CheckResponseMessage(check, status, result, info);
  }
  
  /**
   * Converts a JSON object representing an AddMessage to a AddMessage object
   * @param j JSON object
   * @return AddMessage
   */

  static ShowMessage parseShow(const JSON &jshow) {
    return ShowMessage();
  }

  /**
   * Converts a JSON object representing an AddResponseMessage to an AddResponseMessage object
   * @param j JSON object
   * @return AddResponseMessage
   */
    
  static ShowResponseMessage parseShowResponse(const JSON &jshowr) {
    ShowMessage show = parseShow(jshowr[MESSAGE_SHOW]);
    std::string status = jshowr[MESSAGE_STATUS];
    std::string info = jshowr[MESSAGE_INFO];
    std::set<CartItem> results = parseCartItems(jshowr[MESSAGE_SHOW_RESULT]);
    return ShowResponseMessage(show, results, status, info);
  }
  
  /**
   * Converts a JSON object representing a GoodbyeMessage to a GoodbyeMessage object
   * @param j JSON object
   * @return GoodbyeMessage
   */
  static GoodbyeMessage parseGoodbye(const JSON &jbye) {
    return GoodbyeMessage();
  }

  /**
   * Detects the message type from a JSON object
   * @param jmsg JSON object
   * @return message type
   */
  static MessageType parseType(const JSON &jmsg) {
    std::string msg = jmsg[MESSAGE_TYPE];
    if (MESSAGE_ADD == msg) {
      return MessageType::ADD;
    } else if (MESSAGE_ADD_RESPONSE == msg) {
      return MessageType::ADD_RESPONSE;
    } else if (MESSAGE_REMOVE == msg) {
      return MessageType::REMOVE;
    } else if (MESSAGE_REMOVE_RESPONSE == msg) {
      return MessageType::REMOVE_RESPONSE;
    } else if (MESSAGE_SUBMIT == msg) {
      return MessageType::SUBMIT;
    } else if (MESSAGE_SUBMIT_RESPONSE == msg) {
      return MessageType::SUBMIT_RESPONSE;
    } else if (MESSAGE_CHECK == msg) {
      return MessageType::CHECK;
    } else if (MESSAGE_CHECK_RESPONSE == msg) {
      return MessageType::CHECK_RESPONSE;
    } else if (MESSAGE_SHOW == msg) {
      return MessageType::SHOW;
    } else if (MESSAGE_SHOW_RESPONSE == msg) {
      return MessageType::SHOW_RESPONSE;
    } else if (MESSAGE_GOODBYE == msg) {
      return MessageType::GOODBYE;
    }
    return MessageType::UNKNOWN;
  }

  /**
   * Parses a Message object from JSON, returning in a smart pointer
   * to preserve polymorphism.
   *
   * @param jmsg JSON object
   * @return parsed Message object, or nullptr if invalid
   */
  static std::unique_ptr<Message> parseMessage(const JSON &jmsg) {

    //=============================================================
    // TODO: Add parsing of "remove" and its response
    //=============================================================

    MessageType type = parseType(jmsg);
    switch(type) {
      case ADD: {
        return std::unique_ptr<Message>(new AddMessage(parseAdd(jmsg)));
      }
      case ADD_RESPONSE: {
        return std::unique_ptr<Message>(new AddResponseMessage(parseAddResponse(jmsg)));
      }
      case REMOVE: {
        return std::unique_ptr<Message>(new RemoveMessage(parseRemove(jmsg)));
      }
      case REMOVE_RESPONSE: {
        return std::unique_ptr<Message>(new RemoveResponseMessage(parseRemoveResponse(jmsg)));
      }
      /*case SEARCH: {
        return std::unique_ptr<Message>(new SearchMessage(parseSearch(jmsg)));
      }
      case SEARCH_RESPONSE: {
        return std::unique_ptr<Message>(new SearchResponseMessage(parseSearchResponse(jmsg)));
      }*/
      case SUBMIT: {
        return std::unique_ptr<Message>(new SubmitMessage(parseSubmit(jmsg)));
      }
      case SUBMIT_RESPONSE: {
        return std::unique_ptr<Message>(new SubmitResponseMessage(parseSubmitResponse(jmsg)));
      }
      case CHECK: {
        return std::unique_ptr<Message>(new CheckMessage(parseCheck(jmsg)));
      }
      case CHECK_RESPONSE: {
        return std::unique_ptr<Message>(new CheckResponseMessage(parseCheckResponse(jmsg)));
      }
      case SHOW: {
        return std::unique_ptr<Message>(new ShowMessage(parseShow(jmsg)));
      }
      case SHOW_RESPONSE: {
        return std::unique_ptr<Message>(new ShowResponseMessage(parseShowResponse(jmsg)));
      }
      case GOODBYE: {
        return std::unique_ptr<Message>(new GoodbyeMessage(parseGoodbye(jmsg)));
      }
    }

    return std::unique_ptr<Message>(nullptr);
  }

};

#endif //LAB4_MUSIC_LIBRARY_JSON_H

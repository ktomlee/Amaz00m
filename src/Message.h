/**
 * @file
 *
 * This file contains all message-related objects, independent of the specific API
 *
 * This middle layer allows us to abstract away many of the communication details,
 * allowing us to focus on the core functional implementation.
 *
 */
#ifndef LAB4_MUSIC_LIBRARY_MESSAGES_H
#define LAB4_MUSIC_LIBRARY_MESSAGES_H

#include "Song.h"
#include "warehouse_common.h"
#include <string>

/**
 * Types of messages that can be sent between client/server
 */
enum MessageType {
  ADD,
  ADD_RESPONSE,
  REMOVE,
  REMOVE_RESPONSE,
  SEARCH,
  SEARCH_RESPONSE,
  SUBMIT,
  SUBMIT_RESPONSE,
  CHECK,
  CHECK_RESPONSE,
  SHOW,
  SHOW_RESPONSE,
  GOODBYE,
  UNKNOWN
};

// status messages for response objects
#define MESSAGE_STATUS_OK "OK"
#define MESSAGE_STATUS_ERROR "ERROR"

/**
 * Base class for messages
 */
class Message {
 public:
  virtual MessageType type() const = 0;
};

class ResponseMessage : public Message {
 public:
  const std::string status;
  const std::string info;
  ResponseMessage(const std::string& status,
                  const std::string& info="") :
      status(status), info(info){}

};

/**
 * Add a song to the library
 */
class AddMessage : public Message {
 public:
  const Item item;
  const int quantity;

  AddMessage(const Item& item, const int quantity)  : item(item), quantity(quantity) {}

  MessageType type() const {
    return MessageType::ADD;
  }
};

/**
 * Response to adding a song to the library
 */
class AddResponseMessage : public ResponseMessage {
 public:
  const AddMessage  add;

  AddResponseMessage(const AddMessage& add, std::string status, std::string info = "") :
      ResponseMessage(status, info), add(add) {}

  MessageType type() const {
    return MessageType::ADD_RESPONSE;
  }
};

/**
 * Remove a song from the library
 */
class RemoveMessage : public Message {
 public:
  const Item item;
  const int quantity;

  RemoveMessage(const Item& item, const int quantity) : item(item), quantity(quantity) {}

  MessageType type() const {
    return MessageType::REMOVE;
  }
};

/**
 * Response to removing a song from the library
 */
class RemoveResponseMessage : public ResponseMessage {
 public:
  const RemoveMessage remove;

  RemoveResponseMessage(const RemoveMessage& remove, std::string status, std::string info = "") :
      ResponseMessage(status, info), remove(remove) {}

  MessageType type() const {
    return MessageType::REMOVE_RESPONSE;
  }
};

/**
 * Search the library using regular expressions
 */
class SearchMessage : public Message {
 public:
  const std::string item_regex;

  SearchMessage(const std::string& item_regex) :
      item_regex(item_regex) {}

  MessageType type() const {
    return MessageType::SEARCH;
  }
};

/**
 * Response to a library search
 */
class SearchResponseMessage : public ResponseMessage {
 public:
  const SearchMessage search;
  const std::vector<std::pair<Item, int>> results;

  SearchResponseMessage(const SearchMessage& search, const std::vector<std::pair<Item, int>>& results,
    const std::string& status, const std::string& info = "" ) :
      ResponseMessage(status, info), search(search), results(results) {}

  MessageType type() const {
    return MessageType::SEARCH_RESPONSE;
  }
};

/**
 * Add a song to the library
 */
class SubmitMessage : public Message {
public:
  MessageType type() const {
    return MessageType::SUBMIT;
  }
};

/**
 * Response to adding a song to the library
 */
class SubmitResponseMessage : public ResponseMessage {
public:
  const SubmitMessage  submit;
  
  SubmitResponseMessage(const SubmitMessage& submit, std::string status, std::string info = "") :
  ResponseMessage(status, info), submit(submit) {}
  
  MessageType type() const {
    return MessageType::SUBMIT_RESPONSE;
  }
};

/**
 * Search the library using regular expressions
 */
class CheckMessage : public Message {
public:
  const int orderId;
  
  CheckMessage(const int orderId)  : orderId(orderId) {}
  
  MessageType type() const {
    return MessageType::CHECK;
  }
};

/**
 * Response to adding a song to the library
 */
class CheckResponseMessage : public ResponseMessage {
public:
  const CheckMessage check;
  const Order result;
  
  CheckResponseMessage(const CheckMessage& check, std::string status, Order result, std::string info = "") :
  ResponseMessage(status, info), check(check), result(result) {}
  
  MessageType type() const {
    return MessageType::CHECK_RESPONSE;
  }
};

/**
 * Add a song to the library
 */
class ShowMessage : public Message {
public:
  MessageType type() const {
    return MessageType::SHOW;
  }
};

/**
 * Response to adding a song to the library
 */
class ShowResponseMessage : public ResponseMessage {
public:
  const ShowMessage show;
  const Order result;
  
  ShowResponseMessage(const ShowMessage& show, std::string status, Order result, std::string info = "") :
  ResponseMessage(status, info), show(show), result(result) {}
  
  MessageType type() const {
    return MessageType::SHOW_RESPONSE;
  }
};

/**
 * Goodbye message
 */
class GoodbyeMessage : public Message {
 public:
  MessageType type() const {
    return MessageType::GOODBYE;
  }
};

#endif //LAB4_MUSIC_LIBRARY_MESSAGES_H

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
#include "CartItem.h"
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
 * Add an order to the cart
 */
class AddMessage : public Message {
 public:
    const CartItem cartItem;

  AddMessage(const CartItem& cartItem)  : cartItem(cartItem) {}

  MessageType type() const {
    return MessageType::ADD;
  }
};

/**
 * Response to adding an order to the cart
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
 * Remove an order from the cart
 */
class RemoveMessage : public Message {
 public:
  const CartItem cartItem;

  RemoveMessage(const CartItem& cartItem)  : cartItem(cartItem) {}

  MessageType type() const {
    return MessageType::REMOVE;
  }
};

/**
 * Response to removing an order from the cart
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
 * Checkout cart and add to order queue
 */
class SubmitMessage : public Message {
public:
  MessageType type() const {
    return MessageType::SUBMIT;
  }
};

/**
 * Response to order submit
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
 * Check status of an existing order
 */
class CheckMessage : public Message {
public:
  const std::string orderId;
  
  CheckMessage(const std::string orderId)  : orderId(orderId) {}
  
  MessageType type() const {
    return MessageType::CHECK;
  }
};

/**
 * Response to status check
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
 * Show cart
 */
class ShowMessage : public Message {
public:
  MessageType type() const {
    return MessageType::SHOW;
  }
};

/**
 * Response to show cart
 */
class ShowResponseMessage : public ResponseMessage {
public:
    const ShowMessage show;
    const std::set<CartItem> results;
  
  ShowResponseMessage(const ShowMessage& show, const std::set<CartItem>& results, std::string status, std::string info = "") :
  ResponseMessage(status, info), show(show), results(results) {}
  
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

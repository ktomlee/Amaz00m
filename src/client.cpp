/**
 * @file
 *
 * The Music Library Client connects to a remote server and provides an interface
 * for adding, removing, and searching for songs in the server database
 *
 */

#include "MusicLibrary.h"
#include "JsonWarehouseApi.h"
#include "CartItem.h"

#include <cpen333/process/socket.h>

#include <iostream>
#include <limits>

static const char CLIENT_ADD = '1';
static const char CLIENT_REMOVE = '2';
static const char CLIENT_SHOW = '3';
static const char CLIENT_SUBMIT = '4';
static const char CLIENT_CHECK = '5';
static const char CLIENT_QUIT = '6';

// print menu options
void print_menu() {

  std::cout << "=========================================" << std::endl;
  std::cout << "=                  MENU                 =" << std::endl;
  std::cout << "=========================================" << std::endl;
  std::cout << " (1) Add to Cart" << std::endl;
  std::cout << " (2) Remove from Cart" << std::endl;
  std::cout << " (3) Show Cart" << std::endl;
  std::cout << " (4) Submit Order" << std::endl;
  std::cout << " (5) Check Order" << std::endl;
  std::cout << " (6) Quit"  << std::endl;
  std::cout << "=========================================" << std::endl;
  std::cout << "Enter number: ";
  std::cout.flush();

}


// add a song to remote server
void do_add(MusicLibraryApi &api) {

    std::string item, quantity;
  
    std::cout << std::endl << "Add to Cart" << std::endl;
    std::cout << "Possible Items: Broom, Cup, Banana, Hat" << std::endl;
    std::cout << "Please specify the item and the quantity that you would like to add" << std::endl;
    std::cout << "Item: ";
    std::getline(std::cin, item);
    
    while(!validItem(item)) {
        std::cout << "Item is not valid. Please specify a valid item" << std::endl;
        std::cout << "Item: ";
        std::getline(std::cin, item);
    }
    
    std::cout << "Quantity: ";
    std::getline(std::cin, quantity);

  // send message to server and wait for response
  CartItem cartitem(item, quantity);
  AddMessage msg(cartitem);
  if (api.sendMessage(msg)) {
    // get response
    std::unique_ptr<Message> msgr = api.recvMessage();
    AddResponseMessage& resp = (AddResponseMessage&)(*msgr);

    if (resp.status == MESSAGE_STATUS_OK) {
      std::cout << std::endl << "   \"" << item << "\" added successfully." << std::endl;
    } else {
      std::cout << std::endl << "   Adding \"" << item << "\" failed: " << resp.info << std::endl;
    }
  }

  std::cout << std::endl;
}

// remove song from server
void do_remove(MusicLibraryApi &api) {
  
    std::string item, quantity;
  
    std::cout << std::endl << "Remove from Cart" << std::endl;
    std::cout << "Possible Items: Broom, Cup, Banana, Hat" << std::endl;
    std::cout << "Please specify the item that you would like to remove" << std::endl;
    std::cout << "Item: ";
    std::getline(std::cin, item);
    
    while(!validItem(item)) {
        std::cout << "Item is not valid. Please specify a valid item" << std::endl;
        std::cout << "Item: ";
        std::getline(std::cin, item);
    }
  
  // send message to server and wait for response
  //Song song(artist, title);
  CartItem cartitem(item, "0");
  RemoveMessage msg(cartitem);
  if (api.sendMessage(msg)) {
    // get response
    std::unique_ptr<Message> msgr = api.recvMessage();
    RemoveResponseMessage& resp = (RemoveResponseMessage&)(*msgr);
    
    if (resp.status == MESSAGE_STATUS_OK) {
      std::cout << std::endl << "   \"" << item << "\" removed successfully." << std::endl;
    } else {
      std::cout << std::endl << "   Removing \"" << item << "\" failed: " << resp.info << std::endl;
    }
  }
}

// remove song from server
void do_show(MusicLibraryApi &api) {
  
  
  std::string item, quantity;
  
  // send message to server and wait for response
  //Song song(artist, title);
  ShowMessage msg;
  if (api.sendMessage(msg)) {
    // get response
    std::unique_ptr<Message> msgr = api.recvMessage();
    ShowResponseMessage& resp = (ShowResponseMessage&)(*msgr);
    
    if (resp.status == MESSAGE_STATUS_OK) {
      std::cout << "Cart contains:" << std::endl;
      for(auto &res : resp.results)
      {
        std::cout << res.item << ": " << res.quantity << std::endl;
      }
    } else {
      std::cout << std::endl << resp.info << std::endl;
    }
  }
}

// remove song from server
void do_submit(MusicLibraryApi &api) {
  
  // send message to server and wait for response
  //Song song(artist, title);
  SubmitMessage msg;
  if (api.sendMessage(msg)) {
    // get response
    std::unique_ptr<Message> msgr = api.recvMessage();
    SubmitResponseMessage& resp = (SubmitResponseMessage&)(*msgr);
    
    if (resp.status == MESSAGE_STATUS_OK) {
      std::cout << "Order submitted!" << std::endl;
      std::cout << resp.info << std::endl;
    } else {
      std::cout << std::endl << resp.info << std::endl;
    }
  }
}

// remove song from server
void do_check(MusicLibraryApi &api) {
  
  std::string orderId;
  
  std::cout << std::endl << "Check order by ID" << std::endl;
  std::cout << "Please specify the order Id" << std::endl;
  std::cout << "Id: ";
  std::getline(std::cin, orderId);
  
  // send message to server and wait for response
  //Song song(artist, title);
  CheckMessage msg(orderId);
  if (api.sendMessage(msg)) {
    // get response
    std::unique_ptr<Message> msgr = api.recvMessage();
    CheckResponseMessage& resp = (CheckResponseMessage&)(*msgr);
    
    if (resp.status == MESSAGE_STATUS_OK) {
      std::cout << "Order found!" << std::endl;
      std::cout << "Order Id: " << resp.result.orderId << std::endl;
      std::cout << "Unique items in order: " << resp.result.nitems << std::endl;
        std::cout << "Order Status: " << resp.result.status << std::endl;
      for(int i=0; i<resp.result.nitems; i++)
      {
          std::cout << "Item " << i+1 << ": " << resp.result.items[i].name << " Quantity: " << resp.result.quantity[i] << std::endl;
      }
    } else {
      std::cout << std::endl << resp.info << std::endl;
    }
  }
}

// search for songs on server
void do_goodbye(MusicLibraryApi &api) {
  GoodbyeMessage msg;
  if (api.sendMessage(msg)) {
    std::cout << "Goodbye." << std::endl;
  }
  std::cout << std::endl;
}


int main() {

  // start client
  cpen333::process::socket socket("localhost", MUSIC_LIBRARY_SERVER_PORT);
  
  std::cout << "Client connecting...";
  std::cout.flush();

  // if we open the socket successfully, continue
  if (socket.open()) {
    std::cout << "connected." << std::endl;

    // create API handler
    JsonMusicLibraryApi api(std::move(socket));

    // keep reading commands until the user quits
    char cmd = 0;
    while (cmd != CLIENT_QUIT) {
      print_menu();

      // get menu entry
      std::cin >> cmd;
      // gobble newline
      std::cin.ignore (std::numeric_limits<std::streamsize>::max(), '\n');

      switch(cmd) {
        case CLIENT_ADD:
          do_add(api);
          break;
        case CLIENT_REMOVE:
          do_remove(api);
          break;
        case CLIENT_SHOW:
          do_show(api);
          break;
        case CLIENT_SUBMIT:
          do_submit(api);
          break;
        case CLIENT_CHECK:
          do_check(api);
          break;
        case CLIENT_QUIT:
          do_goodbye(api);
          break;
        default:
          std::cout << "Invalid command number " << cmd << std::endl << std::endl;
      }

      cpen333::pause();
    }
  } else {
    std::cout << "failed." << std::endl;
  }

  return 0;
}


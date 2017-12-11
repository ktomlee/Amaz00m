/**
 * @file
 *
 * This is the main server process.  When it starts it listens for clients.  It then
 * accepts remote commands for modifying/viewing the music database.
 *
 */

#include <fstream>
#include <iostream>
#include <string>
#include <thread>
#include <memory>
#include <mutex>

#include "MusicLibrary.h"
#include "warehouse_common.h"
#include "JsonWarehouseApi.h"
#include "CartItem.h"

#include <cpen333/process/socket.h>
#include <cpen333/process/mutex.h>
#include <cpen333/process/shared_memory.h>

cpen333::process::mutex mutex(SERVER_MUTEX);

/**
 * Main thread function for handling communication with a single remote
 * client.
 *
 * @param lib shared library
 * @param api communication interface layer
 * @param id client id for printing messages to the console
 */
void service(Cart &cart, MusicLibraryApi &&api, int id, int &numActiveClients) {
  std::cout << "Client " << id << " connected" << std::endl;
  
  // receive message
  std::unique_ptr<Message> msg = api.recvMessage();

  // continue while we don't have an error
  while (msg != nullptr) {

    // react and respond to message
    MessageType type = msg->type();
    switch (type) {
      case MessageType::ADD: {
        // process "add" message
        // get reference to ADD
        AddMessage &addm = (AddMessage &) (*msg);
        std::cout << "Client " << id << " adding item: " << addm.cartItem.item << " quantity: " << addm.cartItem.quantity << std::endl;

        // add song to library
        bool success = false;
        mutex.lock();
        success = cart.add(addm.cartItem);
        mutex.unlock();

        // send response
        if (success) {
          api.sendMessage(AddResponseMessage(addm, MESSAGE_STATUS_OK));
        } else {
          api.sendMessage(AddResponseMessage(addm,
            MESSAGE_STATUS_ERROR,
            "Item already exists in cart"));
        }
        break;
      }
        
      case MessageType::REMOVE: {
        //====================================================
        // Implement "remove" functionality
        //====================================================
        RemoveMessage &rmv = (RemoveMessage &) (*msg);
        std::cout << "Client " << id << " removing item: " << rmv.cartItem.item << std::endl;
        
        // remove item from cart
        bool success = false;
        mutex.lock();
        success = cart.remove(rmv.cartItem);
        mutex.unlock();
        
        // send response
        if(success)
        {
          api.sendMessage(RemoveResponseMessage(rmv, MESSAGE_STATUS_OK));
        }
        else
        {
          api.sendMessage(RemoveResponseMessage(rmv,
                                                MESSAGE_STATUS_ERROR,
                                                "Item not removed from cart"));
        }
        break;
      }
    
        case MessageType::SHOW: {
            ShowMessage &show = (ShowMessage &) (*msg);
            
            mutex.lock();
          std::set<CartItem> results = cart.show();
            mutex.unlock();
            
          
            if(!results.empty()) {
                api.sendMessage(ShowResponseMessage(show, results, MESSAGE_STATUS_OK));
            }
            else {
                api.sendMessage(ShowResponseMessage(show, results, MESSAGE_STATUS_ERROR, "Cart is empty"));
            }
          
          break;
        }
        
      case MessageType::SUBMIT: {
        SubmitMessage &sub = (SubmitMessage &) (*msg);
        
        mutex.lock();
        int orderid = cart.submit();
        mutex.unlock();
        
        
        if(orderid != 0) {
          api.sendMessage(SubmitResponseMessage(sub, MESSAGE_STATUS_OK, "Order id: " + std::to_string(orderid)));
        }
        else {
          api.sendMessage(SubmitResponseMessage(sub, MESSAGE_STATUS_ERROR, "Cart is empty"));
        }
        
        break;
      }
        
        
      case MessageType::CHECK: {
        CheckMessage &chk = (CheckMessage &) (*msg);
        
        mutex.lock();
        Order result = cart.check(std::stoi(chk.orderId));
        mutex.unlock();
        
        
        if(result.orderId != 0) {
          api.sendMessage(CheckResponseMessage(chk, MESSAGE_STATUS_OK, result));
        }
        else {
          api.sendMessage(CheckResponseMessage(chk, MESSAGE_STATUS_ERROR, result, "Order does not exist"));
        }
        
        break;
      }
        
            /*
      case MessageType::SEARCH: {
        // process "search" message
        // get reference to SEARCH
        SearchMessage &search = (SearchMessage &) (*msg);

        std::cout << "Client " << id << " searching for: "
                    << search.artist_regex << " - " << search.title_regex << std::endl;

        // search library
        std::vector<Song> results;
        mutex.lock();
        results = lib.find(search.artist_regex, search.title_regex);
        mutex.unlock();

        // send response
        api.sendMessage(SearchResponseMessage(search, results, MESSAGE_STATUS_OK));

        break;
      }
             */
      case MessageType::GOODBYE: {
        // process "goodbye" message
        std::cout << "Client " << id << " closing" << std::endl;
        numActiveClients--;
        return;
      }
      default: {
        std::cout << "Client " << id << " sent invalid message" << std::endl;
      }
         
    }

    // receive next message
    msg = api.recvMessage();
  }
}

/**
 * Load songs from a JSON file and add them to the music library
 * @param lib music library
 * @param filename file to load
 */
void load_songs(MusicLibrary &lib, const std::string& filename) {
/*
  // parse from file stream
  std::ifstream fin(filename);
  if (fin.is_open()) {
    JSON j;
    fin >> j;
    std::vector<Song> songs = JsonConverter::parseSongs(j);
    lib.add(songs);
  } else {
    std::cerr << "Failed to open file: " << filename << std::endl;
  }
*/
}

int main() {


  //MusicLibrary lib;       // main shared music library

  // start server
  cpen333::process::socket_server server(MUSIC_LIBRARY_SERVER_PORT);
  server.open();
  std::cout << "Server started on port " << server.port() << std::endl;

  cpen333::process::socket client;
  int shouldcontinue = true;
  int numActiveClients = 0;
    
  
  int clientId = 0;
  while(shouldcontinue)
  {
    if (server.accept(client)) {
      // create API handler
        JsonMusicLibraryApi api(std::move(client));
        Cart cart;
      // service client-server communication
        std::thread t(service, std::ref(cart), std::move(api), clientId++, std::ref(numActiveClients));
      numActiveClients++;
 
      t.detach();
    }
    
    //std::cout << numActiveClients << std::endl;
    /*
    if(numActiveClients == 0)
    {
      std::cout << "No active users." << std::endl;
      std::cout << "Continue? (1) or Quit? (0)";
      std::cin >> shouldcontinue;
    }
     */
  }

  // close server
  server.close();

  return 0;
}

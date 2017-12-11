#ifndef LAB6_CIRCULARORDERQUEUE_H
#define LAB6_CIRCULARORDERQUEUE_H

#include "OrderQueue.h"
#include <cpen333/thread/semaphore.h>
#include <mutex>

#define CIRCULAR_BUFF_SIZE 8

/**
 * Queue implementation using a circular buffer
 * (i.e. a fixed-size queue)
 */
class CircularOrderQueue : public virtual OrderQueue {
  Order buff_[CIRCULAR_BUFF_SIZE];
  cpen333::thread::semaphore producer_;
  cpen333::thread::semaphore consumer_;
  std::mutex pmutex_;
  std::mutex cmutex_;
  size_t pidx_;
  size_t cidx_;


 public:
  /**
   * Creates a queue with provided circular buffer size
   * @param buffsize size of circular buffer
   */
  CircularOrderQueue() :
      buff_(),
      producer_(CIRCULAR_BUFF_SIZE), consumer_(0),
      pmutex_(), cmutex_(), pidx_(0), cidx_(0){}

  void add(const Order& order) {

    //==================================================
    // Safely add item to "queue"
    //    - wait for empty slot
    //    - safely acquire and increment producer index
    //    - fill slot
    //    - notify others of item availability
    //==================================================
    
    // wait for empty slot
    producer_.wait();
    
    // safely acquire and increment producer index
    pmutex_.lock();
    int pidx;
    pidx = pidx_;
    pidx_ = (pidx_+1)%CIRCULAR_BUFF_SIZE;
    
    // fill slot
    buff_[pidx] = order;
    
    // notify others of item availability
    pmutex_.unlock();
    consumer_.notify();
  }

  Order get() {

    //==================================================
    // Safely remove item from "queue"
    //    - wait for next filled slot
    //    - safely acquire and increment consumer index
    //    - remove item from slot
    //    - notify others of slot availability
    //==================================================

    // wait for next filled slot
    consumer_.wait();
    
    // safely acquire and increment consumer index
    int cidx;
    cidx = cidx_;
    cidx_ = (cidx_+1)%CIRCULAR_BUFF_SIZE;
    
    // remove item from slot
    Order out = buff_[cidx];
    
    // notify others of slot avaliablitity
    cmutex_.unlock();
    producer_.notify();
    
    return out;
  }

};

#endif //LAB6_CIRCULARORDERQUEUE_H

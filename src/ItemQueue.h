#ifndef ITEM_QUEUE_H
#define ITEM_QUEUE_H

#include <cpen333/thread/semaphore.h>
#include "warehouse_common.h"
#include <mutex>
#include <cpen333/process/mutex.h>
#include <cpen333/process/shared_memory.h>
#include <cpen333/process/subprocess.h>

#define ITEM_BUFF_SIZE RECEIVINGQ_SIZE

/**
 * Queue implementation using a circular buffer
 * (i.e. a fixed-size queue)
 */
class ItemQueue {
    Item buff[ITEM_BUFF_SIZE];
    cpen333::thread::semaphore producer;
    cpen333::thread::semaphore consumer;
    std::mutex pmutex;
    std::mutex cmutex;
    size_t pidx_;
    size_t cidx_;
    
    
public:
    /**
     * Creates a queue with provided circular buffer size
     * @param buffsize size of circular buffer
     */
    ItemQueue() :
    buff(),
    producer(ITEM_BUFF_SIZE), consumer(0),
    pmutex(), cmutex(), pidx_(0), cidx_(0){}
    
    void add(const Item& item) {
        
        int pidx;
        pidx = pidx_;
        // update producer index
        producer.wait();
        
        pidx_ = (pidx_+1)%ITEM_BUFF_SIZE;
        pmutex.lock();
        buff[pidx] = item;
        pmutex.unlock();
        
        consumer.notify();
        
    }
    
    Item get() {
        
        int cidx;
        cidx = cidx_;
        
        // update consumer index
        consumer.wait();
        
        cidx_ = (cidx_+1)%ITEM_BUFF_SIZE;
        cmutex.lock();
        Item out = buff[cidx];
        cmutex.unlock();
        
        producer.notify();
        
        return out;
    }
    
};

#endif 

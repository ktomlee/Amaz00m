#ifndef ITEM_QUEUE_H
#define ITEM_QUEUE_H

#include <cpen333/thread/semaphore.h>
#include "warehouse_common.h"
#include <mutex>
#include <cpen333/process/mutex.h>
#include <cpen333/process/shared_memory.h>
#include <cpen333/process/subprocess.h>

#define ITEM_BUFF_SIZE 4096

/**
 * Queue implementation using a circular buffer
 * (i.e. a fixed-size queue)
 */
class ItemQueue {
    std::pair<Item, int> buff[ITEM_BUFF_SIZE];
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
    
    void add(std::pair<Item, int> to_add) {
        
        int pidx;
        pidx = pidx_;
        // update producer index
        producer.wait();
        
        pidx_ = (pidx_+1)%ITEM_BUFF_SIZE;
        pmutex.lock();
        buff[pidx].first = to_add.first;
        buff[pidx].second = to_add.second;
        pmutex.unlock();
        
        consumer.notify();
        
    }
    
    std::pair<Item, int> get() {
        
        int cidx;
        cidx = cidx_;
        
        // update consumer index
        consumer.wait();
        
        cidx_ = (cidx_+1)%ITEM_BUFF_SIZE;
        cmutex.lock();
        std::pair<Item, int> out;
        out.first = buff[cidx].first;
        out.second = buff[cidx].second;
        cmutex.unlock();
        
        producer.notify();
        
        return out;
    }
    
};

#endif 

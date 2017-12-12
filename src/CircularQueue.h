#ifndef CIRCULAR_QUEUE_H
#define CIRCULAR_QUEUE_H

#include <cpen333/thread/semaphore.h>
#include "warehouse_common.h"
#include <mutex>
#include <cpen333/process/mutex.h>
#include <cpen333/process/shared_memory.h>
#include <cpen333/process/subprocess.h>

#define CIRCULAR_BUFF_SIZE SHIPPINGQ_SIZE


/**
 * Queue implementation using a circular buffer
 * (i.e. a fixed-size queue)
 */
class CircularOrderQueue {
    Order buff[CIRCULAR_BUFF_SIZE];
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
    CircularOrderQueue() :
    buff(),
    producer(CIRCULAR_BUFF_SIZE), consumer(0),
    pmutex(), cmutex(), pidx_(0), cidx_(0) {
        //Test
        consumer.notify();
        Order order;
        order.items[0].name = "Turkey";
        order.quantity[0] = 4;
        buff[0] = order;
    }
    
    void add(const Order& order) {
        
        int pidx;
        pidx = pidx_;
        // update producer index
        producer.wait();
        
        pidx_ = (pidx_+1)%CIRCULAR_BUFF_SIZE;
        pmutex.lock();
        buff[pidx] = order;
        pmutex.unlock();
        
        consumer.notify();
        
    }
    
    Order get() {
        
        int cidx;
        cidx = cidx_;
        
        // update consumer index
        consumer.wait();
        
        cidx_ = (cidx_+1)%CIRCULAR_BUFF_SIZE;
        cmutex.lock();
        Order out = buff[cidx];
        cmutex.unlock();
        
        producer.notify();
        
        return out;
    }
    
};

#endif //LAB6_CIRCULARORDERQUEUE_H

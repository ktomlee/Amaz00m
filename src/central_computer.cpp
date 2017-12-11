#include warehouse_common.h

#include <cpen333/process/socket.h>
#include <cpen333/process/mutex.h>
#include <cpen333/process/shared_memory.h>

bool canFulfill()
{
  
  
}

void addToProgressQ()
{
  cpen333::process::shared_object<SharedData> memory(WAREHOUSE_MEMORY_NAME);
  cpen333::process::mutex mutex(MUSICLIBMUTEXNAME);

  mutex.lock();
  for( )
  
}

int main()
{
  while(1)
  {
    
    
  }
  
}

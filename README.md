Simple header only c++ template class for expiration cache. 
put your key type, value type and TTL for current instance and you are ready. 
lib is thread safe, and use one thread for all instances across the application


usage:

```c++
#include <iostream>
#include "ExpirationCache.h"
int main ()
{
  ExpirationCache<int/*keyType*/, int/*value type*/, 4/*TTL*/> e1;
  e1.Put(1,2);
  e1.Put(2,3);
  try
  {
    auto val2 = e1.Get(33);
  }
  catch(std::out_of_range&e)
  {
    //value was not found in cache
    std::cout<<e.what()<<std::endl;
  }
  return 0;
}
```

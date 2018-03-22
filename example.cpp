#include <iostream>
#include "ExpirationCache.h"
int main ()
{
ExpirationCache<int, int, 4> e1;
ExpirationCache<int, int, 6> e2;
e1.Put(1,1);
e2.Put(1,1);
std::cout<<e1.Count()<<" "<<e2.Count()<<std::endl;
std::this_thread::sleep_for(std::chrono::seconds(3));
std::cout<<e1.Count()<<" "<<e2.Count()<<std::endl;
std::this_thread::sleep_for(std::chrono::seconds(7));
std::cout<<e1.Count()<<" "<<e2.Count()<<std::endl;
try
{
	e1.Get(33);
}
catch(std::out_of_range&e)
{
	std::cout<<e.what()<<std::endl;
}
return 0;
}

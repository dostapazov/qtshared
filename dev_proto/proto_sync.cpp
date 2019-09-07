#include "dev_proto.hpp"
#include <algorithm>

namespace devproto {

/**
 * @brief sync_with synchronize data with protocol defined synhro bytes set
 * @param data
 * @param ptr
 * @param sz
 * @return true if sync success
 */

bool   sync_with(storage_t & data, const void * ptr, size_t sz )
{
   while(ptr && sz && data.size()>sz)
   {
     storage_t::iterator   bptr = data.begin();
     storage_t::iterator   eptr = data.end  ();
     storage_t::iterator   fptr = std::find( bptr, eptr, reinterpret_cast<const char*>(ptr)[0]);
     if(fptr<eptr)
     {
      storage_t::size_type  distance = storage_t::size_type(std::distance(bptr,fptr));
      storage_t::pointer    temp_ptr = &data.at(distance);
      if(memcmp(temp_ptr, ptr, sz))
      {
        std::advance(fptr,sz);
        data.erase  (bptr,fptr);
      }
      else
      {
       if(distance)
          data.erase(bptr,fptr);
       return true;
      }

     }
     else
      data.clear();
   }

 return   sz && ptr ? false : true;
}

}// end of namespace devproto


/*  NIKTES Ostapenko D.V.
 *
 * Общая обработка протоколов вида {  <синронизация>, [заголовок ,размер данных] , данные , контрольная сумма  }
 *  ver 1.0 2019-02-28
 * Недостаток. Для протокола где синхронизация отсутствует
 */

#ifdef __cplusplus
#ifndef DEV_PROTO_HPP_
#define DEV_PROTO_HPP_

#include <stdint.h>
#include <memory.h>
#include <vector>

namespace devproto {

using    storage_t = std::vector<char>;
bool     sync_with(storage_t & data, const void * ptr, size_t sz );

#pragma pack(push,1)

template <typename _prolog_type, typename _header_type, typename _data_type = uint8_t>

struct t_hdr
{
    enum {ANYSIZE_ARRAY_T = 1};
    typedef  _prolog_type  prolog_type;
    typedef  _header_type  header_type;

    _prolog_type  prolog;
    _header_type  proto_hdr;
    _data_type    data[ANYSIZE_ARRAY_T];

    const char *  operator  ()()     {return reinterpret_cast<const char*>(this);}
    /*Вычисление размера протокола*/
          size_t  size      () const {return sizeof(*this) - sizeof(this->data[0]) + proto_hdr();}

          size_t  init        (_prolog_type prolog, _header_type hdr, size_t data_size = 0 , const void * data = nullptr)
          {this->prolog = prolog;this->proto_hdr = hdr;  proto_hdr(data_size) ;if(data && data_size){ memcpy(this->data,data,data_size);} return size();}

          size_t  init        (_header_type hdr, size_t data_size = 0 , const void * data = nullptr)
          {return init(_prolog_type(), hdr, data_size, data);}

          size_t  init        (size_t data_size = 0 , const void * data = nullptr)
          {return init(_prolog_type(), _header_type(), data_size, data);}

          template<typename T>
          T *         as(){return reinterpret_cast<T*>(data);}
          template<typename T>
          const T *   as()const {return reinterpret_cast<const T*>(data);}


          const char * last_byte() const {return reinterpret_cast<const char*>(data)+proto_hdr();}
                char * last_byte()       {return reinterpret_cast<char*>(data)+proto_hdr();}
          template< typename T>
                T *    last_byte_as(){return reinterpret_cast<T*>(last_byte());}
          template< typename T>
          const T *    last_byte_as() const {return reinterpret_cast<const T*>(last_byte());}

   /*Вычисление размера*/
   static size_t need_size(size_t sz){ return sizeof (t_hdr) - sizeof(t_hdr::data) + sz;}

   /*Разметка буфера под протокол*/
   static size_t  init    (void * buff, size_t buf_sz, _prolog_type prolog, _header_type hdr, size_t data_size = 0 , const void * data = nullptr)
   {return buff && need_size(data_size)<=buf_sz ? (reinterpret_cast<t_hdr*>(buff))->init(prolog, hdr, data_size, data) : 0; }

   static size_t  init    (void * buff, size_t buf_sz, _header_type hdr, size_t data_size = 0 , const void * data = nullptr)
   {return buff && need_size(data_size)<=buf_sz ? (reinterpret_cast<t_hdr*>(buff))->init(hdr, data_size, data) : 0; }

   static size_t  init    (void * buff, size_t buf_sz, size_t data_size = 0 , const void * data = nullptr)
   {return buff && need_size(size_t(data_size))<=buf_sz ? (reinterpret_cast<t_hdr*>(buff)->init( data_size, data)) : 0; }

};

using empty_struct =  struct  _empty_struct{} ;

using  cmd_hdr_t   = t_hdr<uint8_t,uint8_t,uint8_t>;
using  lpcmd_hdr_t = cmd_hdr_t*;

#pragma pack(pop)

template <typename proto_hdr,
#ifndef PROTOCOL_PT_LINE
          typename crc_type_t = uint32_t>
#else
          typename crc_type_t = uint8_t>
#endif
class proto_buffer 
{
 
  public:
    typedef proto_hdr    header_type;
    typedef crc_type_t   crc_type;
    proto_buffer(size_t sz = 256) {m_storage.reserve(sz);}
    void     clear    (){m_storage.clear();}
    size_t   raw_size () const {return m_storage.size(); }
    size_t   raw_add  (const void * ptr, size_t sz);
    size_t   is_proto_complete();
    bool     remove_first     ();
    bool     is_empty         ();
    size_t   raw_size         ();

    bool     sync(const void * ptr, size_t sz);

    template <typename _SYNC_DATA>
    bool     sync(const _SYNC_DATA & dt);

    const   proto_hdr * header_from_offset(size_t offset = 0) const  {return reinterpret_cast<const proto_hdr *> (&m_storage.at(offset));}
            proto_hdr * header_from_offset(size_t offset = 0)        {return reinterpret_cast<proto_hdr *> (&m_storage.at(offset));}
    const   proto_hdr * operator()(size_t offset = 0) const  {return header_from_offset(offset);}
            proto_hdr * operator()(size_t offset = 0)        {return header_from_offset(offset);}

    template<typename  _size_type>
    static  _size_type proto_size(const proto_hdr * phdr,bool inc_crc = true)
    {return _size_type(phdr ? phdr->size() +  (inc_crc ? sizeof (crc_type) : 0 ) : size_t(0));}
    int     count();
protected:
    size_t   __is_proto_complete(size_t number);
	storage_t m_storage;
    size_t    m_min_sz = sizeof (proto_hdr)-sizeof (proto_hdr::data[0])+sizeof(crc_type_t);
};


template <typename proto_hdr, typename crc_type_t>
size_t  proto_buffer<proto_hdr, crc_type_t>::raw_add (const void * ptr, size_t sz)
{
  if(ptr && sz)
  {
    size_t prev_sz = raw_size();
    m_storage.resize(prev_sz + sz);
    memcpy(&m_storage.at(prev_sz), ptr, sz);
  }
 return raw_size();
}


template <typename proto_hdr, typename crc_type_t>
size_t     proto_buffer<proto_hdr, crc_type_t>::is_proto_complete()
{
   if(raw_size()>= m_min_sz)
   {
     const proto_hdr * phdr = (*this)();
     size_t proto_size =  phdr->size()+sizeof(crc_type_t);
     return proto_size <= raw_size() ? proto_size : 0 ;
   }
  return 0;
}

template <typename proto_hdr, typename crc_type_t>
size_t   proto_buffer<proto_hdr, crc_type_t>::__is_proto_complete(size_t number)
{
  size_t offset = 0;
  size_t sz = raw_size();
  int i = 0;
  while( sz >= m_min_sz )
  {
     const proto_hdr * phdr = (*this)(offset);
     size_t proto_size =  phdr->size()+sizeof(crc_type_t);
     if(proto_size <= sz)
       {
         offset+=proto_size;
         sz-= proto_size;
         if(i == number) return proto_size;
         ++i;
       }
     else break;
  }
 return 0;
}


template <typename proto_hdr, typename crc_type_t>
bool     proto_buffer<proto_hdr, crc_type_t>::remove_first()
{
  size_t proto_size = is_proto_complete();
    if(proto_size)
    {
      storage_t::iterator beg = m_storage.begin();
      storage_t::iterator end = beg;
      std::advance(end, proto_size);
      m_storage.erase(beg, end);
      return true;
    }
  return false;
}

// Count valid proto into buffer
template <typename proto_hdr, typename crc_type_t>
int     proto_buffer<proto_hdr, crc_type_t>::count()
{
   int ret = 0;
   size_t offset = 0;
   size_t rsz    = raw_size();

   while(offset < rsz && rsz-offset >= m_min_sz)
   {
     const proto_hdr * phdr = m_storage.at(offset);
     if(is_proto_complete())
     {
      ++ret;
      offset += proto_size(phdr, true);
     }
     else
     break;
   }
 return ret;
}

template <typename proto_hdr, typename crc_type_t>
inline bool     proto_buffer<proto_hdr, crc_type_t>::is_empty()
{
  return   !m_storage.size();
}

template <typename proto_hdr, typename crc_type_t>
inline size_t     proto_buffer<proto_hdr, crc_type_t>::raw_size()
{
  return   m_storage.size();
}


template <typename proto_hdr, typename crc_type_t>
inline bool     proto_buffer<proto_hdr, crc_type_t>::sync(const void * ptr, size_t sz)
{
    return sync_with(m_storage, ptr, sz);
}


template <typename proto_hdr, typename crc_type_t>
template <typename _SYNC_DATA>
inline bool     proto_buffer<proto_hdr, crc_type_t>::sync(const _SYNC_DATA & dt)
{
 return sync_with(m_storage, &dt, sizeof(_SYNC_DATA));
}


} //end of namespace devproto

#endif
#else
#error "dev_proto must compile with C++ compiler"
#endif

/* Ostapenko D. V.
* NIKTES 2019-04-02
* Signal block / unblock helper
*/

#ifndef SIGNAL_BLOKER_HPP
#define SIGNAL_BLOKER_HPP

#include <qobject.h>

template<typename T>
void block_signals(const QList<T*> & list , bool block)
{
   for(auto v : list)
   {
       v->blockSignals(block);
   }
}


class SignalBlocker
{
  public:
    SignalBlocker() = default;
    template <typename T>
    SignalBlocker(const QList<T*> & list){block(list);}
    ~SignalBlocker(){ unblock(); }
  template <typename T>
  void block(const QList<T*> & list);
  template <typename T>
  void block(T* obj);
  template <typename T>
  void operator += (T* obj){block(obj);}
  template <typename T>
  void operator += (const QList<T*> & list){block(list);}
  void unblock();
  private:
      QList<QObject*> objects;
};

template <typename ChildType>
class ChildrenSignalBlocker:public SignalBlocker
{
public:
     ChildrenSignalBlocker(QObject * parent)
     {
      if(parent)
         block(parent->findChildren<ChildType*>());
     }
};


template <typename T>
void SignalBlocker::block(const QList<T*> &list)
{
  objects.reserve(objects.size()+list.size());
  for(auto w : list)
  {
    w->blockSignals(true);
    objects.append(w);
  }
}

template <typename T>
void SignalBlocker::block( T* obj)
{
  obj->blockSignals(true);
  objects.append(obj);
}


inline void SignalBlocker::unblock()
{
   block_signals(objects,false);
   objects.clear();
}

#endif // SIGNAL_BLOKER_HPP

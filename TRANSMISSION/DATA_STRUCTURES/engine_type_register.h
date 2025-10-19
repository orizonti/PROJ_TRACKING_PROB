#ifndef TYPE_REGISTER_ENGINE_H
#define TYPE_REGISTER_ENGINE_H
#include <QString>
#include <QDebug>
#include <typeinfo>

template<typename T = void*> 
class TypeRegister
{

public:
      static size_t  GetTypeID() { return TYPE_ID;}
      static size_t  GetTypeMax() { return TypeRegister<>::TypeIDMax;}
      static size_t  GetTypeCount() { return TypeCount;}
      static QString GetTypeName()  { return typeid(T).name(); }

      static bool isTypeRegistered()         { return ID() > 0;}
      static bool isTypeValid(const int& TYPE_ID) { return TYPE_ID <= TypeRegister<>::TypeIDMax;}

      constexpr static int ID() { return 0; };

      static constinit const int TYPE_ID;

      static int RegisterType()  
      { 
            TypeRegister<>::TypeCount++; 
            TypeRegister<>::TypeIDMax = (TYPE_ID > TypeRegister<>::TypeIDMax) ? TYPE_ID : TypeRegister<>::TypeIDMax;
            TypeRegister<>::SetTypeSize(sizeof(T));
            return TYPE_ID;
      }

      static size_t GetTypeSize()    {return sizeof(T);}
      static size_t GetMinTypeSize() {return TypeSizeMin;}
      static size_t GetMaxTypeSize() {return TypeSizeMax;}

      static void SetTypeSize(int size) 
      { 
            if(TypeSizeMin > size) TypeSizeMin = size; 
            if(TypeSizeMax < size) TypeSizeMax = size; 
      };

      static uint8_t TypeCount;
      static uint16_t TypeIDMax;
      static size_t TypeSizeMin;
      static size_t TypeSizeMax;
};

template<typename T> uint8_t TypeRegister<T>::TypeCount = 0;
template<typename T> uint16_t TypeRegister<T>::TypeIDMax = 0;
template<typename T> size_t TypeRegister<T>::TypeSizeMin = 60000;
template<typename T> size_t TypeRegister<T>::TypeSizeMax = 0;
template<typename T> constinit const int TypeRegister<T>::TYPE_ID{TypeRegister<T>::ID()} ;


#endif //TYPE_REGISTER_ENGINE_H
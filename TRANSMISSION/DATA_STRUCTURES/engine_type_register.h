#ifndef TYPE_REGISTER_enGINE_H
#define TYPE_REGISTER_enGINE_H
#include <QString>
#include <QDebug>
#include "message_command_structures.h"

template<typename T> class TypeRegister;

class TypeTagRegister
{
//utilite none template class to stroe TypesID map
public:
      template<typename TT> class TYPE_TAG{};
      template<typename TT>     static int GetTypeID(TYPE_TAG<TT>) { return TypeRegister<TT>::GetTypeID();}
      template<typename TT> static QString GetTypename(TYPE_TAG<TT>) { return TypeRegister<TT>::GetTypename(); }

                            static QString GetTypename(const int& ID) {if(isTypeValid(ID)) return TypesID[ID]; 
                                                                                  return "ANVALID_TYPE"; }
      template<typename TT> 
      static bool isTypeValid(const TYPE_TAG<TT>& TAG) { return TypesID.contains(TypeRegister<TT>::GetTypeID());}
      static bool isTypeValid(const int& ID)            { return TypesID.contains(ID);}

      static size_t GetTypeCount() { return TypesID.size(); }

      template<typename TT> static size_t GetTypeSize(const TYPE_TAG<TT>& TAG) { return TypesSizes[GetTypeID(TAG)];};
      static size_t GetMinTypeSize() { return MinTypeSize;};
      static size_t GetMaxTypeSize() { return MaxTypeSize;};

      template<typename TT> static void RegisterType(const TYPE_TAG<TT>& TAG) 
      { 
        TypesID[GetTypeID(TAG)] = GetTypename(TAG);
        TypesSizes[GetTypeID(TAG)] = sizeof(TT);

        if(sizeof(TT) < MinTypeSize || MinTypeSize == 0) MinTypeSize = sizeof(TT);
        if(sizeof(TT) > MaxTypeSize || MaxTypeSize == 0) MaxTypeSize = sizeof(TT);
        qDebug() << "REGISTER TYPE: " << GetTypeID(TAG) << "NAME: " << GetTypename(TAG) << "SIZE: " << sizeof(TT);
      }

      static std::map<int,QString> TypesID;
      static std::map<int,int> TypesSizes;
      static int MaxTypeSize;
      static int MinTypeSize;
};


template<typename T = void*> 
class TypeRegister
{

public:
      static int     GetTypeID()    { return 0;}
      static size_t  GetTypeCount() { return TypeTagRegister::GetTypeCount();}

      //static QString GetTypename()  { return QString(typeid(T).name()).split(" ").at(1); }
      static QString GetTypename()  { return QString(typeid(T).name()); }
      static QString GetTypename(const int& ID)  { return TypeTagRegister::GetTypename(ID); }

      static bool isTypeValid()       { return TypeTagRegister::isTypeValid(TypeTagRegister::TYPE_TAG<T>());}
      static bool isTypeValid(const int& ID) { return TypeTagRegister::isTypeValid(ID);}
      static void RegisterType()  { TypeTagRegister::RegisterType(TypeTagRegister::TYPE_TAG<T>()); }

      static int GetTypeSize() { return TypeTagRegister::GetTypeSize(TypeTagRegister::TYPE_TAG<T>()); }
      static int GetMinTypeSize() {return TypeTagRegister::GetMinTypeSize();}
      static int GetMaxTypeSize() {return TypeTagRegister::GetMaxTypeSize();}
};

template<> int TypeRegister<ControlMessage1>::GetTypeID(); 
template<> int TypeRegister<ControlMessage2>::GetTypeID(); 
template<> int TypeRegister<ControlMessage3>::GetTypeID(); 
template<> int TypeRegister<ControlMessage4>::GetTypeID(); 
template<> int TypeRegister<ControlMessage5>::GetTypeID(); 
template<> int TypeRegister<ControlMessage6>::GetTypeID(); 
#endif //TYPE_REGISTER_enGINE_H
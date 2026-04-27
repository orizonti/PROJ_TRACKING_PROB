#pragma once
#include <memory>
#include <vector>


template<typename V = double>
class PassValueClass
{
public:
	std::vector<PassValueClass*> NodesLinked;
	bool isLinked() { return !NodesLinked.empty();}
	V Value;

	virtual const V& getValue() { return Value;};
	virtual void setValue(V InputValue) { Value = InputValue; };

    void setLink(PassValueClass* NewLink) { NodesLinked.push_back(NewLink);}

	void passValue() { if(!isLinked()) return; for(auto& Link: NodesLinked) *this >> *Link; }
	
	friend PassValueClass& operator >>(               V Value, PassValueClass& Reciever)
	{ Reciever.setValue(Value); if(Reciever.isLinked()) Reciever.passValue(); return Reciever; };

	friend PassValueClass& operator >>(               int Value, PassValueClass& Reciever)
	{ Reciever.setValue((int)Value); if(Reciever.isLinked()) Reciever.passValue(); return Reciever; };

	friend PassValueClass& operator >>(               uint32_t Value, PassValueClass& Reciever)
	{ Reciever.setValue((uint32_t)Value); if(Reciever.isLinked()) Reciever.passValue(); return Reciever; };


    friend PassValueClass& operator >>(PassValueClass& Sender, PassValueClass& Reciever)
	{ Reciever.setValue(Sender.getValue()); if(Reciever.isLinked()) Reciever.passValue(); return Reciever; }

	friend PassValueClass& operator >>(PassValueClass& Sender,  V& Value)
	{ Value = Sender.getValue(); return Sender; }

	friend PassValueClass& operator >>(PassValueClass& Sender,  int& Value)
	{ Value = Sender.getValue(); return Sender; }

	friend PassValueClass& operator >>(PassValueClass& Sender,  uint32_t& Value)
	{ Value = Sender.getValue(); return Sender; }

    friend PassValueClass& operator | (PassValueClass& Sender, PassValueClass& Reciever)
	{ Sender.setLink(&Reciever); return Reciever; }

    friend std::shared_ptr<PassValueClass>  operator | (std::shared_ptr<PassValueClass> Sender, 
														std::shared_ptr<PassValueClass> Reciever)
	{ Sender->setLink(Reciever.get()); return Reciever; }

    friend PassValueClass& operator | (std::shared_ptr<PassValueClass> Sender, PassValueClass& Reciever)
	{ Sender->setLink(&Reciever); return Reciever; }
};

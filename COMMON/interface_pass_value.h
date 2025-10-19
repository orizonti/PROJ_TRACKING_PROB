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

	virtual const V& GetValue() { return Value;};
	virtual void SetValue(V InputValue) { Value = InputValue; };

    void setLink(PassValueClass* NewLink) { NodesLinked.push_back(NewLink);}

	void passCoord() { if(!isLinked()) return; for(auto& Link: NodesLinked) *this >> *Link; }
	
	friend PassValueClass& operator >>(               V Value, PassValueClass& Reciever)
	{ Reciever.SetValue(Value); if(Reciever.isLinked()) Reciever.passCoord(); return Reciever; };

	friend PassValueClass& operator >>(               int Value, PassValueClass& Reciever)
	{ Reciever.SetValue((int)Value); if(Reciever.isLinked()) Reciever.passCoord(); return Reciever; };

	friend PassValueClass& operator >>(               uint32_t Value, PassValueClass& Reciever)
	{ Reciever.SetValue((uint32_t)Value); if(Reciever.isLinked()) Reciever.passCoord(); return Reciever; };


    friend PassValueClass& operator >>(PassValueClass& Sender, PassValueClass& Reciever)
	{ Reciever.SetValue(Sender.GetValue()); if(Reciever.isLinked()) Reciever.passCoord(); return Reciever; }

	friend PassValueClass& operator >>(PassValueClass& Sender,  V& Value)
	{ Value = Sender.GetValue(); return Sender; }

	friend PassValueClass& operator >>(PassValueClass& Sender,  int& Value)
	{ Value = Sender.GetValue(); return Sender; }

	friend PassValueClass& operator >>(PassValueClass& Sender,  uint32_t& Value)
	{ Value = Sender.GetValue(); return Sender; }

    friend PassValueClass& operator | (PassValueClass& Sender, PassValueClass& Reciever)
	{ Sender.setLink(&Reciever); return Reciever; }

    friend std::shared_ptr<PassValueClass>  operator | (std::shared_ptr<PassValueClass> Sender, 
														std::shared_ptr<PassValueClass> Reciever)
	{ Sender->setLink(Reciever.get()); return Reciever; }

    friend PassValueClass& operator | (std::shared_ptr<PassValueClass> Sender, PassValueClass& Reciever)
	{ Sender->setLink(&Reciever); return Reciever; }
};

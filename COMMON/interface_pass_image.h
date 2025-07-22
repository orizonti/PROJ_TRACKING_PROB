#pragma once
#include <opencv2/opencv.hpp>
#include <vector>

class PassImageClass
{
public:
	std::vector<PassImageClass*> NodesLinked;
	bool isLinked() { return !NodesLinked.empty();}

	virtual const cv::Mat& GetOutput() = 0;
	virtual void SetInput(const cv::Mat& Image) = 0;

	void SetLink(PassImageClass* NewLink) { NodesLinked.push_back(NewLink); }
	void PassImage() { if(!isLinked()) return; for(auto& link: NodesLinked) *this >> *link; }
	
	friend PassImageClass& operator >>(const     cv::Mat&  Image, PassImageClass& Reciever)
	{ Reciever.SetInput(Image); if(Reciever.isLinked()) Reciever.PassImage(); return Reciever; }

	friend PassImageClass& operator >>(PassImageClass& Sender,    cv::Mat& Image)
	{ Image = Sender.GetOutput(); return Sender; }

    friend PassImageClass& operator >>(PassImageClass& Sender, PassImageClass& Reciever)
	{ Reciever.SetInput(Sender.GetOutput()); if(Reciever.isLinked()) Reciever.PassImage(); return Reciever; }

    friend PassImageClass& operator | (PassImageClass& Sender, PassImageClass& Reciever)
	{ Sender.SetLink(&Reciever); return Reciever; }

    friend std::shared_ptr<PassImageClass>  operator | ( std::shared_ptr<PassImageClass> Sender, 
	                                                     std::shared_ptr<PassImageClass> Reciever)
	{ Sender->SetLink(Reciever.get()); return Reciever; }

    friend PassImageClass& operator | (std::shared_ptr<PassImageClass> Sender, PassImageClass& Reciever)
	{ Sender->SetLink(&Reciever); return Reciever; }
};



#include "Observable.h"


Observable::Observable(void)
{
}


Observable::~Observable(void)
{
}

void Observable::addObserver(AObserver* obs)
{
	this->mObservers.push_back(obs);
}

void Observable::notifyObservers()
{
	for(std::vector<AObserver*>::iterator it = this->mObservers.begin(); 
		it != this->mObservers.end(); 
		it++)
	{
		(*it)->observedObjectChanged(this);
	};
}
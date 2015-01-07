/**
 *  @file Observable.h
 *
 *	@section DESCRIPTION
 *	Basic Observable-Functionality
 */

#pragma once

#include <vector>
#include "AObserver.h"
/**
 *	Basic Observable-Functionality
 */
class Observable
{
public:
	Observable(void);
	~Observable(void);
	
	/**
	* Add an observer to this objects observers-list.
	*
	* @param _obs Observer to be added.
	*/
	void addObserver(AObserver* _obs);

protected:	
	/**
	* Notify all observers, that this object has been changed.
	*/
	void notifyObservers();

private:
	/**
	*	List of all observers of this object.
	*/
	std::vector<AObserver*> mObservers;
};


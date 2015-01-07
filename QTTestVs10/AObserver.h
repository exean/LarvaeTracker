/**
 *  @file AObserver.h
 *
 *	@section DESCRIPTION
 *  Interface for an observer class.
 */
#pragma once

class Observable;
/**
 *  Interface for an observer class.
 */
class AObserver
{
	public:
		/**
        * Called when an observed object has changed.
        *
        * @param	_source Pointer to source object that has changed.
        */
		virtual void observedObjectChanged(Observable * _source) = 0;
};


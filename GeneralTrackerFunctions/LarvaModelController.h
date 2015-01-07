/**
 *  @file LarvaModelController.h
 *
 *	@section DESCRIPTION
 *	Modifies LarvaModels
 */

#pragma once

#include "..//QTTestVs10//LarvaModel.h"
#include "..//QTTestVs10//Vector2D.h"
#include "Toolbox.h"
#include "ModelFinder.h"
#include "SpineFinder.h"

namespace GeneralTrackerFuncs
{
	/**
	 *	Modifies LarvaModels
	 */
	class LarvaModelController
	{
	public:
		LarvaModelController(void);
		~LarvaModelController(void);

		/**
        * Translates a larva model by given dx,dy.
        *
        * @param	mod_larva Model to be modified by this method
		* @param	_translation Point(dx, dy)
        */
		void translate					(LarvaModel *mod_larva, const Vector2D		_translation);
		/**
        * Moves the model so that its tail alignes to given point.
        *
        * @param	mod_larva Model to be modified by this method
		* @param	_newtail New position of tail
        */
		void progressByTail				(LarvaModel *mod_larva, const Vector2D		_newtail	);
		/**
        * Moves the model so that its head alignes to given point.
        *
        * @param	mod_larva Model to be modified by this method
		* @param	_newhead New position of head
        */
		void progressByHead				(LarvaModel *mod_larva, const Vector2D		_newhead	);
		/**
        * Moved the given model forwards.
        *
        * @param	mod_larva Model to be modified by this method
		* @param	_distance Distance to be moved.
        */
		void progress					(LarvaModel *mod_larva, const double		_distance	);
		/**
        * Moved the given model backwards.
        *
        * @param	mod_larva Model to be modified by this method
		* @param	_distance Distance to be moved.
        */
		void retreat					(LarvaModel *mod_larva, const double		_distance	);
	};
}


#include "LarvaModelController.h"

namespace GeneralTrackerFuncs
{

	LarvaModelController::LarvaModelController(void)
	{
	}


	LarvaModelController::~LarvaModelController(void)
	{
	}

	void LarvaModelController::translate(LarvaModel *mod_larva, const Vector2D _translation)
	{
		mod_larva->setCenter(mod_larva->getCenter() + _translation);
		for (vector<Vector2D>::iterator segIt = mod_larva->getSpine()->begin(); 
			 segIt != mod_larva->getSpine()->end(); 
			 segIt++)
		{
			*segIt += _translation;
		}
	};

	void LarvaModelController::progressByTail(LarvaModel *mod_larva, const Vector2D _newtail)
	{	
		this->progress(mod_larva, (_newtail - mod_larva->getTail()).length());
		this->translate(mod_larva, (_newtail - mod_larva->getTail()));
	};

	void LarvaModelController::progressByHead(LarvaModel *mod_larva, const Vector2D _newhead)
	{
		if(mod_larva->getSpine()->size() == 0) 
		{
			return;
		}
	
		this->progress(mod_larva, (_newhead-mod_larva->getHead()).length());
		this->translate(mod_larva, (_newhead - mod_larva->getHead()));
	};

	void LarvaModelController::progress(LarvaModel *mod_larva, const double _distance)
	{	
		if(_distance < 0)
		{
			return this->retreat(mod_larva, -_distance);
		}
	
		vector<Vector2D> newspine;	
		//directional vector between current and successive point
		Vector2D direction;
		//distance in current segment
		double currentDistance	= 0;
		//distance between current point pair
		double pointDistance	= 0;
		vector<Vector2D>::iterator currentPoint;
	
		for (vector<Vector2D>::iterator segIt = mod_larva->getSpine()->begin();
			segIt != mod_larva->getSpine()->end(); 
			 segIt++)
		{
			currentDistance = _distance;
			currentPoint = vector<Vector2D>::iterator(segIt);
		
			//Go along spine
			while (currentPoint != mod_larva->getSpine()->begin() 
				&& currentDistance > (pointDistance = (*currentPoint- *(currentPoint-1)).length()))
			{
				currentDistance -= pointDistance;
				currentPoint--;
			}
		
			if (currentPoint != mod_larva->getSpine()->begin()) 
			{
				//directional vector between current and successive point
				direction = Vector2D(*(currentPoint-1));
				direction -= Vector2D(*currentPoint);
			}
			else 
			{
				//First point of spine
				direction = Vector2D(*(currentPoint));
				direction -= Vector2D(*(currentPoint+1));
			}
		
			//compute  directional vector
			direction.normalize();
			direction *= currentDistance;
		
			//update spine point
			newspine.push_back(*currentPoint+direction);
		}
		mod_larva->setSpine(newspine);
	};


	void LarvaModelController::retreat (LarvaModel* mod_larva, const double _distance)
	{
		if(_distance < 0)
		{
			return this->progress(mod_larva, -_distance);
		}

		vector<Vector2D> newspine;
	
		//directional vector between current and successive point
		Vector2D direction;
		//distance in current segment
		double currentDistance	= 0;
		//distance between current point pair
		double pointDistance	= 0;
		vector<Vector2D>::iterator currentPoint;
	
		for (vector<Vector2D>::iterator segIt = mod_larva->getSpine()->begin();
			 segIt != mod_larva->getSpine()->end(); 
			 segIt++)
		{
			currentDistance = _distance;
			currentPoint = vector<Vector2D>::iterator(segIt);
		
			//Go along spine
			while (currentPoint != mod_larva->getSpine()->end()-1 
				&& currentDistance > (pointDistance = (*currentPoint - *(currentPoint+1)).length()))
			{
				currentDistance -= pointDistance;
				currentPoint++;
			}
		
			if (currentPoint != mod_larva->getSpine()->end()-1) 
			{
				//Inside Larva:
				//directional vector between current and successive point
				direction = Vector2D(*(currentPoint+1));
				direction -= Vector2D(*currentPoint);
			}
			else 
			{	//Behind tail:
				//directional vector between current and predessor point
				direction = Vector2D(*(currentPoint));
				direction -= Vector2D(*(currentPoint-1));
			}
		
			//compute  directional vector
			direction.normalize();
			direction *= currentDistance;
		
			//updaten spine point
			newspine.push_back(*currentPoint + direction.toPoint());
		}
		mod_larva->setSpine(newspine);
	};
	
}
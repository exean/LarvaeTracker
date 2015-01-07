#include "ExportToCSV.h"

namespace InOut
{
	ExportToCSV::ExportToCSV(void)
	{
	}


	ExportToCSV::~ExportToCSV(void)
	{
	}

	bool ExportToCSV::exportLarvae(
		const string				&	_rowSeperator,
		const string				&	_colSeperator,
		const bool						_resultsInMM,
		const string				&	_file, 
		const vector<LarvaModel*>	&	_larvae,
		const bool						_allXY,
		const bool 					_massXY,
		const bool 					_allRadi,
		const bool 					_contourArea,
		const bool 					_collisionFlag,
		const bool 					_nnDistance,
		const bool 					_massDistance,
		const vector<Point>			&	_landmarks,//Empty, if no information shall be exported
		const bool 					_velocity,
		const bool 					_acceleration,
		const bool 					_originDistance,
		const bool 					_accDistance,
		const bool 					_direction,
		const unsigned int			_timeSteps,
		const double					_pixelPerMM,
		const double 					_fps,
		const int _p1, const int _p2, const int _p3)
	{		
		this->mRowSeperator = _rowSeperator;
		this->mColSeperator = _colSeperator;

		fstream datei;
		datei.open(_file, ios::out);
		std::ostringstream oss;
		
		double pixelToMetricFactor = _resultsInMM ? 1 / _pixelPerMM : 1;

		//All spine points
		if(_allXY)
		{		
			this->getAllXY(_larvae, datei, _timeSteps, pixelToMetricFactor);
		}

		//Mass points
		if(_massXY)
		{					
			this->getMassXY(_larvae, datei, _timeSteps, pixelToMetricFactor);
		}

		//Widths points
		if(_allRadi)
		{					
			this->getAllRadi(_larvae, datei, _timeSteps, pixelToMetricFactor);
		}
		if(_contourArea)//Area
		{					
			this->getContourArea(_larvae, datei, _timeSteps, pixelToMetricFactor);
		}
		//Collision flag
		if(_collisionFlag)
		{		
			this->getCollisionFlag(_larvae, datei, _timeSteps);
		}

		//---Second order:

		//Distance to nearest neighbor
		if(_nnDistance)
		{
			this->getNNDistance(_larvae, datei, _timeSteps, pixelToMetricFactor);
		}

		//Distance between two centers of mass
		if(_massDistance)
		{		
			this->getMassDistance(_larvae, datei, _timeSteps, pixelToMetricFactor);
		}
	
		//Distance to Landmarks, if list is empty->no export
		this->getDistanceToLandmarks(_larvae, _landmarks, datei, _timeSteps, pixelToMetricFactor);
	
		//Velocity
		if(_velocity)
		{
			this->getVelocity(_larvae, datei, _timeSteps, _fps, pixelToMetricFactor);
		}

		//Acceleration
		if(_acceleration)
		{
			this->getAcceleration(_larvae, datei, _timeSteps, _fps, pixelToMetricFactor);
		}

		//Distance to origin
		if(_originDistance)
		{		
			this->getDistanceToOrigin(_larvae, datei, _timeSteps, pixelToMetricFactor);
		}

		//Accumulated distance
		if(_accDistance)
		{
			this->getAccumulatedDistance(_larvae, datei, _timeSteps, pixelToMetricFactor);
		}

		//Direction of movement
		if(_direction)
		{
			this->getMovementDirection(_larvae, datei, _timeSteps);
		}

		//3-Point-curvature
		if(_p1 != -1 && _p2 != -1 && _p3 != -1)
		{
			this->getCurvature(_p1, _p2, _p3, _larvae, datei, _timeSteps);
		}
	
		datei.close();

		return true;
	}

#pragma region  SpinePoints
	void ExportToCSV::setupAllXY (
		const vector< LarvaModel* >	&	_larvae, 
		const unsigned int			_timeSteps,
		vector<vector<string>>		&	out_x,
		vector<vector<string>>		&	out_y,
		int							&	out_spineSize)
	{
		//Determin spine size
		LarvaModel* larva;
		for(larva = _larvae[0]; !larva->isResolved(); larva = larva->getNext());
		out_spineSize = larva->getSpine()->size();
				
		std::ostringstream oss;

		for(unsigned int t = 0; t < _timeSteps; t++)
		{
			vector<string> tempX;
			vector<string> tempY;

			for(int i = 0; i < out_spineSize; i++)
			{	
				oss.str("");
				oss << "x_" << setw(5) << setfill('0') << i << ":" << setw(5) << setfill('0') << t;
				tempX.push_back(oss.str());
			
				oss.str("");								
				oss << "y_" << setw(5) << setfill('0') << i << ":" << setw(5) << setfill('0') << t;
				tempY.push_back(oss.str());
			}
			out_x.push_back(tempX);
			out_y.push_back(tempY);
		}
	}

	void ExportToCSV::getAllXY	(
		const vector<LarvaModel*>	&	_larvae, 
		fstream						&	_file,
		const unsigned int			_timeSteps,
		const double					_pixelToMetricFactor)
	{
		//Setup
		vector<vector<string>> x_;
		vector<vector<string>> y_;
		int spineSize;
		this->setupAllXY(_larvae, _timeSteps, x_, y_, spineSize);

		std::ostringstream oss;

		//Start gathering data
		vector<LarvaModel*>::const_iterator larvaeEnd	= _larvae.end();
		vector<LarvaModel*>::const_iterator larvaIt		= _larvae.begin();

		for(;
			larvaIt != larvaeEnd;
			larvaIt++)
		{			
			//First handle initial gap (e.g. first detection at timestep x > 0)		
			int firstTimeIndex = (*larvaIt)->getTimeIndex();
			for(int t = 0; t < firstTimeIndex; t++)
			{
				this->insertGap(0, spineSize, x_[t]);
				this->insertGap(0, spineSize, y_[t]);
			}
			int time = 0;
			//Second collect data of models
			for(LarvaModel* currentLarva = *larvaIt;
				currentLarva != NULL;
				currentLarva = currentLarva->getNext())
			{
				bool modelResolved = currentLarva->isResolved();
				
				vector<Vector2D> *spine = currentLarva->getSpine();	

				time = currentLarva->getTimeIndex();

				for(int i = 0; i < spineSize; i++)
				{								
					x_[time][i] += this->mColSeperator;					
					y_[time][i] += this->mColSeperator;

					if(modelResolved)
					{
						oss.str("");
						oss << x_[time][i] << (spine->at(i).getX() * _pixelToMetricFactor);
						x_[time][i] = oss.str();

						oss.str("");
						oss << y_[time][i] << (spine->at(i).getY() * _pixelToMetricFactor);
						y_[time][i] = oss.str();
					}					
				}
			}

			//Finally handle terminal gap (e.g. last detection at timestep x < finalTimeStep)	
			for(unsigned int t = time+1; t < _timeSteps; t++)
			{
				this->insertGap(0, spineSize, x_[t]);
				this->insertGap(0, spineSize, y_[t]);
			}
		}

		//Write
		for(vector<vector<string>>::iterator it = x_.begin(); it != x_.end(); it++)
		{
			this->write(*it, _file);
		}
		for(vector<vector<string>>::iterator it = y_.begin(); it != y_.end(); it++)
		{
			this->write(*it, _file);
		}
	}
#pragma endregion

	void ExportToCSV::getMassXY (
		const vector<LarvaModel*>	&	_larvae, 
		fstream						&	_file,
		const unsigned int			_timeSteps,
		const double					_pixelToMetricFactor)
	{
		//Setup
		vector<string> mass_x;
		vector<string> mass_y;
		this->setupStrings	(
			_timeSteps,
			"mass_x:",
			mass_x);
		this->setupStrings	(
			_timeSteps,
			"mass_y:",
			mass_y);	

		std::ostringstream oss;

		//Start gathering data
		vector<LarvaModel*>::const_iterator larvaeEnd	= _larvae.end();
		vector<LarvaModel*>::const_iterator larvaIt		= _larvae.begin();

		for(vector<LarvaModel*>::const_iterator larvaIt = _larvae.begin();
			larvaIt != larvaeEnd;
			larvaIt++)
		{		
			//First handle initial gap (e.g. first detection at timestep x > 0)		
			int firstTimeIndex = (*larvaIt)->getTimeIndex();
			this->insertGap(0, firstTimeIndex, mass_x);
			this->insertGap(0, firstTimeIndex, mass_y);
			
			int time = 0;
			//Second collect data from models
			for(LarvaModel* currentLarva = *larvaIt;
				currentLarva != NULL;
				currentLarva = currentLarva->getNext())
			{
				
				time = currentLarva->getTimeIndex();

				mass_x[time] += this->mColSeperator;
				mass_y[time] += this->mColSeperator;

				if(currentLarva->isResolved())
				{
					oss.str("");
					oss << mass_x[time] << (currentLarva->getCenter().getX() * _pixelToMetricFactor);
					mass_x[time] = oss.str();

					oss.str("");
					oss << mass_y[time] << (currentLarva->getCenter().getY() * _pixelToMetricFactor);
					mass_y[time] = oss.str();	
				}
			}

			//Finally handle terminal gap (e.g. last detection at timestep x < finalTimeStep)		
			this->insertGap(time+1, _timeSteps, mass_y);
			this->insertGap(time+1, _timeSteps, mass_x);
		}

		//Write
		this->write(mass_x, _file);
		this->write(mass_y, _file);
	}

#pragma region  Radi
	void ExportToCSV::setupRadi (
		const vector< LarvaModel* >	&	_larvae, 
		const unsigned int			_timeSteps,
		vector<vector<string>>		&	out_radi,
		int							&	out_spineSize)
	{
		//Determin spine size
		LarvaModel* larva;
		for(larva = _larvae[0]; !larva->isResolved(); larva = larva->getNext());
		out_spineSize = larva->getSpine()->size();
				
		std::ostringstream oss;

		for(unsigned int t = 0; t < _timeSteps; t++)
		{
			vector<string> temp;

			for(int i = 0; i < out_spineSize; i++)
			{	
				oss.str("");
				oss << "radius_" << setw(5) << setfill('0') << i << ":" << setw(5) << setfill('0') << t;
				temp.push_back(oss.str());
			}
			out_radi.push_back(temp);
		}
	}

	void ExportToCSV::getAllRadi(
		const vector<LarvaModel*>	&	_larvae, 
		fstream						&	_file,
		const unsigned int			_timeSteps,
		const double					_pixelToMetricFactor)
	{
		//Setup
		vector<vector<string>> radi;
		int spineSize;
		this->setupRadi(_larvae, _timeSteps, radi, spineSize);

		std::ostringstream oss;

		//Start gathering data
		vector<LarvaModel*>::const_iterator larvaeEnd	= _larvae.end();
		vector<LarvaModel*>::const_iterator larvaIt		= _larvae.begin();

		for(vector<LarvaModel*>::const_iterator larvaIt = _larvae.begin();
			larvaIt != larvaeEnd;
			larvaIt++)
		{			
			//First handle initial gap (e.g. first detection at timestep x > 0)		
			int firstTimeIndex = (*larvaIt)->getTimeIndex();
			for(int t = 0; t < firstTimeIndex; t++)
			{
				this->insertGap(0, spineSize, radi[t]);
			}
			int time = 0;
			//Next handle existing models
			for(LarvaModel* currentLarva = *larvaIt;
				currentLarva != NULL;
				currentLarva = currentLarva->getNext())
			{
				time = currentLarva->getTimeIndex();
				bool larvaResolved = currentLarva->isResolved();				
				vector<double> *widths = NULL;
				if(larvaResolved)
					widths = currentLarva->getWidths();	

				for(int i = 0; i < spineSize; i++)
				{			
					radi[time][i] += this->mColSeperator;
					if(larvaResolved)
					{
						oss.str("");
						oss << radi[time][i] << ((widths->at(i)/2) * _pixelToMetricFactor);
						radi[time][i] = oss.str();
					}
				}
			}

			//Finally handle terminal gap (e.g. last detection at timestep x < finalTimeStep)
			for(unsigned int t = time+1; t < _timeSteps; t++)
			{
				this->insertGap(0, spineSize, radi[t]);
			}
		}

		//Write
		for(vector<vector<string>>::iterator it = radi.begin(); it != radi.end(); it++)
		{			
			this->write(*it, _file);
		}
	}
#pragma endregion

	void ExportToCSV::getContourArea(
			const vector<LarvaModel*>	&	_larvae, 
			fstream						&	_file, 
			const unsigned int			_timeSteps,
			const double					_pixelToMetricFactor)
	{
		//Setup
		vector<string> area;
		this->setupStrings	(
			_timeSteps,
			"area:",
			area);	

		std::ostringstream oss;	

		vector<LarvaModel*>::const_iterator larvaEnd = _larvae.end();
		for(vector<LarvaModel*>::const_iterator larvaIt = _larvae.begin();
			larvaIt != larvaEnd;
			larvaIt++)
		{		
			//First handle initial gap (e.g. first detection at timestep x > 0)		
			int firstTimeIndex = (*larvaIt)->getTimeIndex();
			this->insertGap(0, firstTimeIndex, area);
			int time = 0;
			for(LarvaModel* currentLarva = *larvaIt;
				currentLarva != NULL;
				currentLarva = currentLarva->getNext())
			{
				int time = currentLarva->getTimeIndex();
				bool larvaResolved = currentLarva->isResolved() && !currentLarva->isCollision();
				
				area[time] += this->mColSeperator;
				if(larvaResolved)
				{
					oss.str("");
					oss << area[time] << (cv::contourArea(currentLarva->getContour()) * _pixelToMetricFactor * _pixelToMetricFactor);
					area[time] = oss.str();
				}
			}

			//Finally handle terminal gap (e.g. last detection at timestep x < finalTimeStep)					
			this->insertGap(time+1, _timeSteps, area);
		}

		//Write
		this->write(area, _file);
	}

	void ExportToCSV::getCollisionFlag		(
		const vector<LarvaModel*>	&	_larvae, 
		fstream						&	_file,
		const unsigned int			_timeSteps)
	{
		//Setup
		vector<string> collision;
		this->setupStrings	(
			_timeSteps,
			"collision:",
			collision);

		std::ostringstream oss;	
		
		vector<LarvaModel*>::const_iterator larvaEnd = _larvae.end();
		for(vector<LarvaModel*>::const_iterator larvaIt = _larvae.begin();
			larvaIt != larvaEnd;
			larvaIt++)
		{	
			//First handle initial gap (e.g. first detection at timestep x > 0)		
			int firstTimeIndex = (*larvaIt)->getTimeIndex();
			this->insertGap(0, firstTimeIndex, collision);
			int time = 0;
			for(LarvaModel* currentLarva = *larvaIt;
				currentLarva != NULL;
				currentLarva = currentLarva->getNext())
			{
				time = currentLarva->getTimeIndex();
				bool resolvedLarva = currentLarva->isResolved();

				collision[time] += this->mColSeperator;
				if(resolvedLarva)
				{
					oss.str("");
					oss << collision[time] << (currentLarva->isCollision() ? "1" : "0");
					collision[time] = oss.str();
				}
					
			}

			//Finally handle terminal gap (e.g. last detection at timestep x < finalTimeStep)			
			this->insertGap(time+1, _timeSteps, collision);
		}

		//Write
		this->write(collision, _file);
	}

#pragma region NearestNeighbor
	double ExportToCSV::getNNDistanceAtTime(
		const LarvaModel			*	_larva,
		const vector<LarvaModel*>	&	_neighbors,
		const unsigned int			_time,
		const double					_pixelToMetricFactor)
	{
		double shortestDistance		= std::numeric_limits<double>::max();
		Vector2D	currentCenter	= _larva->getCenter();

		//Iterate over (neighboring) larvamodels at current time
		vector<LarvaModel*>::const_iterator neighborsEnd =  _neighbors.end();
		for(vector<LarvaModel*>::const_iterator compareLarvaIt = _neighbors.begin();
			compareLarvaIt != neighborsEnd;
			compareLarvaIt++)
		{
			LarvaModel * neighbor;
			//Go to correct time
			for(neighbor = (*compareLarvaIt); 
				neighbor->getTimeIndex() < _time && neighbor->hasNext();
				neighbor = neighbor->getNext());

			if(neighbor->getTimeIndex() == _time 
				&& neighbor != _larva 
				&& neighbor->isResolved())
			{
				double distance = (neighbor->getCenter() - currentCenter).length();
				if(distance <= shortestDistance)
				{
					shortestDistance = distance;
				}
			}
		}

		shortestDistance *= _pixelToMetricFactor;
		return shortestDistance;
	}

	void ExportToCSV::getNNDistance(
		const vector<LarvaModel*>	&	_larvae, 
		fstream						&	_file,
		const unsigned int			_timeSteps,
		const double					_pixelToMetricFactor)
	{
		//Setup
		vector<string> nn;
		this->setupStrings(
			_timeSteps,
			"dist_nn:",
			nn);

		std::ostringstream oss;
		
		vector<LarvaModel*>::const_iterator larvaEnd = _larvae.end();
		for(vector<LarvaModel*>::const_iterator larvaIt = _larvae.begin();
			larvaIt != larvaEnd;
			larvaIt++)
		{
			//First handle initial gap (e.g. first detection at timestep x > 0)		
			int firstTimeIndex = (*larvaIt)->getTimeIndex();
			this->insertGap(0, firstTimeIndex, nn);
			int time = 0;
			for(LarvaModel* currentLarva = *larvaIt;
				currentLarva != NULL;
				currentLarva = currentLarva->getNext())
			{
				time = (*larvaIt)->getTimeIndex();
				bool larvaResolved = (*larvaIt)->isResolved();

				nn[time] += this->mColSeperator;
				if(larvaResolved)
				{
					oss.str("");
					oss << nn[time] 
						<< this->getNNDistanceAtTime(
								currentLarva, 
								_larvae, 
								time, 
								_pixelToMetricFactor);
					nn[time] = oss.str();	
				}		
			}

			//Finally handle terminal gap (e.g. last detection at timestep x < finalTimeStep)	
			this->insertGap(time+1, _timeSteps, nn);
		}

		//Write
		this->write(nn, _file);
	}
#pragma endregion

	//Distance to previous contour center of mass
	void ExportToCSV::getMassDistance(
		const vector<LarvaModel*>	&	_larvae, 
		fstream						&	_file, 
		const unsigned int			_timeSteps,
		const double					_pixelToMetricFactor)
	{
		//Setup
		vector<string> dist;
		this->setupStrings(
			_timeSteps,
			"dist_mass:",
			dist);

		std::ostringstream oss;	
	
		int larvaCounter = 0;
		vector<LarvaModel*>::const_iterator larvaEnd = _larvae.end();
		for(vector<LarvaModel*>::const_iterator larvaIt = _larvae.begin();
			larvaIt != larvaEnd;
			larvaIt++,
			larvaCounter++)
		{		
			//First handle initial gap (e.g. first detection at timestep x > 0)		
			int firstTimeIndex = (*larvaIt)->getTimeIndex();
			this->insertGap(0, firstTimeIndex, dist);
			int time = 0;
			for(LarvaModel* currentLarva = *larvaIt;
				currentLarva != NULL;
				currentLarva = currentLarva->getNext())
			{			
				time = currentLarva->getTimeIndex();
				bool larvaResolved = currentLarva->isResolved();

				dist[time] += this->mColSeperator;

				if(larvaResolved)
				{
					if(currentLarva->hasPrev())
					{
						LarvaModel* prev = currentLarva->getPrev();
						if(prev->isResolved())
						{
							double distance = Vector2D(currentLarva->getCenter()-prev->getCenter()).length() * _pixelToMetricFactor;
							
							oss.str("");
							oss << dist[time] << distance;
							dist[time] = oss.str();
						}
					}
				}
			}

			//Finally handle terminal gap (e.g. last detection at timestep x < finalTimeStep)		
			this->insertGap(time+1, _timeSteps, dist);
		}

		//Write
		this->write(dist, _file);
	}

	void ExportToCSV::getDistanceToLandmarks(
		const vector<LarvaModel*>	&	_larvae, 
		const vector<Point>			&	_landmarks, 
		fstream						&	_file, 
		const unsigned int			_timeSteps,
		const double					_pixelToMetricFactor)
	{
		unsigned int landmarkCounter = 0;
		vector<Point>::const_iterator landmarksEnd = _landmarks.end();
		for(vector<Point>::const_iterator landmarkIt = _landmarks.begin();
			landmarkIt != landmarksEnd;
			landmarkIt++, landmarkCounter++)
		{
			std::ostringstream oss;			
			
			//Setup
			vector<string> dist;
			oss.str("");
			oss << "dist_" << landmarkCounter << "_(" << landmarkIt->x << this->mColSeperator << landmarkIt->y << "):";
			this->setupStrings(
				_timeSteps,
				oss.str(),
				dist);
			
			double distance = 0;
			vector<LarvaModel*>::const_iterator larvaEnd = _larvae.end();
			for(vector<LarvaModel*>::const_iterator larvaIt = _larvae.begin();
				larvaIt != larvaEnd;
				larvaIt++)
			{				
				//First handle initial gap (e.g. first detection at timestep x > 0)		
				int firstTimeIndex = (*larvaIt)->getTimeIndex();
				this->insertGap(0, firstTimeIndex, dist);
				int time = 0;
				for(LarvaModel* currentLarva = *larvaIt;
					currentLarva != NULL;
					currentLarva = currentLarva->getNext())
				{
					time = currentLarva->getTimeIndex();
					bool larvaResolved = currentLarva->isResolved();

					dist[time] += this->mColSeperator;
					if(larvaResolved)
					{
						distance = Vector2D(currentLarva->getCenter()-*landmarkIt).length() * _pixelToMetricFactor;
						oss.str("");
						oss << dist[time] << distance;
						dist[time] = oss.str();
					}											
				}

				//Finally handle terminal gap (e.g. last detection at timestep x < finalTimeStep)	
				this->insertGap(time+1, _timeSteps, dist);
			}

			//Write
			this->write(dist, _file);
		}
	}
	
	void ExportToCSV::getDistanceToOrigin(
		const vector<LarvaModel*>	&	_larvae, 
		fstream						&	_file, 
		const unsigned int			_timeSteps,
		const double					_pixelToMetricFactor)
	{
		//Setup
		vector<string> dist;
		this->setupStrings(
			_timeSteps,
			"dist_origin:",
			dist);
		std::ostringstream oss;	

		vector<LarvaModel*>::const_iterator larvaEnd = _larvae.end();
		for(vector<LarvaModel*>::const_iterator larvaIt = _larvae.begin();
			larvaIt != larvaEnd;
			larvaIt++)
		{					
			//First handle initial gap (e.g. first detection at timestep x > 0)		
			int firstTimeIndex = (*larvaIt)->getTimeIndex();
			this->insertGap(0, firstTimeIndex, dist);		

			//Origin = center of mass of first resolved mode
			Vector2D origin;
			LarvaModel * larva;
			for(larva = (*larvaIt); !larva->isResolved(); larva = larva->getNext());
			origin = larva->getCenter();

			double distance = 0;
			int time = 0;
			for(LarvaModel* currentLarva = larva;
				currentLarva != NULL;
				currentLarva = currentLarva->getNext())
			{
				time = currentLarva->getTimeIndex();
				bool larvaResolved = currentLarva->isResolved();
				
				dist[time] += this->mColSeperator;
				if(larvaResolved)
				{
					distance = Vector2D(currentLarva->getCenter()-origin).length() * _pixelToMetricFactor;
					oss.str("");
					oss << dist[time] << distance;
					dist[time] = oss.str();
				}
			}

			//Finally handle terminal gap (e.g. last detection at timestep x < finalTimeStep)	
			this->insertGap(time+1, _timeSteps, dist);
		}

		//Write
		this->write(dist, _file);
	}

	void ExportToCSV::getAccumulatedDistance(
		const vector<LarvaModel*>	&	_larvae, 
		fstream						&	_file, 
		const unsigned int			_timeSteps,
		const double					_pixelToMetricFactor)
	{
		//Setup
		vector<double> distances;
		vector<string> dist;
		this->setupStrings(
			_timeSteps,
			"dist_accumulated:",
			dist);
		std::ostringstream oss;	
	
		unsigned int larvaCounter = 0;
		vector<LarvaModel*>::const_iterator larvaEnd = _larvae.end();
		for(vector<LarvaModel*>::const_iterator larvaIt = _larvae.begin();
			larvaIt != larvaEnd;
			larvaIt++,
			larvaCounter++)
		{			
			//First handle initial gap (e.g. first detection at timestep x > 0)		
			int firstTimeIndex = (*larvaIt)->getTimeIndex();
			this->insertGap(0, firstTimeIndex, dist);
			int time = 0;
			for(LarvaModel* currentLarva = *larvaIt;
				currentLarva != NULL;
				currentLarva = currentLarva->getNext())
			{		
				time = currentLarva->getTimeIndex();
				bool larvaResolved = currentLarva->isResolved();
				if(distances.size() <= larvaCounter)
				{
					distances.push_back(0);
				}
				else if(larvaResolved)
				{
					distances[larvaCounter] += 
						Vector2D(currentLarva->getCenter()-currentLarva->getPrev()->getCenter()).length()
						* _pixelToMetricFactor;
				}

				dist[time] += this->mColSeperator;
				if(larvaResolved)
				{
					oss.str("");
					oss << dist[time] << distances[larvaCounter];					
					dist[time] = oss.str();
				}
			}
			
			//Finally handle terminal gap (e.g. last detection at timestep x < finalTimeStep)		
			this->insertGap(time+1, _timeSteps, dist);
		}

		//Write
		this->write(dist, _file);
	}

	//Center of mass based direction of larva movement
	void ExportToCSV::getMovementDirection(
		const vector<LarvaModel*>	&	_larvae, 
		fstream						&	_file,
		const unsigned int			_timeSteps)
	{
		//Setup
		vector<string> angle;
		this->setupStrings(
			_timeSteps,
			"angle:",
			angle);
		std::ostringstream oss;	
	
		vector<LarvaModel*>::const_iterator larvaeEnd = _larvae.end();
		for(vector<LarvaModel*>::const_iterator larvaIt = _larvae.begin();
			larvaIt != larvaeEnd;
			larvaIt++)
		{			
			//First handle initial gap (e.g. first detection at timestep x > 0)		
			int firstTimeIndex = (*larvaIt)->getTimeIndex();
			this->insertGap(0, firstTimeIndex, angle);	

			double currentAngle = 0;
			int time = 0;
			for(LarvaModel* currentLarva = *larvaIt;
				currentLarva != NULL;
				currentLarva = currentLarva->getNext())
			{	
				time = currentLarva->getTimeIndex();
				bool larvaResolved = currentLarva->isResolved()
					&& currentLarva->hasPrev()
					&& currentLarva->getPrev()->isResolved();				

				angle[time] += this->mColSeperator;
				if(larvaResolved)
				{
					Vector2D a(currentLarva->getCenter()-currentLarva->getPrev()->getCenter());
					Vector2D b(0,1);
					currentAngle = GeneralTrackerFuncs::Toolbox::Rad2Grad(acos((a*b) / (a.length()*b.length())));

					oss.str("");
					oss << angle[time] << currentAngle;
					angle[time] = oss.str();
				}
			}
			
			//Finally handle terminal gap (e.g. last detection at timestep x < finalTimeStep)	
			this->insertGap(time+1, _timeSteps, angle);
		}

		//Write
		this->write(angle, _file);
	}

	void ExportToCSV::getCurvature(
		const int						_p1, 
		const int						_p2, 
		const int						_p3, 
		const vector<LarvaModel*>	&	_larvae, 
		fstream						&	_file,
		const unsigned int			_timeSteps)
	{
		//Setup
		vector<string> angle;
		std::ostringstream oss;
		oss.str("");
		oss << "curvature_" << _p1 << "_" << _p2 << "_" << _p3 << ":";		
		this->setupStrings(
			_timeSteps,
			oss.str(),
			angle);	
	
		double currentAngle = 0;
	
		vector<LarvaModel*>::const_iterator larvaEnd = _larvae.end();		
		for(vector<LarvaModel*>::const_iterator larvaIt = _larvae.begin();
			larvaIt != _larvae.end();
			larvaIt++)
		{		
			//First handle initial gap (e.g. first detection at timestep x > 0)		
			int firstTimeIndex = (*larvaIt)->getTimeIndex();
			this->insertGap(0, firstTimeIndex, angle);	
			int time = 0;
			for(LarvaModel* currentLarva = *larvaIt;
				currentLarva != NULL;
				currentLarva = currentLarva->getNext())
			{	
				time = currentLarva->getTimeIndex();
				bool larvaResolved = currentLarva->isResolved();

				angle[time] += this->mColSeperator;
				if(larvaResolved)
				{ 
					vector<Vector2D>* spine = currentLarva->getSpine();
					Vector2D a(spine->at(_p1)-spine->at(_p2));
					Vector2D b(spine->at(_p3)-spine->at(_p2));
					currentAngle = GeneralTrackerFuncs::Toolbox::Rad2Grad(acos((a*b) / (a.length()*b.length())));

					oss.str("");
					oss << angle[time] << currentAngle;
					angle[time] = oss.str();
				}
			}
			
			//Finally handle terminal gap (e.g. last detection at timestep x < finalTimeStep)		
			this->insertGap(time+1, _timeSteps, angle);
		}

		//Write
		this->write(angle, _file);
	}

	void ExportToCSV::getVelocity (
		const vector<LarvaModel*>	&	_larvae, 
		fstream						&	_file, 
		const unsigned int			_timeSteps, 
		const double					_fps,
		const double					_pixelToMetricFactor)
	{
		//Setup
		std::ostringstream oss;	
		vector<string> velocity;
		this->setupStrings(
			_timeSteps,
			"velocity:",
			velocity);
		double currentVelocity = 0;	
		int larvaCounter = 0;

		vector<LarvaModel*>::const_iterator larvaEnd = _larvae.end();
		for(vector<LarvaModel*>::const_iterator larvaIt = _larvae.begin();
			larvaIt != larvaEnd;
			larvaIt++,
			larvaCounter++)
		{			
			//First handle initial gap (e.g. first detection at timestep x > 0)		
			int firstTimeIndex = (*larvaIt)->getTimeIndex();
			this->insertGap(0, firstTimeIndex, velocity);	
			int time = 0;
			for(LarvaModel* currentLarva = *larvaIt;
				currentLarva != NULL;
				currentLarva = currentLarva->getNext())
			{			
				time = currentLarva->getTimeIndex();
				bool larvaResolved = currentLarva->isResolved()
					&& currentLarva->hasPrev()
					&& currentLarva->getPrev()->isResolved();

				velocity[time] += this->mColSeperator;
				if(larvaResolved)
				{
					currentVelocity = 
						(currentLarva->getCenter() - currentLarva->getPrev()->getCenter()).length()
						* _pixelToMetricFactor
						* _fps;

					oss.str("");
					oss << velocity[time] << currentVelocity;
					velocity[time] = oss.str();
				}
			}
			
			//Finally handle terminal gap (e.g. last detection at timestep x < finalTimeStep)	
			this->insertGap(time+1, _timeSteps, velocity);
		}

		this->write(velocity, _file);
	}

	void ExportToCSV::getAcceleration (
		const vector<LarvaModel*>	&	_larvae, 
		fstream						&	_file, 
		const unsigned int			_timeSteps, 
		const double					_fps, 
		const double					_pixelToMetricFactor)
	{
		//Setup
		std::ostringstream oss;	
		vector<string> dist;
		this->setupStrings(
			_timeSteps,
			"acceleration:",
			dist);
		double accel = 0;

		int larvaCounter = 0;
		vector<LarvaModel*>::const_iterator larvaEnd = _larvae.end();
		for(vector<LarvaModel*>::const_iterator larvaIt = _larvae.begin();
			larvaIt != larvaEnd;
			larvaIt++,
			larvaCounter++)
		{			
			//First handle initial gap (e.g. first detection at timestep x > 0)		
			int firstTimeIndex = (*larvaIt)->getTimeIndex();
			this->insertGap(0, firstTimeIndex, dist);
			int time = 0;
			for(LarvaModel* currentLarva = *larvaIt;
				currentLarva != NULL;
				currentLarva = currentLarva->getNext())
			{			
				time = currentLarva->getTimeIndex();

				bool larvaResolved = currentLarva->isResolved()
					&& currentLarva->hasPrev()
					&& currentLarva->getPrev()->isResolved()
					&& currentLarva->getPrev()->hasPrev()
					&& currentLarva->getPrev()->getPrev()->isResolved();

				dist[time] += this->mColSeperator;
				if(larvaResolved)
				{
					double currentDistance = 
						(currentLarva->getCenter() - currentLarva->getPrev()->getCenter()).length();
					double currentSpeed		= 
						currentDistance;
					double previousSpeed	= 
						(currentLarva->getPrev()->getCenter() - currentLarva->getPrev()->getPrev()->getCenter()).length();

					accel = ((pow(currentSpeed, 2) - pow(previousSpeed, 2)) / (2 * currentDistance))
						* _pixelToMetricFactor * std::pow(_fps,2);

					oss.str("");
					oss << dist[time] << accel;
					dist[time] = oss.str();
				}
			}
			
			//Finally handle terminal gap (e.g. last detection at timestep x < finalTimeStep)		
			this->insertGap(time+1, _timeSteps, dist);
		}

		this->write(dist, _file);
	}
	
	void ExportToCSV::setupStrings	(
		const unsigned int			_timeSteps,
		const string				&	_name,
		vector< string >			&	out_strings)	
	{
		std::ostringstream oss;

		for(unsigned int t = 0; t < _timeSteps; t++)
		{
			oss.str("");								
			oss << _name << setw(5) << setfill('0') << t;
			out_strings.push_back(oss.str());
		}
	}

	void ExportToCSV::insertGap	(
		const int						_begin,
		const int						_end,
		vector< string >			&	mod_strings)
	{	
		for(int t = _begin; t < _end; t++)
		{		
			mod_strings[t] += this->mColSeperator;
		}
	}

	void ExportToCSV::write	(
		const vector<string>		&	_data, 
		fstream						&	_file)
	{
		for(vector<string>::const_iterator it = _data.begin(); it != _data.end(); it++)
		{
			_file << *it << this->mRowSeperator;
		}
	}
}
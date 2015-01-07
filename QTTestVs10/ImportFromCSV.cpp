#include "ImportFromCSV.h"

namespace InOut
{
	ImportFromCSV::ImportFromCSV(void)
	{
	}


	ImportFromCSV::~ImportFromCSV(void)
	{
	}

	vector<LarvaModel*>* ImportFromCSV::importLarvae(
		const QString				&	_file,
		const QString				&	_rowSeperator,
		const QString				&	_colSeperator)
	{
		vector<LarvaModel*>* larvae = 0;
		
		QFile myFile(_file);
		QString line;

		if(!myFile.open(QIODevice::ReadOnly)) 
		{
			QMessageBox::information(0, "Error", "Datei konnte nicht geladen werden: "+myFile.errorString());
		}
		else
		{
			QTextStream in(&myFile);

			while(!in.atEnd()) 
			{
				QString line = in.readLine(); //Read line   
				QStringList rows = line.split(_rowSeperator); //Only does anything if rowseperator isnt linebreak 				

				if(rows.size() == 0)
				{
					continue;
				}

				if(larvae == 0)
				{					
					//cols == amount larvae
					int cols = rows[0].count(_colSeperator);					

					if(cols > 0)
					{
						//Initialize larva-list				
						larvae = new vector<LarvaModel*>();
						for(int i = 0; i < cols; i++)
						{
							larvae->push_back(new LarvaModel(false));
						}
					}
					else
					{
						continue;
					}
				}

				for each(QString row in rows)
				{
					this->parseLine(row, _colSeperator, larvae);
				}
			}

			myFile.close();
		}

		return larvae;
	}

	bool ImportFromCSV::parseLine(
		const QString			&	_line,
		const QString			&	_seperator,
		vector<LarvaModel*>		*	mod_larvae)
	{	
		int pointIndexEnd = _line.indexOf(':'); 

		//Time-Index
		int timeIndexEnd = _line.indexOf(_seperator); 
		unsigned int timeIndex = _line.mid(pointIndexEnd+1,timeIndexEnd-(pointIndexEnd+1)).toInt();

		//Collect values
		QStringList values = _line.right(_line.length()-(timeIndexEnd+1)).split(_seperator);
		
		vector< LarvaModel* >::iterator larvaIt = mod_larvae->begin();

		//Determin type:
		//X-Coordinate of spine-point
		if(_line.startsWith("x_"))
		{
			//Index of spine-point
			int pointIndex = _line.mid(2,pointIndexEnd-2).toInt();

			for each(QString s in values)
			{
				if(!s.isEmpty())
				{
					LarvaModel* larva = this->getLarvaTimeFrame(timeIndex, *larvaIt);
					this->setupLarvaSpine(pointIndex, larva);
					larva->getSpine()->at(pointIndex).setX(s.toDouble());
				}
				larvaIt++;
			}
		}	
		//y-Coordinate of spine-point
		else if(_line.startsWith("y_"))
		{	
			//Index of spine-point
			int pointIndex = _line.mid(2,pointIndexEnd-2).toInt();	

			for each(QString s in values)
			{
				if(!s.isEmpty())
				{
					LarvaModel* larva = this->getLarvaTimeFrame(timeIndex, *larvaIt);
					this->setupLarvaSpine(pointIndex, larva);
					larva->getSpine()->at(pointIndex).setY(s.toDouble());
				}
				larvaIt++;
			}
		}	
		//Radi of spine-point
		else if(_line.startsWith("radius_"))
		{
			//Index of spine-point
			int pointIndex = _line.mid(7,pointIndexEnd-7).toInt();		

			for each(QString s in values)
			{
				if(!s.isEmpty())
				{
					LarvaModel* larva = this->getLarvaTimeFrame(timeIndex, *larvaIt);
					this->setupLarvaSpine(pointIndex, larva);
					larva->getWidths()->at(pointIndex) = 2*s.toDouble();
				}
				larvaIt++;
			}
		}
		//Collision flag
		else if(_line.startsWith("collision"))
		{
			for each(QString s in values)
			{
				if(!s.isEmpty())
				{
					LarvaModel* larva = this->getLarvaTimeFrame(timeIndex, *larvaIt);
					larva->setCollision(s.toInt() == 1);
				}
			}
			larvaIt++;
		}
		//X-Coordinate of center
		else if(_line.startsWith("mass_x"))
		{
			for each(QString s in values)
			{
				if(!s.isEmpty())
				{
					LarvaModel* larva = this->getLarvaTimeFrame(timeIndex, *larvaIt);
					Vector2D moment = larva->getCenter();
					moment.setX(s.toDouble());
					larva->setCenter(moment);
				}
				larvaIt++;
			}
		}	
		//y-Coordinate of center
		else if(_line.startsWith("mass_y"))
		{			
			for each(QString s in values)
			{
				if(!s.isEmpty())
				{
					LarvaModel* larva = this->getLarvaTimeFrame(timeIndex, *larvaIt);
					Vector2D moment = larva->getCenter();
					moment.setY(s.toDouble());
					larva->setCenter(moment);
				}
				larvaIt++;
			}
		}	
		else
		{
			//Ignore
			return false;
		}
	
		return true;	
	}

	LarvaModel*	ImportFromCSV::getLarvaTimeFrame	(
		const unsigned int		_timeIndex, 
		LarvaModel				*	_larva)
	{
		if(_larva->isResolved())
		{
			while(_larva->getTimeIndex() < _timeIndex)
			{
				if(!_larva->hasNext())
				{
					_larva->setNext(new LarvaModel(false));
				}
				_larva = _larva->getNext();
			}				
		}
		//unresolved means this will be the first timeindex 
		//where this model is detected
		_larva->setResolved(true);
		_larva->setTimeIndex(_timeIndex);
		return _larva;
	}

	void ImportFromCSV::setupLarvaSpine(
		const unsigned int		_finalSpineIndex, 
		LarvaModel				*	_larva)
	{
		vector< double >* radi		= _larva->getWidths();
		vector< Vector2D >* spine	= _larva->getSpine();

		while(_finalSpineIndex >= radi->size())
		{
			radi->push_back(0);
		}
		while(_finalSpineIndex >= spine->size())
		{
			spine->push_back(Point());
		}
	}
}
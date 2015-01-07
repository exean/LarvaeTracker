/**
 *  @file LarvaModel.h
 *
 *	@section DESCRIPTION
 *	Model of a larva at a specific time step.
 */

#pragma once

#include "Vector2D.h"
using namespace cv;
/**
 *	Model of a larva at a specific time step.
 */
class LarvaModel
{
public:
#pragma region Con-/Destructor
	/**
	* Constructor
	*
	* @param	_resolved	true, if creator agrees to fill this model with information, false if its just a placeholder.
	*/
	LarvaModel(bool _resolved = true)
	{
		this->init();
		this->mResolved		= _resolved;
		this->mTimeIndex	= 0;
	};

	/**
	* Copy-Constructor
	*
	* @param	_copy			Model to be copied
	* @param	_copySuccessors	Determines wether all successors shall be copied or simply linked.
	*/
	LarvaModel(LarvaModel* _copy, bool _copySuccessors = false)
	{
		this->mTimeIndex		= _copy->getTimeIndex();
		this->mResolved			= _copy->isResolved();
		this->mPrevious			= _copy->getPrev();

		if(_copySuccessors && _copy->hasNext())
		{
			LarvaModel* next = new LarvaModel(_copy->getNext(), true);
			this->setNext(next);
		}
		else
		{
			this->mNext	= _copy->getNext();
		}		
	
		this->mSegmentWidth		= vector<double>(*_copy->getWidths());
		this->mSpine			= vector<Vector2D>(*_copy->getSpine());
		this->mContour			= vector<Point>(_copy->getContour());
	
		this->mCenter			= _copy->getCenter();

		this->mInterpolated		= _copy->mInterpolated;
	
		this->mIntensityHead	= _copy->mIntensityHead;
		this->mIntensityTail	= _copy->mIntensityTail;
		//this->mSpineLength		= _copy->mSpineLength;
	};
	~LarvaModel(void)
	{
	};
#pragma endregion	
	void			deleteChildren() { if(this->mNext){ this->mNext->deleteChildren(); delete this->mNext; this->mNext = 0; }	};

#pragma region Checker
	bool			hasNext()							const	{ return this->mNext!= 0;									};
	bool			hasPrev()							const	{ return this->mPrevious != 0;								};
	bool			isCollision()						const	{ return this->mInterpolated;								};
	bool			isResolved()						const	{ return this->mResolved;									};
#pragma endregion

#pragma region Getter/Setter
	void			setTimeIndex	(unsigned int _time)		{ this->mTimeIndex = _time;									};
	unsigned int	getTimeIndex	()					const	{ return this->mTimeIndex;									};
	void			setResolved		(bool _resolved)			{ this->mResolved = _resolved;								};
	void			setCollision	(bool _col)					{ this->mInterpolated = _col;								};
	LarvaModel*		getPrev			()					const	{ return this->mPrevious;									};
	void			rmPointerToPrev	()							{ this->mPrevious = 0;										};
	void			setNext			(LarvaModel *_next)			{ this->mNext = _next; if(_next) {this->mNext->mPrevious = this; this->mNext->setTimeIndex(this->mTimeIndex+1);}};
	LarvaModel*		getNext			()					const	{ return this->mNext;										};
	LarvaModel*		getLast			()							{ return (this->mNext ? this->mNext->getLast() : this);	};
	LarvaModel*		getFirst		()							{ return (this->mPrevious ? this->mPrevious->getFirst() : this);};
	vector<Vector2D>*getSpine		()							{ return &this->mSpine;									};
	void			setSpine		(vector<Vector2D> _s)		{ this->mSpine = _s;										};
	vector<Point>	getContour		()					const	{ return this->mContour;									};
	void			setContour		(vector<Point> _c)			{ this->mContour = _c;										};
	void			setWidths		(vector<double> _w)			{ this->mSegmentWidth = _w;									};
	vector<double>*	getWidths		()							{ return &(this->mSegmentWidth);							};
	Vector2D		getCenter		()					const	{ return this->mCenter;									};
	void			setCenter		(Vector2D _center)			{ this->mCenter = _center;									};
	
	Vector2D		getTail			()					const	{ return *(this->mSpine.end()-1);							};
	void			setTail			(Vector2D _tail)			{ if(this->mSpine.size() > 1) this->mSpine[this->mSpine.size()-1] = _tail; else this->mSpine.push_back(_tail); };
	void 			setTailIntensity(double _i)					{ this->mIntensityTail = _i;								};
	double			getTailIntensity()					const	{ return this->mIntensityTail;								};
	
	Vector2D		getHead			()					const	{ return this->mSpine[0];									};
	void			setHead			(Vector2D _head)			{ if(this->mSpine.size() > 0) this->mSpine[0] = _head; else this->mSpine.push_back(_head); };
	void 			setHeadIntensity(double _i)					{ this->mIntensityHead = _i;								};
	double			getHeadIntensity()					const	{ return this->mIntensityHead;								};
#pragma endregion

private:
	void init()
	{
		this->mSegmentWidth	= 	vector<double>();
		this->mSpine		= 	vector<Vector2D>();
		this->mNext			=	0;
		this->mPrevious		=	0;
		this->mInterpolated	=	false;
	};

#pragma region Fields
	/**
    * Pointer to model of this larva in t-1
    **/
	LarvaModel*		mPrevious;

	/**
    * Intensity of head-region read from input image
    **/
	double			mIntensityHead;

	/**
    * Intensity of tail-region read from input image
    **/
	double			mIntensityTail;

	/**
    * Overall mSpine-length
    **/
	//double			mSpineLength;

	/**
    * Is this an interpolated model?
    **/
	bool			mInterpolated;

	/**
    * Is this model fully recognized (valid values for mSpine, widths,...)
    **/
	bool			mResolved;

	/**
    * Pointer to model of this larva in t+1
    **/
	LarvaModel*		mNext;

	/**
    * Width of each spine point
    **/
	vector<double>	mSegmentWidth;

	/**
    * a polyline which is the spine
    **/
	vector<Vector2D>	mSpine;

	/**
    * Contour this model is fitted into
    **/
	vector<Point>	mContour;
	
	/**
    * Contour's center of mass
    **/
	Vector2D		mCenter;

	/**
    * Time index corresponds to the index of the input image this larva was found on
    **/
	unsigned int	mTimeIndex;
#pragma endregion
};


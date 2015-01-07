/**
 *  @file Vector2D.h
 *
 *  @section DESCRIPTION
 *  Double-precision 2D-Vector class.
 *	Completely declared inline for easier
 *	usage among multiple projects.
 */
#pragma once
#include <opencv2/opencv.hpp>
using namespace std;
/**
 *  Double-precision 2D-Vector class.
 *	Completely declared inline for easier
 *	usage among multiple projects.
 */
class Vector2D
{
	
public:
	/**
	* Constructor
	*
	* @param _x	First value of vector
	* @param _y	Second value of vector
	*/
	Vector2D(double _x = 0, double _y = 0)
	{
		this->mX = _x;
		this->mY = _y;
	};

	/**
	* Constructor
	*
	* @param _p	Values from this point will be used 
	*			as vector-values.
	*/
	Vector2D(cv::Point _p)
	{
		this->mX = (double)_p.x;
		this->mY = (double)_p.y;
	};
	~Vector2D() {} ;
	
#pragma region getter/setter
	void setX(double _x)
	{
		this->mX = _x;
	}

	void setY(double _y)
	{
		this->mY = _y;
	}

	double  getX()	const
	{
		return this->mX;
	}

	double getY()	const
	{
		return this->mY;
	}
#pragma endregion

	/**
	* Rounds given number.
	* @param _number Number to round
	* @return Rounded number.
	*/
	double round(const double _number) const
	{ 
		return floor(_number+0.5); 
	};
	
#pragma region operators
	double operator*(const Vector2D & v) const
	{
		return this->mX * v.mX + this->mY * v.mY;
	}

	Vector2D operator*(const double _scalar) const
	{
		return Vector2D(
			(double)(this->mX * _scalar), 
			(double)(this->mY * _scalar));
	}

	Vector2D operator*(const float _scalar) const
	{
		return Vector2D(
			this->mX * _scalar, 
			this->mY * _scalar);
	}

	Vector2D operator*(const int _scalar) const
	{
		return Vector2D(
			this->mX * _scalar, 
			this->mY * _scalar);
	}

	Vector2D operator*=(const double _scalar)
	{
		this->mX *= _scalar;
		this->mY *= _scalar;
		return Vector2D(this->mX, this->mY);
	}

	Vector2D operator+(const Vector2D &_vect) const
	{
		return Vector2D(
			this->mX + _vect.mX, 
			this->mY + _vect.mY);
	}	
	
	Vector2D operator+=(const Vector2D &_vect)
	{
		this->mX += _vect.mX, 
		this->mY += _vect.mY;
		return Vector2D(
			this->mX, 
			this->mY);
	}	
	
	Vector2D operator-(const Vector2D &_vect) const
	{
		return Vector2D(
			this->mX - _vect.mX, 
			this->mY - _vect.mY);
	}
	
	Vector2D operator-=(const Vector2D &_vect)
	{
		this->mX -= _vect.mX, 
		this->mY -= _vect.mY;
		return Vector2D(
			this->mX, 
			this->mY);
	}
#pragma endregion
	/**
	* Rotates this vector
	* @param _angle	Rotation angle in radians
	*/
	void rotate(const double _angle)
	{
		double cosangle = cos(_angle);
		double sinangle = sin(_angle);
		double xt = (this->mX * cosangle) - (this->mY * sinangle);
		double yt = (this->mY * cosangle) + (this->mX * sinangle);
		this->mX = xt;
		this->mY = yt;
	}
	
	/**
	* @return Cross-product 
	*/
	double cross(const Vector2D &_vect) const
	{
		return this->mX * _vect.getY() - this->mY * _vect.getX();
	}
	
	/**
	* @return Length of this vector
	*/
	double length()
	{
		return sqrt(this->mX * this->mX + this->mY * this->mY);
	}
	
	/**
	* Normalizes this vector.
	*/
	Vector2D * normalize()
	{
		double length = this->length();
		if(length != 0)
		{
			this->mX /= length;
			this->mY /= length;
		}
		return this;
	}
	
	/**
	* @return Dot-product 
	*/
	double dot(const Vector2D &_vect) const
	{
		return (this->mX * _vect.mX) + (this->mY * _vect.mY);
	}
	
	/**
	* @return An orthogonal vector.
	*/
	Vector2D getOrthogonalVector()
	{
		return Vector2D(-this->mY, this->mX);
	}

	/**
	* @return Point with rounded values of vector.
	*/
	cv::Point toPoint() const
	{
		return cv::Point(
			(int)this->round(this->mX),
			(int)this->round(this->mY));
	}	

private:	
	/**
	* First vector value
	*/
	double mX;

	/**
	* Second vector value
	*/
	double mY;


};
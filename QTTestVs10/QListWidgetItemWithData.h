/**
 *  @file QListWidgetItemWithData.h
 *
 *	@section DESCRIPTION
 *	Extends QListWidgetItem with a field for data.
 */
#pragma once
#include <QListWidget>
template <typename T>
/**
 *	Extends QListWidgetItem with a field for data.
 */
class QListWidgetItemWithData : public QListWidgetItem
{
public:
	QListWidgetItemWithData(
		QString & _text, 
		QListWidget * _view, 
		T _data)
		: 
		QListWidgetItem(
			_text, 
			_view)
	{
		this->mData = _data;
	}
	~QListWidgetItemWithData(void){};

	/**
	* @return Stored data
	*/
	T* getData() { return &this->mData; };

private:
	/**
	* Data saved additional to standard QListWidgetItem functionality.
	*/
	T mData;
};


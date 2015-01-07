#include "ImageLoader.h"

namespace PreprocessingFuncs
{
	ImageLoader::ImageLoader(
		vector< string >					_files,
		Dll::ABackgroundSubtractor		*	_bgSub,
		cv::Rect							_roi,
		Preprocessing					*	_prep)
	{
		this->mFiles	= _files;
		this->mRoi		= _roi;
		this->mPrep		= _prep;
		this->mBgSub	= _bgSub;
	}


	ImageLoader::~ImageLoader(void)
	{
		//if(this->mBgSub)	delete this->mBgSub;
		//if(this->mPrep)		delete this->mPrep;
	}

	void ImageLoader::initialize()
	{
		//Count files
		this->mFileCount = this->mFiles.size();
		//Setup backgroundsubtractor
		if(this->mBgSub)
		{
			this->mBgSub->create(this->mFiles, this->mRoi);
		}
		//Initialize "last-processed"-field
		this->mLastIndex = -1;
	}

	bool ImageLoader::getNext(
		cv::Mat				&	out_img,
		cv::Mat				&	out_binary)
	{
		//Load image of next index
		return	this->get(
			this->mLastIndex+1,
			out_img,
			out_binary);
	}

	bool ImageLoader::get(
		const int				_index,
		cv::Mat				&	out_img,
		cv::Mat				&	out_binary)
	{
		//Check if requested index is in bounds
		if(_index >= this->mFileCount || _index < 0)
		{
			return false;
		}

		//Update index
		this->mLastIndex = _index;

		//Load image
		out_img = cv::imread(this->mFiles[this->mLastIndex])(this->mRoi);

		//Subtract background
		if(this->mBgSub)
		{
			this->mBgSub->apply(out_img);
		}

		//Apply preprocessing
		out_binary = this->mPrep->getBinaryFBImage(out_img);

		return true;
	}

	int ImageLoader::getFileCount()
	{
		return this->mFileCount;
	}

	std::string ImageLoader::getCurrentFileName()
	{
		if(this->mLastIndex >= this->mFileCount || this->mLastIndex < 0)
		{
			return "";
		}
		return this->mFiles[this->mLastIndex];
	}
}

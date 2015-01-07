#include "MinBackgroundSubtraction.h"


MinBackgroundSubtraction::MinBackgroundSubtraction(void)
{
	//Plugin information
	this->mName = "Temporaler Minimum-Filter";
	this->mAuthor = "Tobias Ries (tobias@ries.asia)";
	this->mDescription = "Subtrahiert den zeitliche Minimalwert von jedem Pixel";
	this->mVersion = 1;
	//Plugin information - EOF
}


MinBackgroundSubtraction::~MinBackgroundSubtraction(void)
{
}

	/**
    * Subtracts the mask from the image.
    *
	* @param	mod_img		Image on which the background 
	*						subtraction will be applied.
    * @return	true, if subtraction could be applied.
    */
bool MinBackgroundSubtraction::apply(cv::Mat & mod_img)
{
	//Check if initialized and input image matches mask size
	if(this->mReady && this->mMask.size() == mod_img.size())
	{
		//Subtract mask from input image
		mod_img -= this->mMask;
		return true;
	}
	return false;
}

	/**
    * Creates a mask from the median of provided images.
    *
	* @param	_files		Images to be used.
	* @param	_roi		Region to be used.
    */
void MinBackgroundSubtraction::create(
	const std::vector<std::string>	& _files,
	const cv::Rect					& _roi)
{
	//Count number of input images
	const int		fileCount		= _files.size();

	//Determine amount of images to be used for mask creation
	const double	sampleCount		= (int)(fileCount * mSampleRate);

	//Determine distance between images of input-list to be used
	const double	sampleStep		= (1 / mSampleRate);

	//Index of currently used sample from input-list
	int				fileIndex		= 0;

	//Number of columns in input image (width)
	int				numLines;

	//Number of rows in input image (height)
	int				numCols;

	//Load image
	this->mMask = cv::imread(_files[0])(_roi);	

	//Determine image dimensions
	numLines = this->mMask.rows;
	numCols = this->mMask.cols*this->mMask.channels();

	//Used determined amount of samples to create the mask
	for(int s = 1;
		s < sampleCount;
		s++)
	{
		//Compute index of current sample within input-list
		fileIndex = (int)(s*sampleStep);

		//Load current sample
		cv::Mat image = cv::imread(_files[fileIndex])(_roi);

		//Get pointer to data structure of current sample
		uchar * imageData = image.data;

		//insert data
		int i = 0;
		for(int line = 0; line < numLines; line++)
		{
			//Pointers to corresponding data of sample and mask
			uchar* data = image.ptr<uchar>(line);
			uchar* maskData = this->mMask.ptr<uchar>(line);

			//Update mask with current sample
			for(int col = 0; col < numCols; col++)
			{
				//Always use minimal value (estimated background intensity is minimal)
				maskData[col] = std::min(data[col], maskData[col]);
			}
		}

		image.release();
	}

	//Mask-creation finished, set flag to ready
	this->mReady = true;
}
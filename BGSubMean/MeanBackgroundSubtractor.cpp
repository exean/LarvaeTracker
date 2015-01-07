#include "MeanBackgroundSubtractor.h"


MeanBackgroundSubtractor::MeanBackgroundSubtractor(void)
	: mMask(0)
{
	//Plugin information
	this->mName = "Temporaler Mittelwert-Filter";
	this->mAuthor = "Tobias Ries (tobias@ries.asia)";
	this->mDescription = "Subtrahiert zeitliche Mittelwerte von jedem Pixel";
	this->mVersion = 1;
	//Plugin information - EOF
}


MeanBackgroundSubtractor::~MeanBackgroundSubtractor(void)
{
	if(this->mMask) delete this->mMask;
}

bool MeanBackgroundSubtractor::apply(cv::Mat & mod_img)
{
	if(this->mReady
		&& this->mMask 
		&& this->mMask->size() == mod_img.size())
	{
		mod_img -= *this->mMask;
		return true;
	}
	return false;
}

void MeanBackgroundSubtractor::create(
	const std::vector<std::string>	& _files,
	const cv::Rect					& _roi)
{
	if(this->mMask) delete this->mMask;
	this->mMask = NULL;

	double			*values			= 0;
	const int		fileCount		= _files.size();
	const double	sampleCount		= (int)(fileCount * mSampleRate);
	const double	sampleStep		= (1 / mSampleRate);
	int				fileIndex		= 0;
	int				numLines;
	int				numCols;
	cv::Mat			image;

	for(int s = 0;
		s < sampleCount;
		s++)
	{
		fileIndex = (int)(s*sampleStep);

		//Load image
		cv::Mat image = cv::imread(_files[fileIndex])(_roi);	
		uchar * imageData = image.data;

		if(!this->mMask)
		{
			//Create mask on first loaded image
			numLines = image.rows;
			numCols = image.cols*image.channels();
			this->mMask = new cv::Mat(image.size(), image.type());
			values = new double[numLines*numCols];

			//Fill with initial values
			int i = 0;
			for(int line = 0; line < numLines; line++)
			{
				uchar* data = image.ptr<uchar>(line);
				for(int col = 0; col < numCols; col++)
				{
					values[i++] = data[col] / sampleCount;
				}
			}
		}
		else
		{
			//add values to mask
			int i = 0;
			for(int line = 0; line < numLines; line++)
			{
				uchar* data = image.ptr<uchar>(line);
				for(int col = 0; col < numCols; col++)
				{
					values[i++] += data[col] / sampleCount;
				}
			}
		}

		image.release();
	}

	int i = 0;
	for(int line = 0; line < numLines; line++)
	{
		uchar* data = this->mMask->ptr<uchar>(line);
		for(int col = 0; col < numCols; col++)
		{
			//make sure all values are in bounds
			data[col] = std::min(255, (int)values[i++]);
		}
	}
	delete values;
	this->mReady = true;
}
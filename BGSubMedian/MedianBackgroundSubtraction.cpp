#include "MedianBackgroundSubtraction.h"


MedianBackgroundSubtraction::MedianBackgroundSubtraction(void)
	: mMask(0)
{
	//Plugin information
	this->mName = "Temporaler Median-Filter";
	this->mAuthor = "Tobias Ries (tobias@ries.asia)";
	this->mDescription = "Subtrahiert zeitliche Medianwerte von jedem Pixel";
	this->mVersion = 1;
	//Plugin information - EOF
}


MedianBackgroundSubtraction::~MedianBackgroundSubtraction(void)
{
	if(this->mMask) delete this->mMask;
}

bool MedianBackgroundSubtraction::apply(cv::Mat & mod_img)
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

void MedianBackgroundSubtraction::create(
	const std::vector<std::string>	& _files,
	const cv::Rect					& _roi)
{
	if(this->mMask) delete this->mMask;
	this->mMask = NULL;	

	const int	fileCount		= _files.size();
	const int	sampleCount		= (int)(fileCount * mSampleRate);

	const float sampleStep		= (1 / mSampleRate);
	const int	size_2			= sampleCount / 2;
	const bool oddSampleCount	= (0 != (sampleCount % 2));
	int			numLines;
	int			numCols;
	uchar **	values;
	int			size;
	cv::Mat		image;
	int			fileIndex		= 0;

	//First sample (initialization)
	image			= cv::imread(_files[fileIndex])(_roi);	//Load image
	numLines		= image.rows;
	numCols			= image.cols * image.channels();
	size			= numLines * numCols;
	this->mMask		= new cv::Mat(image.size(), image.type());
	values			= new uchar*[size];
			
	int i = 0;
	for(int line = 0; line < numLines; line++)
	{
		uchar* data	= image.ptr<uchar>(line);
		for(int col = 0; col < numCols; col++)
		{
			values[i]	= new uchar[size_2+1];
			values[i][0] = data[col];
			i++;
		}
	}

	//sample until last
	int sampleCount_1 = sampleCount - 1;
	for(int sampleIndex = 1;
		sampleIndex < sampleCount_1;
		sampleIndex++)
	{
		fileIndex = (int)(sampleIndex*sampleStep);
		image = cv::imread(_files[fileIndex])(_roi);;	//Load image
			
		int finalSortInsertionIndex = std::min(size_2, sampleIndex);

		int i = 0;
		for(int line = 0; line < numLines; line++)
		{
			uchar* data	= image.ptr<uchar>(line);
			for(int col = 0; col < numCols; col++)
			{
				//Insert sorted
				uchar * currentPixValue = values[i];
				int currentval = data[col];
				//Note: sampleIndex = entrypoint of current value without sort,
				//		x >= entrypoint are invalid entries in current value-array
				for(int sortInd = 0; sortInd <= finalSortInsertionIndex; sortInd++)
				{
					if(sortInd != sampleIndex && currentPixValue[sortInd] < currentval)
					{
						continue;
					}
					else
					{
						for(int moveInd = size_2-1; moveInd >= sortInd; moveInd--)
						{
							currentPixValue[moveInd+1] = currentPixValue[moveInd];
						}
						currentPixValue[sortInd] = currentval;
					}
				}
				i++;
			}
		}

		image.release();
	}

	//Last sample (cleanup)
	fileIndex = (int)((sampleCount-1)*sampleStep);
	image = cv::imread(_files[fileIndex])(_roi);	//Load image
	if(oddSampleCount)
	{
		int i = 0;
		for(int line = 0; line < numLines; line++)
		{
			uchar* data			= image.ptr<uchar>(line);
			uchar* resultdata	= this->mMask->ptr<uchar>(line);
			for(int col = 0; col < numCols; col++)
			{
				//Insert sorted
				uchar * currentPixValue = values[i];
				int currentval = data[col];

				//Note: sampleIndex = entrypoint of current value without sort,
				//		x >= entrypoint are invalid entries in current value-array
				for(int sortInd = 0; sortInd <= size_2; sortInd++)
				{
					if(currentPixValue[sortInd] < currentval)
					{
						continue;
					}
					else
					{
						for(int moveInd = size_2-1; moveInd >= sortInd; moveInd--)
						{
							currentPixValue[moveInd+1] = currentPixValue[moveInd];
						}
						currentPixValue[sortInd] = currentval;
					}
				}
				resultdata[col] = currentPixValue[size_2];
				delete currentPixValue;
				i++;
			}
		}
	}
	else
	{
		int i = 0;
		for(int line = 0; line < numLines; line++)
		{
			uchar* data	= image.ptr<uchar>(line);			
			uchar* resultdata	= this->mMask->ptr<uchar>(line);
			for(int col = 0; col < numCols; col++)
			{
				//Insert sorted
				uchar * currentPixValue = values[i];
				int currentval = data[col];
				//Note: sampleIndex = entrypoint of current value without sort,
				//		x >= entrypoint are invalid entries in current value-array
				for(int sortInd = 0; sortInd <= size_2; sortInd++)
				{
					if(currentPixValue[sortInd] < currentval)
					{
						continue;
					}
					else
					{
						for(int moveInd = size_2-1; moveInd >= sortInd; moveInd--)
						{
							currentPixValue[moveInd+1] = currentPixValue[moveInd];
						}
						currentPixValue[sortInd] = currentval;
					}
				}
				resultdata[col] = (currentPixValue[size_2 - 1] + currentPixValue[size_2]) / 2;
				delete currentPixValue;
				i++;
			}
		}
	}
	image.release();
	delete values;

	this->mReady = true;
}
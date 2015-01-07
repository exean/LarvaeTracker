//#include "BackgroundSubtractorMedian.h"
//
//namespace BackgroundSubtraction
//{
//	BackgroundSubtractorMedian::BackgroundSubtractorMedian()
//	{
//		this->mask = 0;
//	}
//
//
//	BackgroundSubtractorMedian::~BackgroundSubtractorMedian(void)
//	{
//		if(this->mask) delete this->mask;
//	}
//
//	void BackgroundSubtractorMedian::apply(cv::Mat & img)
//	{
//		if(this->mask)
//		{
//			img -= *this->mask;
//		}
//	}
//
//	void BackgroundSubtractorMedian::create(
//		const std::vector<std::string>	& files,
//		const cv::Rect					& roi)
//	{
//		if(this->mask) delete this->mask;
//
//		const int	fileCount		= files.size();
//		const float sampleCount	= (int)(fileCount * mSampleRate);
//		const float sampleStep		= (1 / mSampleRate);
//		const int	size_2			= sampleCount / 2;
//		const bool oddSampleCount	= (int)sampleCount % 2;
//		uchar ** values;
//		uchar * resultValues;
//		int size;
//		cv::Mat image;
//		int fileIndex = 0;
//
//		//First sample (initialization)
//		image = GeneralTrackerFuncs::Toolbox::LoadImage(files[fileIndex], roi);	//Load image
//		uchar * imageData = image.data;
//		if(!this->mask)
//		{
//			size = image.rows*image.cols*image.channels();
//			this->mask = new cv::Mat(image.size(), image.type());
//			values = new uchar*[size];
//			resultValues = this->mask->data;
//			for(int i = 0; i < size; i++)
//			{
//				values[i] = new uchar[size_2+1];
//				resultValues[i] = values[i][0] = imageData[i];
//			}
//		}
//
//		//sample until last
//		int sampleCount_1 = sampleCount - 1;
//		for(int sampleIndex = 1;
//			sampleIndex < sampleCount_1;
//			sampleIndex++)
//		{
//			fileIndex = (int)(sampleIndex*sampleStep);
//			image = GeneralTrackerFuncs::Toolbox::LoadImage(files[fileIndex], roi);	//Load image
//			imageData = image.data;
//			
//			int finalSortInsertionIndex = std::min(size_2, sampleIndex);
//
//			for(int i = 0; 
//				i < size; 
//				i++)
//			{
//				//Insert sorted
//				uchar * currentPixValue = values[i];
//				int currentval = imageData[i];
//				//Note: sampleIndex = entrypoint of current value without sort,
//				//		x >= entrypoint are invalid entries in current value-array
//				for(int sortInd = 0; sortInd <= finalSortInsertionIndex; sortInd++)
//				{
//					if(sortInd != sampleIndex && currentPixValue[sortInd] < currentval)
//					{
//						continue;
//					}
//					else
//					{
//						for(int moveInd = size_2-1; moveInd >= sortInd; moveInd--)
//						{
//							currentPixValue[moveInd+1] = currentPixValue[moveInd];
//						}
//						currentPixValue[sortInd] = currentval;
//					}
//				}
//			}
//			image.release();
//		}
//
//		//Last sample (cleanup)
//		fileIndex = (int)(sampleCount*sampleStep);
//		image = GeneralTrackerFuncs::Toolbox::LoadImage(files[fileIndex], roi);	//Load image
//		imageData = image.data;
//		if(oddSampleCount)
//		{
//			for(int i = 0; 
//				i < size; 
//				i++)
//			{
//				//Insert sorted
//				uchar * currentPixValue = values[i];
//				int currentval = imageData[i];
//				//Note: sampleIndex = entrypoint of current value without sort,
//				//		x >= entrypoint are invalid entries in current value-array
//				for(int sortInd = 0; sortInd <= size_2; sortInd++)
//				{
//					if(currentPixValue[sortInd] < currentval)
//					{
//						continue;
//					}
//					else
//					{
//						for(int moveInd = size_2-1; moveInd >= sortInd; moveInd--)
//						{
//							currentPixValue[moveInd+1] = currentPixValue[moveInd];
//						}
//						currentPixValue[sortInd] = currentval;
//					}
//				}
//				resultValues[i] = currentPixValue[size_2];
//				delete currentPixValue;
//			}
//		}
//		else
//		{
//			for(int i = 0; 
//				i < size; 
//				i++)
//			{
//				//Insert sorted
//				uchar * currentPixValue = values[i];
//				int currentval = imageData[i];
//				//Note: sampleIndex = entrypoint of current value without sort,
//				//		x >= entrypoint are invalid entries in current value-array
//				for(int sortInd = 0; sortInd <= size_2; sortInd++)
//				{
//					if(currentPixValue[sortInd] < currentval)
//					{
//						continue;
//					}
//					else
//					{
//						for(int moveInd = size_2-1; moveInd >= sortInd; moveInd--)
//						{
//							currentPixValue[moveInd+1] = currentPixValue[moveInd];
//						}
//						currentPixValue[sortInd] = currentval;
//					}
//				}
//				resultValues[i] = (currentPixValue[size_2 - 1] + currentPixValue[size_2]) / 2;
//				delete currentPixValue;
//			}
//		}
//		image.release();
//		delete values;
//
//		cv::namedWindow("Mask");cv::imshow("Mask", *this->mask);
//
//		//if (oddSampleCount)
//		//{
//		//	for(int i = 0; 
//		//		i < size; 
//		//		i++)
//		//	{
//		//		uchar * v = values[i];
//		//		std::sort(v, v+sampleCount);
//		//		this->mask->data[i] = v[size_2];
//		//		delete v;
//		//	}
//		//	
//		//}
//		//else 
//		//{
//		//	for(int i = 0; 
//		//		i < size; 
//		//		i++)
//		//	{
//		//		uchar * v = values[i];
//		//		std::sort(v, v+sampleCount);
//		//		this->mask->data[i] = (v[size_2 - 1] + v[size_2]) / 2;
//		//		delete v;
//		//	}
//		//}
//		this->mReady = true;
//	}
//}
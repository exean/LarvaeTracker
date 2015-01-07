/**
 *  @file ImageLoader.h
 *
 *	@section DESCRIPTION
 *	After being initialized with a list of files and
 *	a set of preprocessing methods, this class provides
 *	easy access to image-files used as input for 
 *	tracking algorithms.
 */

#pragma once
#include "..//qttestvs10//ABackgroundSubtractor.h"
#include "Preprocessing.h"
namespace PreprocessingFuncs
{
	/**
	 *	After being initialized with a list of files and
	 *	a set of preprocessing methods, this class provides
	 *	easy access to image-files used as input for 
	 *	tracking algorithms.
	 */
	class ImageLoader
	{
	public:
		/**
        * Constructor
        *
		* @param	_files Complete list of all input-files
		*			the tracking algorithm shall work on.
		* @param	_bgSub Background-subtractor to be applied
		*			to each image.
		* @param	_roi Region of interest specifying the
		*			the sub-image to be provided through this
		*			class.
        * @param	_prep Object that applies preprocessing
		*			methods to every image loaded through
		*			this class.
        */
		ImageLoader(			
			vector< string >					_files,
			Dll::ABackgroundSubtractor		*	_bgSub,
			cv::Rect							_roi,
			Preprocessing					*	_prep);
		~ImageLoader(void);

		/**
        * Initializes the objects fields and background-
        * subtractor.
        */
		void		initialize();

		/**
        * Load the image-data successive to the last loaded
		* data (according to provided file-list).
        *
		* @param	out_img Loaded image file.
		* @param	out_binary Image with applied preprocessing
		*			(including background-subtraction).
        * @return	true, if a successive file could be loaded.
        */
		bool		getNext(
			cv::Mat							&	out_img,
			cv::Mat							&	out_binary);

		/**
        * Load the image-data of soecified index
		* (according to provided file-list).
        *
		* @param	_index Index of image-file to bea loaded.
		* @param	out_img Loaded image file.
		* @param	out_binary Image with applied preprocessing
		*			(including background-subtraction).
        * @return	true, if a file of given index could be loaded.
        */
		bool		get(
			const int							_index,
			cv::Mat							&	out_img,
			cv::Mat							&	out_binary);

		/**
        * Provides the number of files in this objects file-list.
        *
        * @return	Amount of loadable image-files.
        */
		int			getFileCount();

		/**
        * Provides the name of last loaded file.		
        *
        * @return	Name of last loaded image file.
        */
		std::string getCurrentFileName();

	private:
		/**
        * List of all files this object offers to load.        
        */
		std::vector< std::string >			mFiles;
		/**
        * Object handling background-subtraction.
        */
		Dll::ABackgroundSubtractor		*	mBgSub;
		/**
        * Region of interest specifying the part of
		* any file that will be processed by this class.
        */
		cv::Rect							mRoi;
		/**
        * Object handling preprocessing of images.
        */
		Preprocessing					*	mPrep;
		/**
        * Size of mFiles.
        */
		int									mFileCount;
		/**
        * Index of last loaded file in mFiles.
        */
		int									mLastIndex;
	};
}

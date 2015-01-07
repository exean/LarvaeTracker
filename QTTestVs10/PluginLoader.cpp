#include "PluginLoader.h"

namespace Dll
{
	#pragma region Singleton
	// Global static pointer used to ensure a single instance of the class.
	PluginLoader* PluginLoader::mInstance = NULL; 

	PluginLoader* PluginLoader::Instance()
	{
	   if (!mInstance)   // Only allow one instance of class to be generated.
	   {
		  mInstance = new PluginLoader;
	   }
	   return mInstance;
	}
	#pragma endregion

	PluginLoader::PluginLoader(void)
	{
		this->reloadPlugins();
	}

	PluginLoader::~PluginLoader(void)
	{
		for each(APlugin* plugin in this->mSingeInstancePlugins)
		{
			delete plugin;
		}
	}

	void PluginLoader::reloadPlugins()
	{
		this->mTracker.clear();
		this->mBackgroundSubtractor.clear();
		this->mUnspecifiedPlugins.clear();

		//Get a list of all dlls in program dir (could be changed for
		//different directories for different plugin types).
		QString s = QApplication::applicationDirPath()+QString(QDir::separator()) + "Plugins" + QString(QDir::separator());
		QDir myDir(s);
		QStringList dllList = myDir.entryList(QStringList("*.dll"));
	
		//Try to load plugins
		for(QList<QString>::iterator dllIt = dllList.begin();
			dllIt != dllList.end();
			dllIt++)
		{
			if(std::find(
				this->mSingeInstancePluginFiles.begin(),
				this->mSingeInstancePluginFiles.end(),
				*dllIt) != this->mSingeInstancePluginFiles.end())
			{
				//Some plugins may only be loaded once!
				continue;
			}

			wstring cdll = (s+*dllIt).toStdWString();//L".../TrackerCurvature.dll"

			HINSTANCE dllHandle = LoadLibrary(cdll.c_str());
			if(dllHandle)
			{
				CreateTrackerFunc cr3at0r = (CreateTrackerFunc) ::GetProcAddress (dllHandle, "CreateTracker");	
				if(cr3at0r)
				{
					this->mTracker.push_back(cr3at0r);
				}
				else
				{
					CreateBackgroundSubtractorFunc cr3at0r = 
						(CreateBackgroundSubtractorFunc) ::GetProcAddress (dllHandle, "CreateBackgroundSubtractor");	
					if(cr3at0r!=NULL)
					{
						this->mBackgroundSubtractor.push_back(cr3at0r);
					}
					else
					{
						CreatePluginFunc cr3at0r = 
							(CreatePluginFunc) ::GetProcAddress (dllHandle, "CreatePlugin");	
						if(cr3at0r!=NULL)
						{
							APlugin* plugin = cr3at0r();
							if(plugin->isMultiInstancingAllowed())
							{
								this->mUnspecifiedPlugins.push_back(cr3at0r);
								delete plugin;
							}
							else
							{
								this->mSingeInstancePlugins.push_back(plugin);							
								this->mSingeInstancePluginFiles.push_back(*dllIt);
							}
						}
					}
				}
			}
		}
	}

	vector< ATracker* >	PluginLoader::getTrackers()
	{
		vector< ATracker* > result;
		for each(CreateTrackerFunc creator in this->mTracker)
		{
			result.push_back(creator());
		}
		return result;
	}

	vector< ABackgroundSubtractor* > PluginLoader::getBackgroundSubtractors()
	{
		vector< ABackgroundSubtractor* > result;
		for each(CreateBackgroundSubtractorFunc creator in this->mBackgroundSubtractor)
		{
			result.push_back(creator());
		}
		return result;
	}

	vector< APlugin* > PluginLoader::getAll()
	{
		vector< APlugin* > result(this->mSingeInstancePlugins);
		for each(CreateTrackerFunc creator in this->mTracker)
		{
			result.push_back(creator());
		}
		for each(CreateBackgroundSubtractorFunc creator in this->mBackgroundSubtractor)
		{
			result.push_back(creator());
		}
		for each(CreatePluginFunc creator in this->mUnspecifiedPlugins)
		{
			result.push_back(creator());
		}
		return result;
	}
}
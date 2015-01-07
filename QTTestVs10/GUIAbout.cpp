#include "GUIAbout.h"

namespace Gui
{
	GUIAbout::GUIAbout(QWidget *parent)
		: QDialog(parent)
	{
		this->mUi.setupUi(this);
		
		connect(this->mUi.pushOk, SIGNAL(clicked()), this, SLOT(hide()));

		this->insertPluginInfortmation();
	}

	GUIAbout::~GUIAbout(void)
	{
	}

	void GUIAbout::insertPluginInfortmation()
	{
		Dll::PluginLoader * p = Dll::PluginLoader::Instance();
		vector< Dll::APlugin* > plugins = p->getAll();

		QString text = "";

		for each(Dll::APlugin* plugin in plugins)
		{
			text.append(plugin->getName().c_str());
			text.append(" ( ");
			text.append(plugin->getAuthor().c_str());
			text.append(" )\n");

			if(plugin->isMultiInstancingAllowed())
			{
				delete plugin;
			}
		}

		this->mUi.labelPlugins->setText(text);
	}

	void GUIAbout::hide()
	{
		this->setVisible(false);
	}
}
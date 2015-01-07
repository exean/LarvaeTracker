#include "GUIParameterLayout.h"

namespace Gui
{
	GUIParameterLayout::GUIParameterLayout(
		QWidget							*	_parent, 
		std::vector<ParameterWrapper>	*	_parameters,
		const bool							_OkBtn,
		const bool							_CancelBtn)
		 :  QGridLayout(_parent)//QDialog(parent)
	{
		this->setColumnStretch(1, 1);
		this->setColumnMinimumWidth(1, 200);

		int rowIndex = 0;
		//Insert gui-elements for all parameters
		for(std::vector<ParameterWrapper>::iterator it = _parameters->begin();
			it != _parameters->end();
			it++)
		{
			QLabel *l = new QLabel(it->mName.c_str(), _parent);
			this->addWidget(l, rowIndex, 0);

			QWidget* input;
			switch(it->mType)
			{
			case ParameterWrapper::DOUBLE:				
				{
					QDoubleSpinBox * i = new QDoubleSpinBox(_parent);
					i->setRange(std::numeric_limits<double>::min(), std::numeric_limits<double>::max());
					i->setValue(it->mDouble);
					input = i;
					break;
				}
			case ParameterWrapper::FLOAT:
				{
					QDoubleSpinBox * i = new QDoubleSpinBox(_parent);
					i->setRange(std::numeric_limits<float>::min(), std::numeric_limits<float>::max());
					i->setValue(it->mFloat);
					input = i;
					break;
				}
			case ParameterWrapper::INT:
				{
					QSpinBox * i = new QSpinBox(_parent);
					i->setRange(std::numeric_limits<int>::min(), std::numeric_limits<int>::max());
					i->setValue(it->mInt);
					input = i;
					break;
				}
			case ParameterWrapper::BOOLEAN:
				{
					QCheckBox * i = new QCheckBox(_parent);
					i->setChecked(it->mBoolean);
					input = i;
					break;
				}
			case ParameterWrapper::SCALAR:
				{
					QGroupBox * i = new QGroupBox(_parent);

					QDoubleSpinBox * a = new QDoubleSpinBox(_parent);
					a->setRange(std::numeric_limits<double>::min(), std::numeric_limits<double>::max());
					a->setValue(it->mScalar[0]);
					a->setMaximumWidth(070);
					QDoubleSpinBox * b = new QDoubleSpinBox(i);
					b->setRange(std::numeric_limits<double>::min(), std::numeric_limits<double>::max());
					b->setValue(it->mScalar[1]);
					b->setMaximumWidth(70);
					QDoubleSpinBox * c = new QDoubleSpinBox(i);
					c->setRange(std::numeric_limits<double>::min(), std::numeric_limits<double>::max());
					c->setValue(it->mScalar[2]);
					c->setMaximumWidth(70);
					QDoubleSpinBox * d = new QDoubleSpinBox(i);
					d->setRange(std::numeric_limits<double>::min(), std::numeric_limits<double>::max());
					d->setValue(it->mScalar[3]);
					d->setMaximumWidth(70);

					 QHBoxLayout *vbox = new QHBoxLayout;
					 vbox->addWidget(a);
					 vbox->addWidget(b);
					 vbox->addWidget(c);
					 vbox->addWidget(d);
					 vbox->setMargin(0);
					 i->setLayout(vbox);					
					input = i;
					break;
				}
			default:
				{
					QLineEdit* i = new QLineEdit(_parent);
					i->setText(GUIParameterLayout::ParameterToString(*it));
					input = i;
					break;
				}
			}
			this->mParameters[&*it] = input;			
			this->addWidget(input, rowIndex, 1);

			l = new QLabel(it->mDescription.c_str(), _parent);
			l->setWordWrap(true);
			this->addWidget(l, rowIndex+1, 1);

			rowIndex += 2;
		}
	
		if(_CancelBtn)
		{
			QPushButton * pushCancel = new QPushButton("Abbrechen", _parent);
			this->addWidget(pushCancel, rowIndex, 0);
			connect(pushCancel,		SIGNAL(clicked()), this, SLOT(reject()));	
		}
		if(_OkBtn)
		{
			QPushButton * pushOk = new QPushButton("Ok", _parent);		
			this->addWidget(pushOk, rowIndex, 1);
			connect(pushOk,		SIGNAL(clicked()), this, SLOT(updateParams()));					
		}
	}


	GUIParameterLayout::~GUIParameterLayout(void)
	{
	}

	void GUIParameterLayout::setValues(
		const std::vector< ParameterWrapper > &		_algoParams)
	{
		for(std::map< ParameterWrapper*, QWidget* >::iterator it = this->mParameters.begin();
			it != this->mParameters.end();
			it++)
		{
			std::string value = it->first->mName;
			for each (ParameterWrapper param in _algoParams)
			{
				//Set values to correct fields by name
				if(!param.mName.compare(value))
				{
					switch(it->first->mType)
					{
					case ParameterWrapper::DOUBLE:
						{
							((QDoubleSpinBox*)it->second)->setValue(param.mDouble);						
							break;
						}
					case ParameterWrapper::FLOAT:
						{
							((QDoubleSpinBox*)it->second)->setValue(param.mFloat);						
							break;
						}
					case ParameterWrapper::INT:
						{
							((QSpinBox*)it->second)->setValue(param.mInt);						
							break;
						}
					case ParameterWrapper::BOOLEAN:
						{
							((QCheckBox*)it->second)->setChecked(param.mBoolean);						
							break;
						}
					case ParameterWrapper::SCALAR:
						{
							QLayout *layout = ((QGroupBox*)it->second)->layout();
							for(int i = 0; i < 4; i++)
							{
								((QDoubleSpinBox*)layout->itemAt(i)->widget())->setValue(param.mScalar[i]);
							}
							break;
						}
					default:
						{
							((QLineEdit*)it->second)->setText(GUIParameterLayout::ParameterToString(param));
							break;
						}
					}
					break;
				}
			}
		}
	}

	QString GUIParameterLayout::ParameterToString(
		const ParameterWrapper & _param)
	{
		switch(_param.mType)
		{
		case ParameterWrapper::DOUBLE:
			return QString::number(_param.mDouble);
		case ParameterWrapper::FLOAT:
			return (QString::number(_param.mFloat));
		case ParameterWrapper::INT:
			return (QString::number(_param.mInt));
		case ParameterWrapper::BOOLEAN:
			return _param.mBoolean ? "t" : "f";
		case ParameterWrapper::MAT:
			{
				cv::Mat mat = _param.mMat;
				int w = mat.size().width;
				int h = mat.size().height;
				QString matString = "";
				for(int y = 0; y < h; y++)
				{
					matString += "[";
					for(int x = 0; x < w-1; x++)
					{
						matString += QString::number(mat.at<uchar>(y,x)) + ";";
					}
					matString += QString::number(mat.at<uchar>(y,w-1));
					matString += "]";
				}
				return (matString);
			}			
		case ParameterWrapper::SCALAR:
			{
				cv::Scalar sca = _param.mScalar;
				QString scaString = QString::number(sca[0]);
				for(int i = 1; i < sca.channels; i++)
				{					
					scaString += ";";
					scaString += QString::number(sca[i]);					
				}				
				return (scaString);
			}			
		default:
			return "";
		}
	}

	void GUIParameterLayout::ParameterFromString(
		const QString		& _value, 
		ParameterWrapper	* mod_param)
	{
		switch(mod_param->mType)
		{
		case ParameterWrapper::DOUBLE:
			mod_param->mDouble = _value.toDouble();
			break;
		case ParameterWrapper::FLOAT:
			mod_param->mFloat = _value.toFloat();
			break;
		case ParameterWrapper::INT:
			mod_param->mInt = _value.toInt();
			break;
		case ParameterWrapper::BOOLEAN:
			mod_param->mBoolean = (!_value.compare("t")) ? true : false;
			break;
		case ParameterWrapper::MAT:
			{
				QStringList rows = _value.split(']');
				mod_param->mMat.release();
				mod_param->mMat = cv::Mat(
					rows.count()-1,
					(*rows.begin()).split(';').count(),
					CV_8U,
					cv::Scalar(1));
				int y = 0;
				for(QStringList::iterator rowIt = rows.begin();
					rowIt != rows.end();
					rowIt++)
				{
					int x = 0;
					rowIt->remove('[');
					QStringList cols = rowIt->split(';');
					if(cols.size() > 1)
					{
						for(QStringList::iterator colIt = cols.begin();
							colIt != cols.end();
							colIt++)
						{			
							int i = colIt->toInt();
							mod_param->mMat.at<uchar>(y,x) = i;
							x++;
						}
						y++;
					}
				}
			}
			break;
		case ParameterWrapper::SCALAR:
			{
				QStringList channels = _value.split(';');
				int cnt = channels.size();				
				mod_param->mScalar = 
					cnt == 4 ? cv::Scalar(channels[0].toDouble(), channels[1].toDouble(), channels[2].toDouble(), channels[3].toDouble())
					: cnt == 3 ? cv::Scalar(channels[0].toDouble(), channels[1].toDouble(), channels[2].toDouble())
					: cnt == 2 ? cv::Scalar(channels[0].toDouble(), channels[1].toDouble())
					: cv::Scalar(channels[0].toDouble());
			}
			break;
		default:
			break;
		}
	}

	void GUIParameterLayout::updateParams()
	{
		//Retrieve values from gui
		for(std::map< ParameterWrapper*, QWidget* >::iterator it = this->mParameters.begin();
			it != this->mParameters.end();
			it++)
		{
			switch(it->first->mType)
			{
			case ParameterWrapper::DOUBLE:
				{
					it->first->mDouble = ((QDoubleSpinBox*)it->second)->value();
					break;
				}
			case ParameterWrapper::FLOAT:
				{
					it->first->mFloat = ((QDoubleSpinBox*)it->second)->value();
					break;
				}
			case ParameterWrapper::INT:
				{
					it->first->mInt = ((QSpinBox*)it->second)->value();
					break;
				}
			case ParameterWrapper::BOOLEAN:
				{
					it->first->mBoolean = ((QCheckBox*)it->second)->isChecked();
					break;
				}			
			case ParameterWrapper::SCALAR:
				{
					QLayout *layout = ((QGroupBox*)it->second)->layout();
					for(int i = 0; i < 4; i++)
					{
						it->first->mScalar[i] = ((QDoubleSpinBox*)layout->itemAt(i)->widget())->value();
					}
					break;
				}
			default:
				{
					QString value = ((QLineEdit*)it->second)->text();
					GUIParameterLayout::ParameterFromString(value, it->first);
					break;
				}
			}

			emit accepted();
		}
	}

	void GUIParameterLayout::reject()
	{
		emit rejected();
	}
}
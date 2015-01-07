/**
 *  @file GUIParameterLayout.h
 *
 * @section DESCRIPTION
 * Creates a dynamic for for a set of ParameterWrapper-wrapped parameters.
 */
#pragma once

#include <QDialog>
#include <QFrame>
#include <QString>
#include <QLabel>
#include <QPushButton>
#include <QErrorMessage>
#include <QCheckBox>
#include <QLineEdit>
#include <QGroupBox>
#include <QGridLayout>
#include <QDoubleSpinBox>

#include "ParameterWrapper.h"

namespace Gui
{
	/**
	 * Creates a dynamic for for a set of ParameterWrapper-wrapped parameters.
	 */
	class GUIParameterLayout : public QGridLayout//QDialog
	{
		Q_OBJECT

		public:
			/**
			 * Constructor
			 *
			 * @param _parent		Parent-widget
			 * @param _parameters	Parameters for which a form shall be created.
			 * @param _OkBtn		Add an OK-Button
			 * @param _CancelBtn	Add a Cancel-Button
			 */
			GUIParameterLayout(
				QWidget							*	_parent,
				std::vector<ParameterWrapper>	*	_parameters,
				const bool							_OkBtn = false,
				const bool							_CancelBtn = false);
			~GUIParameterLayout(void);
			
			/**
			 * Updates the values of this form
			 *
			 * @param _algoParams Specified values
			 */
			void setValues(
				const std::vector< ParameterWrapper >	&	_algoParams);

			/**
			 * Creates a string descrbing given wrapped parameter.
			 *
			 * @param _param	Parameter to be described
			 * @return Description of the parameter
			 */
			static QString ParameterToString(
				const ParameterWrapper	& _param);
			/**
			 * Parses a parameter from a string..
			 *
			 * @param _value	String to be parsed
			 * @param mod_param	Specifies the type of the value to be parse, Parsed parameter will be saved here.
			 */
			static void ParameterFromString(
				const QString			& _value, 
				ParameterWrapper		* mod_param);

		private:
			QErrorMessage *errorMessageDialog;		
			/**
			 * Parameters displayed by this form.
			 **/
			std::map< ParameterWrapper*, QWidget* > mParameters;

		public slots:
			/**
			*	Retrieve values from gui (->mParameters)
			*/
			void updateParams();
			void reject();

		signals:
			void rejected();
			void accepted();
	};
}
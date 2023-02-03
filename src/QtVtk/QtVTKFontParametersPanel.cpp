#include "QtVtk/QtVTKFontParametersPanel.h"
#include <QtUtil/QtConfiguration.h>
#include <QtUtil/QtUnicodeHelper.h>

#include <assert.h>

#include <QLayout>
#include <QPushButton> 
#include <QValidator>
#include <QColorDialog>

#include <vtkSystemIncludes.h>


USING_STD;
USING_UTIL;

static const Charset	charset ("àéèùô");
USE_ENCODING_AUTODETECTION

const QString	QtVTKFontParametersPanel::_arialFamily ("Arial");
const QString	QtVTKFontParametersPanel::_courierFamily ("Courrier");
const QString	QtVTKFontParametersPanel::_timesFamily ("Times");


QtVTKFontParametersPanel::QtVTKFontParametersPanel (
			QWidget* parent, const string& appTitle,
			int family, int size, const Color& color,
			THREESTATES bold, THREESTATES italic)
	: QtGroupBox (parent), 
	  _fontLabel (0), _familiesComboBox (0), _sizeTextField (0),
	  _boldCheckBox (0), _italicCheckBox (0), _appTitle (appTitle),
	  _colorModified (false)
{
	QVBoxLayout*	layout	= new QVBoxLayout (this);
	setLayout (layout);
	setSpacing (QtConfiguration::spacing);
	setMargin (QtConfiguration::margin);

	// Le texte en exemple :
	_fontLabel	= new QLabel (QSTR ("Exemple de police de caractères"), this);
	_fontLabel->setMinimumSize (_fontLabel->sizeHint ( ));
	QColor	fgColor (color.getRed ( ), color.getGreen ( ), color.getBlue ( ));
	QPalette	p (_fontLabel->palette ( ));	
	p.setColor (QPalette::WindowText, fgColor);
	p.setColor (QPalette::Text, fgColor);
	_fontLabel->setPalette (p);
	layout->addWidget (_fontLabel);

	// Les paramètres :
	QtGroupBox*		parametersGroup	= new QtGroupBox (this);
	layout->addWidget (parametersGroup);
	QHBoxLayout*	groupLayout	= new QHBoxLayout (parametersGroup);
	parametersGroup->setLayout (groupLayout);
	groupLayout->setContentsMargins (0, 0, 0, 0);
//	groupLayout->setSpacing (QtConfiguration::spacing);
//	groupLayout->setMargin (QtConfiguration::margin);
	_familiesComboBox	= new QComboBox (parametersGroup);
	groupLayout->addWidget (_familiesComboBox);
	_familiesComboBox->addItem ("");
	_familiesComboBox->addItem (_arialFamily);
	_familiesComboBox->addItem (_courierFamily);
	_familiesComboBox->addItem (_timesFamily);
	_familiesComboBox->setMinimumSize (_familiesComboBox->sizeHint ( ));
	connect (_familiesComboBox, SIGNAL(activated (int)), this, 
	         SLOT (familyChangedCallback (int)));

	QLabel*	sizeLabel	= new QLabel ("Taille :", parametersGroup);
	sizeLabel->setMinimumSize (sizeLabel->sizeHint ( ));
	groupLayout->addWidget (sizeLabel);
	_sizeTextField	= new QtTextField (parametersGroup);
	_sizeTextField->setVisibleColumns (3);
	_sizeTextField->setMaxLength (2);
	_sizeTextField->setMinimumSize (_sizeTextField->sizeHint ( ));
	_sizeTextField->setText (QString::number (size));
	_sizeTextField->setValidator (new QIntValidator (1, 99, _sizeTextField));
	connect (_sizeTextField, SIGNAL(returnPressed ( )), this,
	         SLOT (fontChangedCallback ( )));
	groupLayout->addWidget (_sizeTextField);

	_boldCheckBox	= new QCheckBox ("Gras", parametersGroup);
	switch (bold)
	{
		case ON		: _boldCheckBox->setChecked (true);		break;
		case OFF	: _boldCheckBox->setChecked (false);	break;
		default		:	// v 3.4.0. Tristate if needed.
					  _boldCheckBox->setTristate ( );
					  _boldCheckBox->setCheckState (Qt::PartiallyChecked);
	}	// switch (bold)
	connect (_boldCheckBox, SIGNAL(stateChanged (int)), this, 
	         SLOT (familyChangedCallback (int)));
	_boldCheckBox->setMinimumSize (_boldCheckBox->sizeHint ( ));
	groupLayout->addWidget (_boldCheckBox);
	_italicCheckBox	= new QCheckBox ("Italiques", parametersGroup);
	switch (italic)
	{
		case ON		: _italicCheckBox->setChecked (true);	break;
		case OFF	: _italicCheckBox->setChecked (false);	break;
		default		:	// v 3.4.0. Tristate if needed.
					  _italicCheckBox->setTristate ( );
					  _italicCheckBox->setCheckState (Qt::PartiallyChecked);
	}	// switch (italic)
	connect (_italicCheckBox, SIGNAL(stateChanged (int)), this, 
	         SLOT (familyChangedCallback (int)));
	_italicCheckBox->setMinimumSize (_italicCheckBox->sizeHint ( ));
	groupLayout->addWidget (_italicCheckBox);
	
	// La couleur :
	QPushButton*	colorButton	= new QPushButton ("Couleur ...", this);
	colorButton->setFixedSize (colorButton->sizeHint ( ));
	connect (colorButton, SIGNAL(clicked ( )), this, 
	         SLOT (setColorCallback ( )));
	colorButton->setAutoDefault (false);
	colorButton->setDefault (false);
	layout->addWidget (colorButton);

	// MAJ du texte en exemple :
	setFontFamily (family);

	adjustSize ( );
}	// QtVTKFontParametersPanel::QtVTKFontParametersPanel


QtVTKFontParametersPanel::QtVTKFontParametersPanel (
							const QtVTKFontParametersPanel&)
	: QtGroupBox (0)
{
	assert (0 && "QtVTKFontParametersPanel copy constructor is forbidden.");
}	// QtVTKFontParametersPanel::QtVTKFontParametersPanel (const QtVTKFontParametersPanel&)


QtVTKFontParametersPanel& QtVTKFontParametersPanel::operator = (
							const QtVTKFontParametersPanel&)
{
	assert (0 && "QtVTKFontParametersPanel assignment operator is forbidden.");
	return *this;
}	// QtVTKFontParametersPanel::operator =


QtVTKFontParametersPanel::~QtVTKFontParametersPanel ( )
{
}	// QtVTKFontParametersPanel::~QtVTKFontParametersPanel


int QtVTKFontParametersPanel::getFontFamily ( ) const
{
	assert ((0 != _familiesComboBox) && "QtVTKFontParametersPanel::getFontFamily : null combo box.");
	QString	family	= _familiesComboBox->currentText ( );
	if (_arialFamily == family)
		return VTK_ARIAL;
	else if (_timesFamily == family)
		return VTK_TIMES;
	else if (_courierFamily == family)
		return VTK_COURIER;

	return -1;
}	// QtVTKFontParametersPanel::getFontFamily


void QtVTKFontParametersPanel::setFontFamily (int family)
{
	assert ((0 != _familiesComboBox) && "QtVTKFontParametersPanel::setFontFamily : null combo box.");
	switch (family)
	{
		case VTK_ARIAL		: 
			_familiesComboBox->setCurrentIndex (
								_familiesComboBox->findText (_arialFamily));
			break;
		case VTK_COURIER	: 
			_familiesComboBox->setCurrentIndex (
								_familiesComboBox->findText (_courierFamily));
			break;
		case VTK_TIMES		: 
			_familiesComboBox->setCurrentIndex (
								_familiesComboBox->findText (_timesFamily));
			break;
		default				:
			_familiesComboBox->setCurrentIndex (
								_familiesComboBox->findText (""));
	}	// switch (family)

	parametersChanged ( );
}	// QtVTKFontParametersPanel::setFontFamily


int QtVTKFontParametersPanel::getFontSize ( ) const
{
	assert ((0 != _sizeTextField) && "QtVTKFontParametersPanel::getFontSize : null text field.");
	bool	ok	= false;
	int	size	= _sizeTextField->text ( ).toInt (&ok);
	if (false == ok)
		return -1;

	return size;
}	// QtVTKFontParametersPanel::getFontSize


void QtVTKFontParametersPanel::setFontSize (int size)
{
	assert ((0 != _sizeTextField) && "QtVTKFontParametersPanel::setFontSize : null text field.");
	_sizeTextField->setText (QString::number (size));

	parametersChanged ( );
}	// QtVTKFontParametersPanel::setFontSize


THREESTATES QtVTKFontParametersPanel::isBold ( ) const
{
	assert ((0 != _boldCheckBox) && "QtVTKFontParametersPanel::isBold : null check box.");
	switch (_boldCheckBox->checkState ( ))
	{
		case Qt::Checked	: return ON;
		case Qt::Unchecked	: return OFF;
		default				: return UNDETERMINED;
	}	// switch (_boldCheckBox->checkState ( ))
}	// QtVTKFontParametersPanel::isBold


void QtVTKFontParametersPanel::setBold (THREESTATES bold)
{
	assert ((0 != _boldCheckBox) && "QtVTKFontParametersPanel:setBold : null check box.");
	if (ON == bold)
		_boldCheckBox->setChecked (true);
	else if (OFF == bold)
		_boldCheckBox->setChecked (false);
	else
	{
		_boldCheckBox->setCheckState (Qt::PartiallyChecked);
	}

	parametersChanged ( );
}	// QtVTKFontParametersPanel::setBold


THREESTATES QtVTKFontParametersPanel::isItalic ( ) const
{
	assert ((0 != _italicCheckBox) && "QtVTKFontParametersPanel::isItalic : null check box.");
	switch (_italicCheckBox->checkState ( ))
	{
		case Qt::Checked	: return ON;
		case Qt::Unchecked	: return OFF;
		default				: return UNDETERMINED;
	}	// switch (_italicCheckBox->checkState ( ))
}	// QtVTKFontParametersPanel::isItalic


void QtVTKFontParametersPanel::setItalic (THREESTATES italic)
{
	assert ((0 != _italicCheckBox) && "QtVTKFontParametersPanel:setItalic : null check box.");
	if (ON == italic)
		_italicCheckBox->setChecked (true);
	else if (OFF == italic)
		_italicCheckBox->setChecked (false);
	else
	{
		_italicCheckBox->setCheckState (Qt::PartiallyChecked);
	}

	parametersChanged ( );
}	// QtVTKFontParametersPanel::setItalic


Color QtVTKFontParametersPanel::getColor ( ) const
{
	assert ((0 != _fontLabel) && "QtVTKFontParametersPanel::getColor : null label.");
	QPalette	p (_fontLabel->palette ( ));	
	QColor		color	= p.color (QPalette::WindowText);
	Color	fontColor (color.red ( ), color.green ( ), color.blue ( ));

	return fontColor;
}	// QtVTKFontParametersPanel::getColor


void QtVTKFontParametersPanel::setColor (const Color& color)
{
	assert ((0 != _fontLabel) && "QtVTKFontParametersPanel::setColor : null label.");
	QColor	fgColor (color.getRed ( ), color.getGreen ( ), color.getRed ( ));
	QPalette	p (_fontLabel->palette ( ));	
	p.setColor (QPalette::WindowText, fgColor);
	p.setColor (QPalette::Text, fgColor);
	_fontLabel->setPalette (p);
	parametersChanged ( );
}	// QtVTKFontParametersPanel::setColor


void QtVTKFontParametersPanel::setColorCallback ( )
{
	Color	currentColor	= getColor ( );
	QColor	defaultColor (
					currentColor.getRed ( ), currentColor.getGreen ( ),
					currentColor.getBlue ( ));
	QColor	color	= QColorDialog::getColor (defaultColor, this,
					"Couleur de la fonte", QColorDialog::DontUseNativeDialog); 
	if (false == color.isValid ( ))
		return;

	if (defaultColor == color)
		return;

	QPalette	p (_fontLabel->palette ( ));	
	p.setColor (QPalette::WindowText, color);
	p.setColor (QPalette::Text, color);
	_fontLabel->setPalette (p);
	_colorModified	= true;
	parametersChanged ( );
}	// QtVTKFontParametersPanel::setColorCallback


void QtVTKFontParametersPanel::familyChangedCallback (int)
{
	parametersChanged ( );
}	// QtVTKFontParametersPanel::familyChangedCallback


void QtVTKFontParametersPanel::fontChangedCallback ( )
{
	parametersChanged ( );
}	// QtVTKFontParametersPanel::fontChangedCallback


void QtVTKFontParametersPanel::parametersChanged ( )
{
	// MAJ du texte en exemple. */
	QFont	font;
	switch (getFontFamily ( ))
	{
		case VTK_ARIAL		: font.setFamily ("Arial");	break;
		case VTK_COURIER	: font.setFamily ("Courier");	break;
		case VTK_TIMES		: font.setFamily ("Times");	break;
		default				: font.setFamily ("Courier");
	}	// switch (getFontFamily ( ))
	font.setPointSize (0 >= getFontSize ( ) ? 12 : getFontSize ( ));
	font.setBold (ON == isBold ( ) ? true : false);
	font.setItalic (ON == isItalic ( ) ? true : false);
	_fontLabel->setFont (font);
/*
	Color	color	= getColor ( );
	QColor	fgColor (color.getRed ( ), color.getGreen ( ), color.getRed ( ));
	_fontLabel->setPaletteForegroundColor (fgColor);*/
}	// QtVTKFontParametersPanel::parametersChanged ( )


#ifndef QT_VTK_GRAPHIC_WIDGET_H
#define QT_VTK_GRAPHIC_WIDGET_H

#ifndef VTK_9
#ifndef VTK_8
#ifdef VTK_WIDGET
#include <QVTKWidget.h>
#define USE_VTK_WIDGET 1 
#elif VTK_WIDGET2
#include <vtkGenericOpenGLRenderWindow.h>
#include <QVTKWidget2.h>
#include <QVTKInteractor.h>
#define USE_VTK_WIDGET 1
#endif	// VTK_WIDGET
#else	// VTK_8
#include <QVTKOpenGLWidget.h>
#include <vtkGenericOpenGLRenderWindow.h>
#define USE_VTK_WIDGET 1
#endif	// VTK_8
#else	// VTK_9
#include <QVTKOpenGLNativeWidget.h>
#include <vtkGenericOpenGLRenderWindow.h>
#define USE_VTK_WIDGET 1
#endif	// VTK_9
#include <QVTKInteractor.h>

#include <vtkInteractorStyle.h>
#include <vtkVersionMacros.h>	// v 8.1.1


/**
 * Surcharge de la classe <I>Qt/VTK</I> (<I>QVTKWidget</I>, <I>QVTKWidget2</I>, <I>QVTKOpenGLWidget</I>, <I>QVTKOpenGLNativeWidget</I> utilisée) afin
 * de fournir un service générique selon les versions de <I>VTK</I> :
 * <OL>
 * <LI>de s'assurer du focus lorsque qu'elle est survolée par le curseur de la
 * souris.
 * <LI>De raffraichir la fenêtre lorsqu'elle est démasquée
 * </OL>
 * @since	7.12.0
 * @warning	<B>ATTENTION :</B>la version reposant sur le <I>QVTKWidget2</I> (obsolète) a une facheuse tendance à planter
 * sur station Ubuntu avec driver Nvidia.
 */
#ifdef VTK_WIDGET
class QtVtkGraphicWidget : public QVTKWidget
#elif VTK_WIDGET2
class QtVtkGraphicWidget : public QVTKWidget2
#elif VTK_9
class QtVtkGraphicWidget : public QVTKOpenGLNativeWidget
#elif VTK_8
class QtVtkGraphicWidget : public QVTKOpenGLWidget
#endif	// VTK_WIDGET
{
	public :

	QtVtkGraphicWidget (QWidget* parent);
	virtual ~QtVtkGraphicWidget ( );
	static QtVtkGraphicWidget* create (QWidget* parent, vtkInteractorStyle* interactorStyle, bool allowStereoMode = true);
	static std::string implementationInfos ( );
	virtual vtkRenderWindow* getRenderWindow ( );
	virtual void setRenderWindow (vtkRenderWindow*);
	virtual QVTKInteractor* getInteractor ( );
	virtual void setInteractor (QVTKInteractor*);
#if (VTK_MAJOR_VERSION > 9) || ((VTK_MAJOR_VERSION == 9) && (VTK_MINOR_VERSION >= 2))
	virtual void SetRenderWindow (vtkRenderWindow*);	// v 8.1.1
	virtual vtkRenderWindow* GetRenderWindow ( );		// v 8.1.1
#endif

	protected :

	virtual void enterEvent (QEvent* event);
	virtual void focusInEvent (QFocusEvent* event);

	
	private :
	
	QtVtkGraphicWidget (const QtVtkGraphicWidget&);
	QtVtkGraphicWidget& operator = (const QtVtkGraphicWidget&);
};	// class QtVtkGraphicWidget


#endif	// QT_VTK_GRAPHIC_WIDGET_H

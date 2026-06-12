#include <TkUtil/Exception.h>

#include <QMainWindow>
#include <QMenu>

#include <QtVtk/QtVtkGraphicWidget.h>
#include <vtkRenderer.h>
#include <vtkActor.h>
#include <vtkPolyData.h>

#include <vector>


class QtHardwareSelectionWindow : public QMainWindow
{
	Q_OBJECT

	public :

	QtHardwareSelectionWindow ( );

	~QtHardwareSelectionWindow ( )
	{ }


	protected :

	void createGui ( );
	void createMenu ( );


	protected slots :

	virtual void quitCallback ( );
	virtual void removeObjectsCallback ( );
	virtual void sphereCallback ( );
	virtual void cylinderCallback ( );


	private :

	QtHardwareSelectionWindow (const QtHardwareSelectionWindow&)
		: QMainWindow ( )
	{ }
	QtHardwareSelectionWindow& operator = (const QtHardwareSelectionWindow&)
	{ return *this; }

	QtVtkGraphicWidget*			_graphicWindow;
	vtkRenderer*				_renderer;
	vtkRenderWindow*			_renderWindow;
	std::vector<vtkActor*>		_actors;
	std::vector<vtkPolyData*>	_surfaces;
};	// class QtHardwareSelectionWindow



#include <TkUtil/Exception.h>

#include <QMainWindow>
#include <QMenu>

#include <QtVtk/QtVtkGraphicWidget.h>
#include <vtkRenderer.h>
#include <vtkActor.h>
#include <vtkPolyData.h>

#include <vector>


class QtViewPointWindow : public QMainWindow
{
	Q_OBJECT

	public :

	QtViewPointWindow ( );

	~QtViewPointWindow ( )
	{ }


	protected :

	void createGui ( );
	void createMenu ( );


	protected slots :


	virtual void printCallback ( );
	virtual void printToFileCallback ( );
	virtual void quitCallback ( );
	virtual void removeObjectsCallback ( );
	virtual void sphereCallback ( );
	virtual void cylinderCallback ( );


	private :

	QtViewPointWindow (const QtViewPointWindow&)
		: QMainWindow ( )
	{ }
	QtViewPointWindow& operator = (const QtViewPointWindow&)
	{ return *this; }

	QtVtkGraphicWidget*			_graphicWindow;
	vtkRenderer*				_renderer;
	vtkRenderWindow*			_renderWindow;
	std::vector<vtkActor*>		_actors;
	std::vector<vtkPolyData*>	_surfaces;
};	// class QtViewPointWindow



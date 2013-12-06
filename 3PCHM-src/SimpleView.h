/*=========================================================================
	Copyright: Dec.5 2013
	BIT OTISLAB
	Medical Group
	Jingfan Fan
=========================================================================*/

#ifndef SimpleView_H
#define SimpleView_H

/* Include VTK header files*/
#include <vtkCleanPolyData.h>
#include <vtkAppendPolyData.h>
#include <vtkAxesActor.h>
#include <vtkPolyDataMapper.h>
#include <vtkDataSetMapper.h>
#include <vtkQtTableView.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkSmartPointer.h>
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkPoints.h>
#include <vtkPolyVertex.h>
#include <vtkPolyData.h>
#include <vtkCellArray.h>
#include <vtkIterativeClosestPointTransform.h>
#include <vtkLandmarkTransform.h>
#include <vtkActor.h>
#include <vtkPlane.h>
#include <vtkPlaneSource.h>
#include <vtkProperty.h>
#include <vtkOBJReader.h>
#include <vtkPLYReader.h>
#include <vtkPLYWriter.h>
#include <vtkImageCanvasSource2D.h>
#include <vtkBMPWriter.h>
#include <vtkMath.h>
#include <vtkMatrix4x4.h>

/* Include QT header files*/
#include <QFileDialog>
#include <QFile>
#include <QTextCodec>
#include <QMessageBox>
#include <QTextEdit>
#include <QTextCursor>
#include <QFileInfo>
#include <QTextDocumentWriter>
#include <QtGui>
#include <QMainWindow>

/* Include CHM header files */
#include "CHM.h"

#define arc(x) (3.141592653*x/180)

/* Include C++ header files*/
#include <math.h>
#include <stdlib.h>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <string>
using namespace std;

#include "ui_SimpleView.h"

// Forward Qt class declarations
class Ui_SimpleView;
class QTextEdit;

// Forward VTK class declarations
class vtkQtTableView;


class SimpleView : public QMainWindow
{
	Q_OBJECT

public:
	SimpleView();
	~SimpleView();

	/* slots */
	public slots:
		virtual void slotOpenSource();
		virtual void slotOpenTarget();
		virtual void slotTransform();
		virtual void slotICP();
		virtual void slotNormalize();
		virtual void slotConvexHull();
		virtual void slotCHM();
		virtual void slotExit();
		virtual void slotRandomT();
		virtual void slotNoising();
		virtual void slotRender();
		
private:
	// Designer form
	Ui_SimpleView *ui;

	/* Variables */
	CHM_Register CHM;

	vtkSmartPointer<vtkQtTableView> TableView;

	vtkSmartPointer<vtkPoints> ProjPoints_Source;
	vtkSmartPointer<vtkPoints> ProjPoints_Target;

	vtkSmartPointer<vtkPolyData> PolyData_Source;
	vtkSmartPointer<vtkPolyData> PolyData_Target;
	vtkSmartPointer<vtkPolyData> SparsePoints_Source;
	vtkSmartPointer<vtkPolyData> SparsePoints_Target;
	
	vtkSmartPointer<vtkPolyDataMapper> PolyDataMapper_Source;
	vtkSmartPointer<vtkPolyDataMapper> PolyDataMapper_Target;
	vtkSmartPointer<vtkPolyDataMapper> SparsePointsMapper_Source;
	vtkSmartPointer<vtkPolyDataMapper> SparsePointsMapper_Target;

	vtkSmartPointer<vtkPolyDataMapper> PolyDataMapper_ConvexHullSource;
	vtkSmartPointer<vtkPolyDataMapper> PolyDataMapper_ConvexHullTarget;

	vtkSmartPointer<vtkActor> Actor_Source;
	vtkSmartPointer<vtkActor> Actor_Target;
	vtkSmartPointer<vtkActor> Actor_SparsePointsSource;
	vtkSmartPointer<vtkActor> Actor_SparsePointsTarget;

	vtkSmartPointer<vtkActor> Actor_ConvexHullSource;
	vtkSmartPointer<vtkActor> Actor_ConvexHullTarget;

	vtkSmartPointer<vtkAxesActor> Actor_axes;

	vtkSmartPointer<vtkRenderer> ren;

	bool Flag_Source, Flag_Target, Flag_ConvexHull, Flag_Proj, Flag_ProjMatch;

	/* Functions */
	//SimpleView *findMainWindow(const QString &fileName);
};

#endif // SimpleView_H

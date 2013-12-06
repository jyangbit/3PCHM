/*=========================================================================
	Copyright: Dec.5 2013
	BIT OTISLAB
	Medical Group
	Jingfan Fan
=========================================================================*/

#include "SimpleView.h"

/* Constructor of SimpleView*/
SimpleView::SimpleView()
{
	this->ui = new Ui_SimpleView;
	this->ui->setupUi(this);

	/* Create VTK objects */ 
	VTK_NEW(vtkQtTableView, this->TableView);
	VTK_NEW(vtkPolyData, this->PolyData_Source);
	VTK_NEW(vtkPolyData, this->PolyData_Target);

	VTK_NEW(vtkPolyData, SparsePoints_Source);
	VTK_NEW(vtkPolyData, SparsePoints_Target);

	VTK_NEW(vtkPolyDataMapper, this->PolyDataMapper_Source);
	VTK_NEW(vtkPolyDataMapper, this->PolyDataMapper_Target);

	VTK_NEW(vtkPolyDataMapper, this->SparsePointsMapper_Source);
	VTK_NEW(vtkPolyDataMapper, this->SparsePointsMapper_Target);

	VTK_NEW(vtkPolyDataMapper, this->PolyDataMapper_ConvexHullSource);
	VTK_NEW(vtkPolyDataMapper, this->PolyDataMapper_ConvexHullTarget);


	VTK_NEW(vtkActor, this->Actor_Source);
	VTK_NEW(vtkActor, this->Actor_Target);

	VTK_NEW(vtkActor, this->Actor_SparsePointsSource);
	VTK_NEW(vtkActor, this->Actor_SparsePointsTarget);

	VTK_NEW(vtkActor, this->Actor_ConvexHullSource);
	VTK_NEW(vtkActor, this->Actor_ConvexHullTarget);

	VTK_NEW(vtkAxesActor, this->Actor_axes);

	VTK_NEW(vtkRenderer, this->ren);

	/* Set up action signals and slots */
	connect(this->ui->Action_OpenSource, SIGNAL(triggered()), this, SLOT(slotOpenSource()));
	connect(this->ui->Action_OpenTarget, SIGNAL(triggered()), this, SLOT(slotOpenTarget()));
	connect(this->ui->Action_Exit, SIGNAL(triggered()), this, SLOT(slotExit()));
	connect(this->ui->PushButton_Transform, SIGNAL(clicked()), this, SLOT(slotTransform()));
	connect(this->ui->PushButton_LMICP, SIGNAL(clicked()), this, SLOT(slotICP())); 
	connect(this->ui->PushButton_ConvexHull, SIGNAL(clicked()), this, SLOT(slotConvexHull())); 
	connect(this->ui->PushButton_3PCHM, SIGNAL(clicked()), this, SLOT(slotCHM())); 
	connect(this->ui->PushButton_Normalization, SIGNAL(clicked()), this, SLOT(slotNormalize())); 
	connect(this->ui->PushButton_RandomT, SIGNAL(clicked()), this, SLOT(slotRandomT())); 
	connect(this->ui->PushButton_Noising, SIGNAL(clicked()), this, SLOT(slotNoising())); 
	connect(this->ui->PushButton_Render, SIGNAL(clicked()), this, SLOT(slotRender())); 

	/* Initialization */
	this->ui->LineEdit_RotationX->setText("0");
	this->ui->LineEdit_RotationY->setText("0");
	this->ui->LineEdit_RotationZ->setText("0");
	this->ui->LineEdit_TranslationX->setText("0");
	this->ui->LineEdit_TranslationY->setText("0");
	this->ui->LineEdit_TranslationZ->setText("0");
	this->ui->LineEdit_Bounding->setText("1.0");
	this->ui->LineEdit_SourceSD->setText("5000");
	this->ui->LineEdit_TargetSD->setText("5000");
	this->ui->LineEdit_Sigma->setText("2.0");
	this->ui->RadioButton_Source->setOn(true);
	this->ui->ViewCheck_Axis->setOn(true);
	this->ui->ViewCheck_ConvexHull->setOn(false);
	this->ui->ViewCheck_PointData->setOn(false);
	this->ui->ViewCheck_PolyData->setOn(true);
	this->ui->ViewCheck_Source->setOn(true);
	this->ui->ViewCheck_Target->setOn(true);
	this->ui->LineEdit_PointDataOpacity->setText("1.0");
	this->ui->LineEdit_PolyDataOpacity->setText("1.0");
	this->ui->LineEdit_ConvexHullOpacity->setText("1.0");
	this->ui->LineEdit_SourceR->setText("0.0");
	this->ui->LineEdit_SourceG->setText("0.0");
	this->ui->LineEdit_SourceB->setText("1.0");
	this->ui->LineEdit_TargetR->setText("1.0");
	this->ui->LineEdit_TargetG->setText("0.0");
	this->ui->LineEdit_TargetB->setText("0.0");

	this->Flag_Source = false;
	this->Flag_Target = false;
	this->Flag_ConvexHull = false;
	this->Flag_Proj = false;
	this->Flag_ProjMatch = false;

	this->slotRender();
	//this->ui->Widget_ProjMatch->deleteLater();
}

SimpleView::~SimpleView()
{
	//The smart pointers should clean up for up
}

void SimpleView::slotRender()
{
	double r, g, b, o;

	r = this->ui->LineEdit_SourceR->text().toDouble();
	g = this->ui->LineEdit_SourceG->text().toDouble();
	b = this->ui->LineEdit_SourceB->text().toDouble();
	o = this->ui->LineEdit_PolyDataOpacity->text().toDouble();
	PolyDataMapper_Source->SetInput(PolyData_Source);
	Actor_Source->SetMapper(PolyDataMapper_Source);
	Actor_Source->GetProperty()->SetColor(r, g, b);
	Actor_Source->GetProperty()->SetPointSize(0);
	Actor_Source->GetProperty()->SetOpacity(o);

	o = this->ui->LineEdit_PointDataOpacity->text().toDouble();
	SparsePointsMapper_Source->SetInput(SparsePoints_Source);
	Actor_SparsePointsSource->SetMapper(SparsePointsMapper_Source);
	Actor_SparsePointsSource->GetProperty()->SetColor(r, g, b);
	Actor_SparsePointsSource->GetProperty()->SetPointSize(2);
	Actor_SparsePointsSource->GetProperty()->SetOpacity(o);

	o = this->ui->LineEdit_ConvexHullOpacity->text().toDouble();
	PolyDataMapper_ConvexHullSource->SetInput(CHM.ConvexHull_Source);
	Actor_ConvexHullSource->SetMapper(PolyDataMapper_ConvexHullSource);
	Actor_ConvexHullSource->GetProperty()->SetColor(r, g, b);
	Actor_ConvexHullSource->GetProperty()->SetOpacity(o);
	Actor_ConvexHullSource->GetProperty()->SetPointSize(0);

	r = this->ui->LineEdit_TargetR->text().toDouble();
	g = this->ui->LineEdit_TargetG->text().toDouble();
	b = this->ui->LineEdit_TargetB->text().toDouble();
	o = this->ui->LineEdit_PolyDataOpacity->text().toDouble();
	PolyDataMapper_Target->SetInput(PolyData_Target);
	Actor_Target->SetMapper(PolyDataMapper_Target);
	Actor_Target->GetProperty()->SetColor(r, g, b);
	Actor_Target->GetProperty()->SetPointSize(0);
	Actor_Target->GetProperty()->SetOpacity(o);

	o = this->ui->LineEdit_PointDataOpacity->text().toDouble();
	SparsePointsMapper_Target->SetInput(SparsePoints_Target);
	Actor_SparsePointsTarget->SetMapper(SparsePointsMapper_Target);
	Actor_SparsePointsTarget->GetProperty()->SetColor(r, g, b);
	Actor_SparsePointsTarget->GetProperty()->SetPointSize(2);
	Actor_SparsePointsTarget->GetProperty()->SetOpacity(o);

	o = this->ui->LineEdit_ConvexHullOpacity->text().toDouble();
	PolyDataMapper_ConvexHullTarget->SetInput(CHM.ConvexHull_Target);
	Actor_ConvexHullTarget->SetMapper(PolyDataMapper_ConvexHullTarget);
	Actor_ConvexHullTarget->GetProperty()->SetColor(r, g, b);
	Actor_ConvexHullTarget->GetProperty()->SetOpacity(o);
	Actor_ConvexHullTarget->GetProperty()->SetPointSize(0);


	this->ren->SetBackground(.3, .4, .5);
	this->ren->RemoveAllViewProps();
	if (this->ui->ViewCheck_Axis->isOn())
		this->ren->AddActor(this->Actor_axes);
	if (this->ui->ViewCheck_Source->isOn())
	{
		if (this->ui->ViewCheck_PolyData->isOn())
			this->ren->AddActor(this->Actor_Source);
		if (this->ui->ViewCheck_PointData->isOn())
			this->ren->AddActor(this->Actor_SparsePointsSource);
		if (this->ui->ViewCheck_ConvexHull->isOn())
			this->ren->AddActor(this->Actor_ConvexHullSource);
	}
	if (this->ui->ViewCheck_Target->isOn())
	{
		if (this->ui->ViewCheck_PolyData->isOn())
			this->ren->AddActor(this->Actor_Target);
		if (this->ui->ViewCheck_PointData->isOn())
			this->ren->AddActor(this->Actor_SparsePointsTarget);
		if (this->ui->ViewCheck_ConvexHull->isOn())
			this->ren->AddActor(this->Actor_ConvexHullTarget);
	}
	this->ui->VTKWidget_Render->GetRenderWindow()->AddRenderer(this->ren);
	this->ui->VTKWidget_Render->GetRenderWindow()->Render();
}

/* Action to Open the Source Data File */
void SimpleView::slotOpenSource()
{
	QString fileName = QFileDialog::getOpenFileName("../Data","PLY FILES(*.ply)",0,"","Open Source File");
	QFile file(fileName);
	if (!file.open(QFile::ReadOnly | QFile::Text)) {
		QMessageBox::warning(this, tr("SDI"),tr("Cannot read file %1:\n%2.").arg(fileName).arg(file.errorString()));
		return;
	}

	QTextStream in(&file);
	QApplication::setOverrideCursor(Qt::WaitCursor);
	QApplication::restoreOverrideCursor();

	static int sequenceNumber = 1;

	QString curFile;
	bool isUntitled = fileName.isEmpty();
	if (isUntitled) {
		curFile = tr("document%1.txt").arg(sequenceNumber++);
	} else {
		curFile = QFileInfo(fileName).canonicalFilePath();
	}

	VTK_CREATE(vtkPLYReader, PLYReader);
	PLYReader->SetFileName(curFile);
	PLYReader->Update();
	PolyData_Source->DeepCopy(PLYReader->GetOutput());

	/* Process on Points Data */
	/*
	VTK_CREATE(vtkCellArray, vertices);
	vtkIdType pid[1];
	int i;
	for (i = 0; i < this->PolyData_Source->GetNumberOfPoints(); i++)
	{
	pid[0] = i;
	vertices->InsertNextCell(1, pid);
	}
	PolyData_Source->SetVerts(vertices);
	//printf("%d\n", this->PolyData_Source->GetNumberOfCells());
	*/
	/* Only Points Data */

	slotRender();

	setWindowModified(false);
	setWindowFilePath(curFile);
	statusBar()->showMessage(tr("File loaded"), 2000);
	this->Flag_Source = true;
	this->Flag_ConvexHull = false;
	this->Flag_Proj = false;
	this->Flag_ProjMatch = false;
}

/* Action to Open the Target Data File */
void SimpleView::slotOpenTarget()
{
	QString fileName = QFileDialog::getOpenFileName("../Data","PLY FILES(*.ply)",0,"","Open Target File");
	QFile file(fileName);
	if (!file.open(QFile::ReadOnly | QFile::Text)) {
		QMessageBox::warning(this, tr("SDI"),tr("Cannot read file %1:\n%2.").arg(fileName).arg(file.errorString()));
		return;
	}

	QTextStream in(&file);
	QApplication::setOverrideCursor(Qt::WaitCursor);
	QApplication::restoreOverrideCursor();

	static int sequenceNumber = 1;

	bool isUntitled = fileName.isEmpty();
	QString curFile;
	if (isUntitled) {
		curFile = tr("document%1.txt").arg(sequenceNumber++);
	} else {
		curFile = QFileInfo(fileName).canonicalFilePath();
	}


	VTK_CREATE(vtkPLYReader, PLYReader);
	PLYReader->SetFileName(curFile);
	PLYReader->Update();
	PolyData_Target->DeepCopy(PLYReader->GetOutput());

	/* Process on Points Data */
	/*
	VTK_CREATE(vtkCellArray, vertices);
	vtkIdType pid[1];
	int i;
	for (i = 0; i < this->PolyData_Target->GetNumberOfPoints(); i++)
	{
	pid[0] = i;
	vertices->InsertNextCell(1, pid);
	}
	PolyData_Target->SetVerts(vertices);
	//printf("%d\n", this->PolyData_Source->GetNumberOfCells());
	*/
	/* Only Points Data */

	slotRender();

	setWindowModified(false);
	setWindowFilePath(curFile);

	statusBar()->showMessage(tr("File loaded"), 2000);
	this->Flag_Target = true;
	this->Flag_ConvexHull = false;
	this->Flag_Proj = false;
	this->Flag_ProjMatch = false;
}


void SimpleView::slotExit() 
{
	qApp->exit();
}


/*
SimpleView *SimpleView::findMainWindow(const QString &fileName)
{
QString canonicalFilePath = QFileInfo(fileName).canonicalFilePath();

foreach (QWidget *widget, qApp->topLevelWidgets()) 
{
SimpleView *mainWin = qobject_cast<SimpleView *>(widget);
if (mainWin && mainWin->curFile == canonicalFilePath)
return mainWin;
}
return 0;
}
*/

/* Function of ICP Registration */
void SimpleView::slotICP()
{
	if (!(Flag_Source && Flag_Target)) 
	{
		QMessageBox::warning(this, tr("WARNING"),tr("Please read file first!."));
		return;
	}


	// Setup ICP transform
	VTK_CREATE(vtkIterativeClosestPointTransform, icp);
	icp->SetSource(PolyData_Source);
	icp->SetTarget(PolyData_Target);
	icp->GetLandmarkTransform()->SetModeToRigidBody();
	icp->SetMaximumNumberOfIterations(30);
	icp->StartByMatchingCentroidsOn();
	icp->Modified();
	icp->Update();

	// Get the resulting transformation matrix (this matrix takes the source points to the target points)
	vtkSmartPointer<vtkMatrix4x4> m = icp->GetMatrix();
	//std::cout << "The resulting matrix is: " << *m << std::endl;

	// Transform the source points by the ICP solution
	VTK_CREATE(vtkTransformPolyDataFilter, icpTransformFilter);

#if VTK_MAJOR_VERSION <= 5
	icpTransformFilter->SetInput(PolyData_Source);
#else
	icpTransformFilter->SetInputData(PolyData_Source);
#endif

	icpTransformFilter->SetTransform(icp);
	icpTransformFilter->Update();

	PolyData_Source->DeepCopy(icpTransformFilter->GetOutput());

	/* Visualization */
	slotRender();

	statusBar()->showMessage("Done", 2000);
}

/* Function of Transforming one Model */
void SimpleView::slotTransform()
{
	double a, b, r, x, y, z;
	double s[3];
	bool choice;

	/* Build the Transformation Objects */
	VTK_CREATE(vtkTransform, Transform_Pre);
	VTK_CREATE(vtkTransformPolyDataFilter, TransformFilter_Pre);
	VTK_CREATE(vtkTransform, Transform_Real);
	VTK_CREATE(vtkTransformPolyDataFilter, TransformFilter_Real);
	VTK_CREATE(vtkTransform, Transform_Drawback);
	VTK_CREATE(vtkTransformPolyDataFilter, TransformFilter_Drawback);

	/* Get the Transformation Parameters */
	a = this->ui->LineEdit_RotationX->text().toDouble();
	b = this->ui->LineEdit_RotationY->text().toDouble();
	r = this->ui->LineEdit_RotationZ->text().toDouble();
	x = this->ui->LineEdit_TranslationX->text().toDouble();
	y = this->ui->LineEdit_TranslationY->text().toDouble();
	z = this->ui->LineEdit_TranslationZ->text().toDouble();

	/* Choose the object */
	choice = this->ui->RadioButton_Target->isOn();
	if (choice)
	{
		TransformFilter_Pre->SetInput(PolyData_Target);
		PolyData_Target->GetCenter(s);
	}
	else
	{
		TransformFilter_Pre->SetInput(PolyData_Source);
		PolyData_Source->GetCenter(s);
	}

	/* Translate the Model center to 0,0,0 */
	Transform_Pre->Translate(-s[0], -s[1], -s[2]);
	Transform_Pre->RotateX(0);
	Transform_Pre->RotateY(0);
	Transform_Pre->RotateZ(0);
	/* Transform the Model with Transform Parameters */
	Transform_Real->RotateX(a);
	Transform_Real->RotateY(b);
	Transform_Real->RotateZ(r);
	Transform_Real->Translate(x, y, z);
	/* Translate the Model center back to the source */
	Transform_Drawback->Translate(s[0], s[1], s[2]);
	Transform_Drawback->RotateX(0);
	Transform_Drawback->RotateY(0);
	Transform_Drawback->RotateZ(0);
	//左右，右为正,上下，上为正，前后，前为正

	TransformFilter_Pre->SetTransform(Transform_Pre);
	TransformFilter_Pre->Update();

	TransformFilter_Real->SetInput(TransformFilter_Pre->GetOutput());
	TransformFilter_Real->SetTransform(Transform_Real);
	TransformFilter_Real->Update();

	TransformFilter_Drawback->SetInput(TransformFilter_Real->GetOutput());
	TransformFilter_Drawback->SetTransform(Transform_Drawback);
	TransformFilter_Drawback->Update();

	if (choice)
		PolyData_Target->DeepCopy(TransformFilter_Drawback->GetOutput());
	else
		PolyData_Source->DeepCopy(TransformFilter_Drawback->GetOutput());

	/* Visualization */
	slotRender();

	/* Write the Result to .PLY File */
	/*
	vtkSmartPointer<vtkPLYWriter> writer = vtkSmartPointer<vtkPLYWriter>::New();
	writer->SetFileName("../Data/123.ply");
	writer->SetInput(PolyData_Source);
	writer->SetFileTypeToASCII();
	writer->Write();
	*/
}

/* Function of Normalizing one Model */
void SimpleView::slotNormalize()
{
	double bounds[6];
	double cent[3];
	double s;
	double maxbound = 0;
	double Bounding = this->ui->LineEdit_Bounding->text().toDouble();

	/* Build the Transformation Objects */
	VTK_CREATE(vtkTransform, Transform);
	VTK_CREATE(vtkTransformPolyDataFilter, TransformFilter);
	bool choice;
	choice = this->ui->RadioButton_Target->isOn();
	if (choice)
	{
		TransformFilter->SetInput(PolyData_Target);
		PolyData_Target->GetBounds(bounds);
		PolyData_Target->GetCenter(cent);
	}
	else
	{
		TransformFilter->SetInput(PolyData_Source);
		PolyData_Source->GetBounds(bounds);
		PolyData_Source->GetCenter(cent);
	}
	for (int i = 0; i < 6; i++)
		maxbound = max(maxbound, fabs(bounds[i] - cent[i/2]));

	s = Bounding / maxbound;

	Transform->Scale(s,s,s);
	Transform->Translate(-cent[0], -cent[1], -cent[2]);
	TransformFilter->SetTransform(Transform);
	TransformFilter->Update();

	if (choice)
		PolyData_Target->DeepCopy(TransformFilter->GetOutput());
	else
		PolyData_Source->DeepCopy(TransformFilter->GetOutput());

	/* Visualization */
	slotRender();
}

/* Function of 3PCHM*/
void SimpleView::slotCHM()
{
	this->slotConvexHull();

	CHM.Register();

	VTK_CREATE(vtkTransform, Transform);
	VTK_CREATE(vtkTransformPolyDataFilter, TransformFilter);
	Transform->SetMatrix(CHM.BestTransform);
	TransformFilter->SetInput(PolyData_Source);
	TransformFilter->SetTransform(Transform);
	TransformFilter->Update();
	PolyData_Source->DeepCopy(TransformFilter->GetOutput());

	TransformFilter->SetInput(SparsePoints_Source);
	TransformFilter->Update();
	SparsePoints_Source->DeepCopy(TransformFilter->GetOutput());

	this->slotConvexHull();

	/* ICP Refine if need */
	//this->slotICP();

	/* Visualization PolyData*/
	slotRender();
}

/* Function of Extraction for the Convex Hull */
void SimpleView::slotConvexHull()
{	
	CHM.SetSource(PolyData_Source, this->ui->LineEdit_SourceSD->text().toInt());
	CHM.SetTarget(PolyData_Target, this->ui->LineEdit_TargetSD->text().toInt());
	CHM.Cal_CH_Source();
	CHM.Cal_CH_Target();

	Flag_ConvexHull = true;

	SparsePoints_Source->SetPoints(CHM.Points_Source);
	SparsePoints_Target->SetPoints(CHM.Points_Target);

	VTK_CREATE(vtkCellArray, vertices_Source);
	VTK_CREATE(vtkCellArray, vertices_Target);
	vtkIdType pid[1];
	for (int i = 0; i < CHM.Points_Source->GetNumberOfPoints(); i++)
	{
		pid[0] = i;
		vertices_Source->InsertNextCell(1, pid);
	}	
	SparsePoints_Source->SetVerts(vertices_Source);
	for (int i = 0; i < SparsePoints_Target->GetNumberOfPoints(); i++)
	{
		pid[0] = i;
		vertices_Target->InsertNextCell(1, pid);
	}	
	SparsePoints_Target->SetVerts(vertices_Target);

	slotRender();
}

/* Random Shuffle one Model */
void SimpleView::slotRandomT()
{
	char s[100];
	double x;
	srand((unsigned)time(NULL));
	x = rand()%180;
	sprintf(s, "%.0lf", x);
	this->ui->LineEdit_RotationX->setText(s);
	x = rand()%180;
	sprintf(s, "%.0lf", x);
	this->ui->LineEdit_RotationY->setText(s);
	x = rand()%180;
	sprintf(s, "%.0lf", x);
	this->ui->LineEdit_RotationZ->setText(s);
	x = rand()%200/100.0 - 1;
	sprintf(s, "%.2lf", x);
	this->ui->LineEdit_TranslationX->setText(s);
	x = rand()%200/100.0 - 1;
	sprintf(s, "%.2lf", x);
	this->ui->LineEdit_TranslationY->setText(s);
	x = rand()%200/100.0 - 1;
	sprintf(s, "%.2lf", x);
	this->ui->LineEdit_TranslationZ->setText(s);
}

/* Gassian Noise Generator */
double NoiseGen(double p) {
	double fx[7] = {0, 2.2, 13.7, 50, 84.2, 97.8, 100};
	double ns[7] = {-3, -2, -1, 0, 1, 2, 3};
	int i;
	for (i = 1; i < 7; i++)
		if (p < fx[i])
			break;
	return (ns[i] - ns[i - 1])*(p - fx[i - 1]) / (fx[i] - fx[i - 1]);
}

/* Noising one Model */
void SimpleView::slotNoising()
{
	double sig = this->ui->LineEdit_Sigma->text().toDouble() / 10;
	bool choice = this->ui->RadioButton_Target->isOn();
	double x[3];

	if (choice)
	{
		for (int i = 0; i < PolyData_Target->GetNumberOfPoints(); i++)
		{
			PolyData_Target->GetPoint(i, x);
			x[0] = x[0] + NoiseGen(rand() % 100) * sig;
			x[1] = x[1] + NoiseGen(rand() % 100) * sig;
			x[2] = x[2] + NoiseGen(rand() % 100) * sig;
			PolyData_Target->GetPoints()->SetPoint(i, x);
		}
		Actor_Target->GetProperty()->SetOpacity(0.3);
	}
	else
	{
		for (int i = 0; i < PolyData_Source->GetNumberOfPoints(); i++)
		{
			PolyData_Source->GetPoint(i, x);
			x[0] = x[0] + NoiseGen(rand() % 100) * sig;
			x[1] = x[1] + NoiseGen(rand() % 100) * sig;
			x[2] = x[2] + NoiseGen(rand() % 100) * sig;
			PolyData_Source->GetPoints()->SetPoint(i, x);
		}
		Actor_Source->GetProperty()->SetOpacity(0.3);
	}

	/* Visualization */
	slotRender();
}


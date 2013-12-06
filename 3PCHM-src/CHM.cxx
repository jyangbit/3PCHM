/*=========================================================================
	Copyright: Dec.5 2013
	BIT OTISLAB
	Medical Group
	Jingfan Fan
=========================================================================*/

#include "CHM.h"

CHM_Register::CHM_Register()
{
	VTK_NEW(vtkPoints, Points_Source);
	VTK_NEW(vtkPoints, Points_Target);
	VTK_NEW(vtkPolyData, Poly_Source);
	VTK_NEW(vtkPolyData, Poly_Target);
	VTK_NEW(vtkPolyData, ConvexHull_Source);
	VTK_NEW(vtkPolyData, ConvexHull_Target);
	VTK_NEW(vtkMatrix4x4, BestTransform);
}

CHM_Register::~CHM_Register()
{
}

/* Set Source Data in this Class */
void CHM_Register::SetSource(vtkSmartPointer<vtkPolyData> Source, int sd)
{
	sd = std::max (sd , 5000);
	int step = Source->GetPoints()->GetNumberOfPoints() / sd + 1;
	for (int i = 0; i * step < Source->GetPoints()->GetNumberOfPoints(); i ++)
		this->Points_Source->InsertPoint(i, Source->GetPoints()->GetPoint(i * step));
	this->Poly_Source->SetPoints(this->Points_Source);
}

/* Set Target Data in this Class */
void CHM_Register::SetTarget(vtkSmartPointer<vtkPolyData> Target, int sd)
{
	sd = std::max (sd , 5000);
	int step = Target->GetPoints()->GetNumberOfPoints() / sd + 1;
	for (int i = 0; i * step < Target->GetPoints()->GetNumberOfPoints(); i ++)
		this->Points_Target->InsertPoint(i, Target->GetPoints()->GetPoint(i * step));
	this->Poly_Target->SetPoints(this->Points_Target);
}

/* Calculate 3D Convex Hull by vtkDelaunay3D */
void CHM_Register::Cal_CH_Source()
{
	VTK_CREATE(vtkDelaunay3D, delaunay);
	delaunay->SetInput(this->Poly_Source);
	delaunay->Update();
	delaunay->GetBoundingTriangulation();
	VTK_CREATE(vtkDataSetSurfaceFilter, surfaceFilter);
	surfaceFilter->SetInputConnection(delaunay->GetOutputPort());
	surfaceFilter->Update(); 
	ConvexHull_Source->DeepCopy(surfaceFilter->GetOutput());
}

/* Calculate 3D Convex Hull by vtkDelaunay3D */
void CHM_Register::Cal_CH_Target()
{
	VTK_CREATE(vtkDelaunay3D, delaunay);
	delaunay->SetInput(this->Poly_Target);
	delaunay->Update();
	delaunay->GetBoundingTriangulation();
	VTK_CREATE(vtkDataSetSurfaceFilter, surfaceFilter);
	surfaceFilter->SetInputConnection(delaunay->GetOutputPort());
	surfaceFilter->Update(); 
	ConvexHull_Target->DeepCopy(surfaceFilter->GetOutput());
}

/* Iterate the optimal transformation */
void CHM_Register::Register()
{
	int i, j, k, l, iterid = 0, si = 10, NumofInlier, MaxInlier = 0;
	double minimalerror = inf, sumerror, closestPointDist, InlierDist, bounds[6], maxbound;

	Poly_Target->GetBounds(bounds);
	for (int i = 0; i < 3; i++)
		maxbound = std::max(maxbound, fabs(bounds[i * 2] - bounds[i * 2 + 1]));

	InlierDist = maxbound / 200;

	VTK_CREATE(vtkPoints, Tri_Source);
	VTK_CREATE(vtkPoints, Tri_Target);
	VTK_CREATE(vtkLandmarkTransform, landmarkTransform);
	VTK_CREATE(vtkTransform, Transform);
	VTK_CREATE(vtkTransformPolyDataFilter, TransformFilter);
	VTK_CREATE(vtkKdTree, kdtree);

	landmarkTransform->SetModeToRigidBody();
	//landmarkTransform->SetModeToSimilarity();

	kdtree->BuildLocatorFromPoints(Poly_Target->GetPoints());
	cout<<"KDTree Build Succes"<<endl;
	for (i = 0; i < ConvexHull_Source->GetNumberOfCells(); i++)
	{		
		Tri_Source->DeepCopy(ConvexHull_Source->GetCell(i)->GetPoints());
		landmarkTransform->SetSourceLandmarks(Tri_Source);

		for (j = 0; j < ConvexHull_Target->GetNumberOfCells(); j++)
		{
			for (k = 0; k < 3; k++)
			{
				Tri_Target->SetNumberOfPoints(3);
				Tri_Target->SetPoint(0, ConvexHull_Target->GetCell(j)->GetPoints()->GetPoint(k%3));
				Tri_Target->SetPoint(1, ConvexHull_Target->GetCell(j)->GetPoints()->GetPoint((k+1)%3));
				Tri_Target->SetPoint(2, ConvexHull_Target->GetCell(j)->GetPoints()->GetPoint((k+2)%3));
				if (!isSimilar(Tri_Source, Tri_Target))
					continue;
				landmarkTransform->SetTargetLandmarks(Tri_Target);
				landmarkTransform->Update();
				
				/* Translate the Source Model */
				Transform->SetMatrix(landmarkTransform->GetMatrix());
				TransformFilter->SetTransform(Transform);
				TransformFilter->SetInput(Poly_Source);
				TransformFilter->Update();

				sumerror = 0;
				NumofInlier = 0;
				for (l = 0; l < TransformFilter->GetOutput()->GetNumberOfPoints(); l += si)
				{
					kdtree->FindClosestPoint(TransformFilter->GetOutput()->GetPoint(l), closestPointDist);
					sumerror += closestPointDist;
					if (closestPointDist < InlierDist)
						NumofInlier ++;
				}


				/* Closest Point Measure */
				/*
				if (sumerror < minimalerror)
				{
					minimalerror = sumerror;
					BestTransform->DeepCopy(landmarkTransform->GetMatrix());
					cout<<"The "<< ++iterid <<"th iter error is: "<< minimalerror / 500 <<endl;
					cout<<*BestTransform;
				}
				*/

				if (NumofInlier > MaxInlier)
				{
					MaxInlier = NumofInlier;
					BestTransform->DeepCopy(landmarkTransform->GetMatrix());
					cout<<"The "<< ++iterid <<"th iter error is: "<< sumerror / 500 <<endl;
					cout<<"The Max count of inliers is: "<< MaxInlier << endl;
					cout<<*BestTransform;
				}
			}
		}
	}
}

double CHM_Register::distance(double *pt1, double *pt2)
{
	return sqrt((pt1[0]-pt2[0]) * (pt1[0]-pt2[0]) + (pt1[1]-pt2[1]) * (pt1[1]-pt2[1]) + (pt1[2]-pt2[2]) * (pt1[2]-pt2[2]));
}

/*Judge the congruence of two triangles */
bool CHM_Register::isSimilar(vtkPoints *a, vtkPoints *b)
{
	double alens[3], blens[3];
	double x[3], y[3];

	for (int i = 0; i < 3; i++)
	{
		/* Why alens[i] = distance(a->GetPoint(i%3), a->GetPoint((i+1)%3); is error??? */
		a->GetPoint(i%3, x);
		a->GetPoint((i+1)%3, y);
		alens[i] = distance(x, y);

		if (alens[i] < minieps)
			return false;

		b->GetPoint(i%3, x);
		b->GetPoint((i+1)%3, y);
		blens[i] = distance(x, y);
		
		if (blens[i] < minieps)
			return false;
		
		if (alens[i] / blens[i] > 1.1 || blens[i] / alens[i] > 1.1)
			return false;
	}
	return true;
}


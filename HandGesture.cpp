#include "HandGesture.hpp"

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/video/background_segm.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>

using namespace cv;
using namespace std;

HandGesture::HandGesture() {
	
}


double HandGesture::getAngle(Point s, Point e, Point f) {
	
	double v1[2],v2[2];
	v1[0] = s.x - f.x;
	v1[1] = s.y - f.y;

	v2[0] = e.x - f.x;
	v2[1] = e.y - f.y;

	double ang1 = atan2(v1[1],v1[0]);
	double ang2 = atan2(v2[1],v2[0]);

	double angle = ang1 - ang2;
	if (angle > CV_PI) angle -= 2 * CV_PI;
	if (angle < -CV_PI) angle += 2 * CV_PI;
	return (angle * 180.0/CV_PI);
}
void HandGesture::FeaturesDetection(Mat mask, Mat output_img) {
	
	vector<vector<Point> > contours;
	Mat temp_mask;
	mask.copyTo(temp_mask);
	int index = -1;

        // CODIGO 3.1
        // detección del contorno de la mano y selección del contorno más largo
        //...

		//este bucle busca el contorno más largo

		if (!mask.empty()){
		findContours(mask, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

		index=0;
		for(int i = 0; i<contours.size(); i++){
			
			if(contours.at(i).size() > contours.at(index).size())
				index = i;
		}

        // pintar el contorno más largo
		drawContours(output_img,contours, index, cv::Scalar(255, 0, 0), 2, 8, vector<Vec4i>(),0, Point());
		

	//obtener el convex hull	
	vector<int> hull;
	convexHull(contours[index],hull);
	
	// pintar el convex hull
	Point pt0 = contours[index][hull[hull.size()-1]];
	for (int i = 0; i < hull.size(); i++)
	{
		Point pt = contours[index][hull[i]];
		line(output_img, pt0, pt, Scalar(0, 0, 255), 2, CV_AA);
		pt0 = pt;
	}
	
    //obtener los defectos de convexidad
	vector<Vec4i> defects;
	convexityDefects(contours[index], hull, defects);
		
		
		int cont = 0;
		for (int i = 0; i < defects.size(); i++) {
			
			Point s = contours[index][defects[i][0]];
			Point e = contours[index][defects[i][1]];
			Point f = contours[index][defects[i][2]];
			float depth = (float)defects[i][3] / 256.0;
			double angle = getAngle(s, e, f);
		
            // CODIGO 3.2
            // filtrar y mostrar los defectos de convexidad

			//TIPS: alta profundidad
			// ángulo máximo 
			// OJO CON LA ESCALA (se hace con boundingrect)
			/* 
				el área del rectángulo (o cualquier medida se hace más pequeña, 
				así que profundidad = 0.2 por boundingretc (o sea, el porcentaje del rectángulo)
				En resumen: dejarlo en función del boundingrect 
			*/

			// Rectángulo de contorno, para calcular la profundidad de los dedos
			Rect hand_rect = boundingRect(contours[index]);
			rectangle(output_img, hand_rect, Scalar(255,255,0), 2);

			// Para distinguir los defectos de convexidad que corresponden a los dedos
			if (angle < 90.0 && depth > 0.2 * hand_rect.height) {
				circle(output_img, f, 5, Scalar(0,255,0), 5);
				cont++;
			}

        }
	
	// CONTADOR DEL NUMERO DE DEDOS POR PANTALLA
	Rect hand_rect = boundingRect(contours[index]);
	if (cont == 0 && hand_rect.height < 1.35 * hand_rect.width){ cont = -1;}

	char dedos[3];
	sprintf(dedos, "DEDOS: %d", cont+1);
	putText(output_img, dedos, cvPoint(30,50), CV_FONT_HERSHEY_SIMPLEX, 1.0, Scalar(0,0,0), 2, 5, false);

	// PUNTERO (SI SOLO HAY UN DEDO ABIERTO)
	bool pointer = false;
	if (cont==0){
		Point p = contours[index][defects[0][0]];
		for (int i = 1; i < defects.size(); i++) {	
			Point s = contours[index][defects[i][0]];
			if (s.y < p.y){
				p = s;
			}
		}
		circle(output_img, p, 5, Scalar(0,255,255), 5);
		pointer = true;
	}

	// DETECTOR DE SI LA MANO ESTÁ ABIERTA O CERRADA
	if (cont == -1 && pointer == false){
		putText(output_img, "MANO CERRADA", cvPoint(200,50), CV_FONT_HERSHEY_SIMPLEX, 1.0, Scalar(0,0,255), 2, 5, false);
	}
	if (cont == 4){
		putText(output_img, "MANO ABIERTA", cvPoint(200,50), CV_FONT_HERSHEY_SIMPLEX, 1.0, Scalar(0,255,0), 2, 5, false);
	}

	}
		
}

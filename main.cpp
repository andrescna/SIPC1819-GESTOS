#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/video/background_segm.hpp>

#include <stdio.h>
#include <string>
#include <iostream>

#include "MyBGSubtractorColor.hpp"
#include "HandGesture.hpp"

using namespace std;
using namespace cv;



int main(int argc, char** argv)
{

	Mat frame, bgmask, out_frame;

	//Abrimos la webcam

	VideoCapture cap;
	cap.open(0);
	if (!cap.isOpened())
	{
		printf("\nNo se puede abrir la cámara\n");
		return -1;
	}
        int cont = 0;
        while (frame.empty()&& cont < 2000 ) {

                cap >> frame;
                ++cont;
        }
        if (cont >= 2000) {
                printf("No se ha podido leer un frame válido\n");
                exit(-1);
        }

	// Creamos las ventanas que vamos a usar en la aplicación

	namedWindow("Reconocimiento");
	namedWindow("Fondo");

        // creamos el objeto para la substracción de fondo

	MyBGSubtractorColor bgsubtractor(cap);

	// creamos el objeto para el reconocimiento de gestos

	bgsubtractor.LearnModel();

	// iniciamos el proceso de obtención del modelo del fondo

	for (;;)
	{
		cap >> frame;
		flip(frame, frame, 1);
		if (frame.empty())
		{
			printf("Leído frame vacío\n");
			continue;
		}
		int c = cvWaitKey(40);
		if ((char)c == 'q') break;

		// obtenemos la máscara del fondo con el frame actual
		bgsubtractor.ObtainBGMask(frame, bgmask);

                // CODIGO 2.1
		
		// limpiar la máscara del fondo de ruido
		medianBlur(bgmask, bgmask, 5);

		int dilation_size = 5;
		Mat element = (getStructuringElement(MORPH_RECT, Size(2 * dilation_size +1, 2* dilation_size +1), Point(dilation_size,dilation_size)));

		dilate(bgmask, bgmask, element);
		erode(bgmask, bgmask, element);

		// deteccion de las características de la mano
		
		if (!bgmask.empty()){					
			HandGesture temp;
			temp.FeaturesDetection(bgmask,frame);
		}
		
                // mostramos el resultado de la sobstracción de fondo
		imshow("Fondo", bgmask);
                // mostramos el resultado del reconocimento de gestos
		imshow("Reconocimiento", frame);

	}

	destroyWindow("Reconocimiento");
	destroyWindow("Fondo");
	cap.release();
	return 0;
}

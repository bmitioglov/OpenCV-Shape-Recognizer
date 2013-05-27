

#include <cv.h>
#include <highgui.h>
#include <stdlib.h>
#include <stdio.h>
//#include "opencv2/highgui/highgui.hpp"
//#include "opencv2/imgproc/imgproc.hpp"

using namespace cv;
using namespace std;

// сравнение объектов по моментам их контуров 
//templ - образец, original - набор фигур
void testMatch(IplImage* original, IplImage* templCircle, IplImage* templRectangle) 
{
        assert(original!=0);
        assert(templ!=0);

		CvScalar target_color[4] = 
		{ // in BGR order
			{{   0,   0, 255,   0 }},  // red
			{{   0, 255,   0,   0 }},  // green
			{{ 255,   0,   0,   0 }},  // blue
			{{   0, 255, 255,   0 }}   // yellow
		};

        printf("[i] test cvMatchShapes()\n");

        IplImage* src=0;
		//IplImage* dst=0;

        src=cvCloneImage(original);
		
		
        IplImage* binI = cvCreateImage( cvGetSize(original), 8, 1);
        IplImage* binCircle = cvCreateImage( cvGetSize(templCircle), 8, 1);
		IplImage* binRectangle = cvCreateImage( cvGetSize(templRectangle), 8, 1);		

        // заведём цветные картинки
        IplImage* rgb = cvCreateImage(cvGetSize(original), 8, 3);
        cvConvertImage(src, rgb, CV_GRAY2BGR);
        IplImage* rgbCircle = cvCreateImage(cvGetSize(templCircle), 8, 3);
        cvConvertImage(templCircle, rgbCircle, CV_GRAY2BGR);
		IplImage* rgbRectangle = cvCreateImage(cvGetSize(templRectangle), 8, 3);
        cvConvertImage(templRectangle, rgbRectangle, CV_GRAY2BGR);

        // получаем границы изображения и шаблона
        cvCanny(src, binI, 50, 200);
        cvCanny(templCircle, binCircle, 50, 200);
		cvCanny(templRectangle, binRectangle, 20, 200);

	
		IplImage* binI2 = cvCreateImage( cvGetSize(original), 8, 1);
        //IplImage* binCircle2 = cvCreateImage( cvGetSize(templCircle), 8, 1);

		cvDilate(binI, binI2); 
		cvErode(binI2, binI);
        // показываем
        cvNamedWindow( "cannyI", 1 );
        cvShowImage( "cannyI", binI);

        cvNamedWindow( "cannyCircle", 1 );
		cvShowImage( "cannyCircle", binCircle);

		cvNamedWindow("cannyRectangle", 1);
		cvShowImage("cannyRectangle", binRectangle);

        // для хранения контуров
        CvMemStorage* storage = cvCreateMemStorage(0);
        CvSeq* contoursI=0, *contoursCircle=0, *contoursRectangle=0;

        // находим контуры исходного изображения
        int contoursICount = cvFindContours( binI, storage, &contoursI, sizeof(CvContour), CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, cvPoint(0,0));

        // для отметки контуров
        CvFont font;
        cvInitFont(&font, CV_FONT_HERSHEY_PLAIN, 1.0, 1.0);
        char buf[1024];
        int counter=0;
        cvConvertImage(src, rgb, CV_GRAY2BGR);
       

		// находим самый длинный контур Круга
        cvFindContours( binCircle, storage, &contoursCircle, sizeof(CvContour), CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE , cvPoint(0,0));
        CvSeq* seqCircle=0;
        double perimCircle = 0;

		        if(contoursCircle!=0){
                for(CvSeq* seq0 = contoursCircle;seq0!=0;seq0 = seq0->h_next){
                        double perim = cvContourPerimeter(seq0);
                        if(perim>perimCircle){
                                perimCircle = perim;
                                seqCircle = seq0;
                        }
                        cvDrawContours(rgbCircle, seqCircle, CV_RGB(255,216,0), CV_RGB(0,0,250), 0, 1, 8); 
                }
        }

		// находим самый длинный контур Прямоугольника

		cvFindContours( binRectangle, storage, &contoursRectangle, sizeof(CvContour), CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE , cvPoint(0,0));
        CvSeq* seqRectangle=0;
        double perimRectangle = 0;

		if(contoursRectangle!=0){
			for(CvSeq* seq0 = contoursRectangle;seq0!=0;seq0 = seq0->h_next)
			{
                double perim = cvContourPerimeter(seq0);
                if(perim>perimRectangle){
                        perimRectangle = perim;
                        seqRectangle = seq0;
                }
                cvDrawContours(rgbRectangle, seqRectangle, CV_RGB(255,216,0), CV_RGB(0,0,250), 0, 1, 8); 
			}
		}

		
				
				
		//1) Ищем для Круга--------------------------------------------------
        CvSeq* seqM=0;
        double matchMax=1000;
        // обходим контуры изображения 
        counter=0;
		int i=0; float rad; CvPoint2D32f point; 
        if(contoursI!=0){
                // поиск лучшего совпадения контуров по их моментам 
                for(CvSeq* seq0 = contoursI;seq0!=0;seq0 = seq0->h_next){
                        double match0 = cvMatchShapes(seq0, seqCircle, 3);
                        if(match0<matchMax){
                                matchMax = match0;
                                seqM = seq0;
                        }
						cvMinEnclosingCircle(seq0,&point,&rad); // получим окружность содержащую контур

						int     decimal,   sign;
						char    *buffer;
						int     precision = 4;
						double  source = match0;

						buffer = _ecvt( source, precision, &decimal, &sign ); 
						cvPutText(rgb, buffer, cvPointFrom32f(point), &font, CV_RGB(0,0,250));
						printf( "source: %2.10f   buffer: '%s'  decimal: %d  sign: %d\n",
								source, buffer, decimal, sign );
						 
				}
        }
        // рисуем найденный контур
		IplImage* maxMatchContourImageCircle = cvCreateImage( cvGetSize(original), 8, 3);
		cvSet(maxMatchContourImageCircle, CV_RGB(255,255,255));
		cvDrawContours(maxMatchContourImageCircle, seqM, CV_RGB(250,0,0), CV_RGB(250,0,0), 0, 2, 8); // рисуем контур
		double x=0,y=0;
		


		CvPoint* pt; int xmin = 100000; int xmax=-100000; int ymin = 100000; int ymax = -100000;
		std::cout<<seqM->total<<std::endl;
		for(int i=0; i<seqM->total;i++){
			CvPoint* point = reinterpret_cast<CvPoint*>(cvGetSeqElem(seqM, i));
			if (point->x > xmax) xmax = point->x;
			if (point->x < xmin) xmin = point->x;
			if (point->y > ymax) ymax = point->y;
			if (point->y < ymin) ymin = point->y;
			
			printf("x = %d y = %d\n",point->x, point->y);
		}

		printf("radius = %d centerX = %d centerY = %d", (xmax - xmin)/2, xmin+(xmax-xmin)/2, ymin+(ymax-ymin)/2);

		int radius = 15;
		cvCircle(maxMatchContourImageCircle,
				cvPoint((int)(xmin+(xmax-xmin)/2),(int)(ymin+(ymax-ymin)/2)),
				(xmax - xmin)/2,
				target_color[1],2);



        cvNamedWindow( "maxMatchContourCircle", 1 );
		cvShowImage( "maxMatchContourCircle", maxMatchContourImageCircle);
		cvNamedWindow( "original with matches circle", 1 );
		cvShowImage( "original with matches circle", rgb);



		//2) ищем для Прямоугольника---------------------------------------------
		seqM=0;
        matchMax=1000;
        // обходим контуры изображения 
        counter=0;
		i=0;
		IplImage* matchesRectangle = cvCreateImage( cvGetSize(original), 8, 3);
		cvConvertImage(src, matchesRectangle, CV_GRAY2BGR);
        if(contoursI!=0){
                // поиск лучшего совпадения контуров по их моментам 
                for(CvSeq* seq0 = contoursI;seq0!=0;seq0 = seq0->h_next){
                        double match0 = cvMatchShapes(seq0, seqRectangle, 3);
                        if(match0<matchMax){
                                matchMax = match0;
                                seqM = seq0;
                        }
						cvMinEnclosingCircle(seq0,&point,&rad); // получим окружность содержащую контур

						int     decimal,   sign;
						char    *buffer;
						int     precision = 4;
						double  source = match0;

						

						buffer = _ecvt( source, precision, &decimal, &sign ); 
						cvPutText(matchesRectangle, buffer, cvPointFrom32f(point), &font, CV_RGB(0,0,250));
						printf( "source: %2.10f   buffer: '%s'  decimal: %d  sign: %d\n",
								source, buffer, decimal, sign );
						 
				}
        }
        // рисуем найденный контур
		IplImage* maxMatchContourImageRectangle = cvCreateImage( cvGetSize(original), 8, 3);
		cvSet(maxMatchContourImageRectangle, CV_RGB(255,255,255));
		cvDrawContours(maxMatchContourImageRectangle, seqM, CV_RGB(250,0,0), CV_RGB(250,0,0), 0, 2, 8); // рисуем контур
		x=0,y=0;


		cvNamedWindow( "maxMatchContourRectangle", 1 );
		cvShowImage( "maxMatchContourRectangle", maxMatchContourImageRectangle);
		cvNamedWindow( "original with matches rectangle", 1 );
		cvShowImage( "original with matches rectangle", matchesRectangle);

		//найдём углы детектором Харриса
		/// Параметры детектора:
		int blockSize = 2;
		int apertureSize = 3;
		double k = 0.04;

		IplImage* binMaxMatchContourImage = cvCreateImage( cvGetSize(maxMatchContourImageRectangle), 8, 1);
		IplImage* afterHarrisImage = cvCreateImage( cvGetSize(maxMatchContourImageRectangle), 8, 1);
		
		cvCanny(maxMatchContourImageRectangle, binMaxMatchContourImage, 50, 200);
		const int MAX_CORNERS = 15;
		CvPoint2D32f corners[MAX_CORNERS] = {0}; 
		CvArr* temp_image; 
		int maxCornersToFind = 15;
		cvGoodFeaturesToTrack(binMaxMatchContourImage, afterHarrisImage, temp_image, corners, &maxCornersToFind, 0.01, 1);
		
		//нарисуем круги вокруг углов
		for( int i = 0; i < MAX_CORNERS; i++) {
			int radius = 15;
			cvCircle(maxMatchContourImageRectangle,
					cvPoint((int)(corners[i].x + 0.5f),(int)(corners[i].y + 0.5f)),
					radius,
					target_color[1],1);
		}

		cvNamedWindow( "after Harris Image", 1 );
		cvShowImage( "after Harris Image", maxMatchContourImageRectangle);






        // ждём нажатия клавиши
        cvWaitKey(0);

        // освобождаем ресурсы
        cvReleaseMemStorage(&storage);
        cvReleaseImage(&src);
        cvReleaseImage(&rgb);
        cvReleaseImage(&rgbCircle);
        cvReleaseImage(&binI);
        cvReleaseImage(&binCircle);
		cvReleaseImage(&binRectangle);

        // удаляем окна
        cvDestroyAllWindows();
}

int main(int argc, char* argv[])
{
        IplImage *original=0, *templCircle=0, *templRectangle=0;

        // имя картинки задаётся первым параметром
        char* filename = argc >= 2 ? argv[1] : "D:\\work\\DiplomApplication2\\DiplomApplication2\\kontur6.png";
        // получаем картинку
        original = cvLoadImage(filename, 0);

        printf("[i] image: %s\n", filename);
        assert( original != 0 );

        // шаблон Круг
        char* filename2 = argc >= 3 ? argv[2] : "D:\\work\\DiplomApplication2\\DiplomApplication2\\kontur0.png";
        // получаем картинку 
        templCircle = cvLoadImage(filename2, 0);

        printf("[i] template: %s\n", filename2);
        assert( templCircle != 0 );

		//шаблон Прямоугольник
		char* filename3 = argc >= 3 ? argv[2] : "D:\\work\\DiplomApplication2\\DiplomApplication2\\kontur1.png";
        // получаем картинку 
        templRectangle = cvLoadImage(filename3, 0);

        printf("[i] template: %s\n", filename3);
        assert( templRectangle != 0 );


        // сравнение
		testMatch(original, templCircle, templRectangle);

        // освобождаем ресурсы
        cvReleaseImage(&original);
        cvReleaseImage(&templCircle);
		 cvReleaseImage(&templRectangle);
        // удаляем окна
        cvDestroyAllWindows();
        return 0;
}
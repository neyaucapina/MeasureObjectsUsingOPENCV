#include <iostream>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <algorithm>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv\cv.h"

using namespace std;
using namespace cv;

//Variables globales
VideoCapture cam(1);
//VideoCapture cam2(2);
Mat vid,vid2,gray,adap,vidc,th,bluri;

//Nombres de las ventanas
const string MainWindow1 = "Imagen de la cam";
const string MainWindow2 = "Imagen de con bordes";
const string MainWindow3 = "Imagen de Threshold";
const string MainWindow4 = "Imagen en escala de Grises";
const string MainWindow5 = "Erode y dilate";
const string MainWindow6 = "Salida de Canny";
const string MainWindow7 = "Salida de Canny Modificada";
const string TrackBar1 = "Imagen de con bordes";

//Variables de TrackBars
int lowThreshold=51;
int max_lowThreshold=255;

//Pixeles por metrica
//float pixelsPerMetric = 24.26;
///IMPORTANT
///In these seccion we define the pixel per metric
float mon_med=28.00;///Main dimension of the first OBJETC, is the diameter of the first circle at the left
float factor=0.0;

//Constates de deteccion de area
const int FRAME_WIDTH = 640;
const int FRAME_HEIGHT = 480;
const int MAX_NUM_OBJECTS = 50;
const int MIN_OBJECT_AREA = 1000;
const int MAX_OBJECT_AREA = 150000;
int numObjects;
int k,l,m,x,y,con=0;

int main(int argc, char *argv[]){
    ///Asigancion de propiedades a las ventanas
    namedWindow(MainWindow1,WINDOW_AUTOSIZE);
    namedWindow(MainWindow2,WINDOW_AUTOSIZE);
    //namedWindow(MainWindow4,WINDOW_AUTOSIZE);
    //namedWindow(MainWindow5,WINDOW_AUTOSIZE);
    namedWindow(MainWindow6,WINDOW_AUTOSIZE);
    //namedWindow(MainWindow7,WINDOW_AUTOSIZE);

    Mat kernel=getStructuringElement(MORPH_ELLIPSE,Size(5,5),Point(-1,-1));
    while(1){
        //Creacion del trackbar
        createTrackbar( "Min Threshold:", TrackBar1, &lowThreshold, max_lowThreshold,0);

        //Carga del video
        cam>>vid;
        //flip(vid,vid,1);//voltear el video
        imshow(MainWindow1,vid);

        //Deteccion de circulos
        cvtColor(vid, gray, CV_BGR2GRAY);
        GaussianBlur(gray, bluri, Size(7, 7),2,0,2);
        threshold(bluri,th, lowThreshold, 255, THRESH_BINARY);
        //gray.copyTo(th);
        imshow(MainWindow2,th);
        //imshow(MainWindow4,gray);

        //Mejorar el circulo
        erode(th,th,kernel,Point(-1,-1),1);
        dilate(th,th,kernel,Point(-1,-1),2);
        //imshow(MainWindow5,th);

        //Deteccion de bordes
        Mat canny_output,canny_cop;
        vector<vector<Point> > contours;
        vector<Vec4i> hierarchy;
        // Detectar los bordes con un umbral min = 100 y max = 200
        Canny(th, canny_output, 150, 255);
        imshow(MainWindow6, canny_output);
        //morphologyEx(edged, MORPH_CLOSE, kernel);
        //Close Kernel
        Mat kernelc=getStructuringElement(MORPH_RECT,Size(3,3),Point(-1,-1));
        //morphologyEx( canny_output, canny_cop, 1, kernelc );

        //imshow("Salida con closed", canny_cop);
        // Buscar los contornos de la imagen, se almacenan en contours
        findContours(canny_output, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
        //imshow(MainWindow7, canny_output);

        //Ordenar contornos
        sort(contours.begin(),
             contours.end(),
             []( const vector<Point>& a, const vector<Point> & b ){
                    Rect ra(boundingRect(a));
                    Rect rb(boundingRect(b));
                    return (ra.x > rb.x);
                }
             );

        //Find the rotated rectangles and ellipses for each contour
        vector<RotatedRect> minRect( contours.size() );
        for( int i = 0; i < contours.size(); i++ )
            minRect[i] = minAreaRect( Mat(contours[i]) );

        ///PROCESO DE IDETENFICACION DE AREAS Y CONTORNOS
        if(hierarchy.size()>0){
            numObjects=hierarchy.size();
            //Si el numero de monedos es menor que 50
            if(numObjects<MAX_NUM_OBJECTS){
                for(k=0;k<numObjects;k++){
                    //Calculo del Area
                    Moments moment = moments((cv::Mat)contours[k]);
                    double area = moment.m00;
                    cout<<"Area: "<<k+1<<" es "<<area<<endl;
                    if (area>MIN_OBJECT_AREA && area<MAX_OBJECT_AREA){
                        //Calculamos el centro de masas
                        x = moment.m10 / area;
                        y = moment.m01 / area;
                        //DIBUJAMOS LOS CONTORNOS
                        Scalar color = CV_RGB(0, 0, 255);
                        //drawContours(vid, contours, (int)k, color, 2, 8, hierarchy, 0, Point());

                        //DIBUJAMOS EL CENTRO
                        circle(vid, Point(x, y), 5, Scalar(255, 255, 255),-1);

                        //DIBUJAMOS LOS RECTANGULOS,LOS PUNTOS EXTREMOS Y PUNTOS MEDIOS
                        Scalar color1 = CV_RGB(0, 255, 0);
                        Point2f rect_points[4];
                        Point2f mid_points[4];
                        minRect[k].points( rect_points );

                        for( int j = 0; j < 4; j++ ){
                            //Dibujamos los rectangulos
                            line( vid, rect_points[j], rect_points[(j+1)%4], color1, 2,1);
                            //Dibujamos los puntos en los extremos
                            circle(vid,rect_points[j],5,Scalar(9,255,255),-1);
                            //Obtenemos los midpoints y los dibujamos
                            mid_points[j]=(rect_points[j]+rect_points[(j+1)%4])*0.5;
                            circle(vid,mid_points[j],5,Scalar(0,0,255),-1);
                        }

                        //Dibujamos las lineas entre cada punto medio
                        line( vid, mid_points[0], mid_points[2], Scalar(255,128,0), 2,1);
                        line( vid, mid_points[1], mid_points[3], Scalar(255,128,0), 2,1);

                        //Factor de Conversion
                        double r;
                        if(k==0){
                            r=sqrt(area/CV_PI);
                            factor=mon_med/r;
                        }
                        //CALCULO DE LAS DIMENSIONES DE ANCHO Y LARGO
                        float pix_ancho=sqrt(pow((mid_points[0].x-mid_points[2].x),2) + pow((mid_points[0].y-mid_points[2].y),2));
                        float pix_alto =sqrt(pow((mid_points[1].x-mid_points[3].x),2) + pow((mid_points[1].y-mid_points[3].y),2));
                        cout<<"Ancho: "<<pix_ancho<<endl;
                        cout<<"Alto: "<< pix_alto <<endl;

                        //Calculo de dimensiones
                        float dim_ancho=pix_ancho*factor;
                        float dim_alto =pix_alto *factor;

                        //Colocar los valores
                        if(k>0){
                            putText(vid,to_string(dim_alto), Point(mid_points[1].x-15,mid_points[1].y-10), FONT_HERSHEY_COMPLEX_SMALL, 0.65, Scalar(255, 0, 0), 0.4, CV_AA);
                            putText(vid,to_string(dim_ancho),Point(mid_points[2].x+10,mid_points[2].y   ), FONT_HERSHEY_COMPLEX_SMALL, 0.65, Scalar(255, 0, 0), 0.4, CV_AA);
                        }else{
                            putText(vid,to_string(mon_med), Point(mid_points[1].x-15,mid_points[1].y-10) , FONT_HERSHEY_COMPLEX_SMALL, 0.65, Scalar(255, 0, 0), 0.4, CV_AA);
                            putText(vid,to_string(mon_med),Point(mid_points[2].x+10,mid_points[2].y   )  , FONT_HERSHEY_COMPLEX_SMALL, 0.65, Scalar(255, 0, 0), 0.4, CV_AA);
                        }
                        //Vision de numeros
                        //stringstream num;
                        //num<<k;
                        putText(vid,"Objeto "+to_string(k+1), Point(x+5,y+5), FONT_HERSHEY_COMPLEX_SMALL,0.5, Scalar(255, 255, 255),1, CV_AA);
                    }
                }

                //Conteo de de piezas
                putText(vid,"There're " + to_string(numObjects)+" Objects", Point(0, 40),FONT_HERSHEY_COMPLEX_SMALL,1, Scalar(255, 0, 0), 0.4, CV_AA);
                imshow(MainWindow1, vid);
            }else{
                putText(vid,"Ha superado el limite de objetos", Point(0, 20),1, 1, Scalar(255, 0, 0), 2);
                imshow(MainWindow1, vid);
            }
        }else{
            putText(vid,"No Piezas", Point(0, 20),1, 1.2, Scalar(255, 0, 0), 2);
            imshow(MainWindow1, vid);
        }
        //Secuencia de Cierre de video
        int c=waitKey(5);
        if((char)c==27)
            break;
    }
    return 0;
}



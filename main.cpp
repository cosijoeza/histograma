#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <math.h>

using namespace cv;
using namespace std;

void on_trackbar(int porcentaje,void*);
Mat histograma(Mat,int,int);

const int slider_max = 255;
int minimo = 0,maximo = 0;
Mat img,imgGray,imgGray_rgb,imgHist,im;

int main(int argc, char** argv)
{    
	if( argc != 2 ){ cout <<"No se encuentra la imagen" << endl; return -1; }
	img = imread(argv[1],CV_LOAD_IMAGE_COLOR);
	if(!img.data)                              // Verifica que sea una entrada correcta
  	{
      cout << "No se puede abrir o encontrar la imagen" << endl ;
      return -1;
  	}
    //Transformar imagen a escala de gris. Leerla en rgb para tener los 3 canales
    cvtColor(img,imgGray,CV_RGB2GRAY);
    cvtColor(img,im,CV_RGB2GRAY);
    imwrite("imGray.jpg",imgGray);
    imgHist = histograma(imgGray,0,0);
    imgGray = imread("imGray.jpg",CV_LOAD_IMAGE_COLOR);

 	namedWindow("imagen",WINDOW_NORMAL);
    resizeWindow("imagen",1020,620);
    createTrackbar("max","imagen", &maximo, slider_max,on_trackbar);
 	createTrackbar("min","imagen", &minimo, slider_max,on_trackbar);
    on_trackbar(maximo,0);
    
    waitKey(0);
    return 0;
}

void on_trackbar(int,void*)
{
    Mat imgGray_rgb = imgGray.clone();
    int x,y;
    int nRen = imgGray_rgb.rows,
        nCol = imgGray_rgb.cols,
        nCanales = imgGray_rgb.channels();
    for(x = 0; x < nRen; x++)
    {
        uchar *renglon = imgGray_rgb.ptr<uchar>(x);
        for(y = 0; y < (nCol * nCanales); y+=nCanales)
        {
            if( *(renglon + y) >= minimo && *(renglon + y) <= maximo )
            {
                *(renglon + y + 0) = 0;
                *(renglon + y + 1) = 255;
                *(renglon + y + 2) = 0;
            }
        }
    }
    imshow("imagen",histograma(im,minimo,maximo));
    namedWindow("foto",WINDOW_NORMAL);
    resizeWindow("foto",800,1200);
    imshow("foto",imgGray_rgb);
}
Mat histograma(Mat img,int min, int max)
{
    int i,j,x,y;
    int range = 256;
    int x1,x2,y1,y2;
    int maxGray = 0,maxBlue = 0, maxGreen = 0, maxRed = 0;

    int nCanales = img.channels();
    int nRen = img.rows;
    int nCol = img.cols;
    
    vector <int> intensity(range * nCanales);
    fill(intensity.begin(),intensity.end(),0);//Rellenar vector con 0
    Mat image(100,range,CV_8UC3,Scalar(0,0,0)); //Color fondo de nueva imagen

   //Conteo intensidades y calculo de intensidad maxima en cada canal
    for(x = 0; x < nRen; x++)
    {
        uchar * renglon = img.ptr<uchar>(x);
        for(y = 0; y < nCol; y+=nCanales)
        {
            if(nCanales == 3)
            {
                intensity[(int(*(renglon + y + 0)) * nCanales) + 0]++; //B 
                intensity[(int(*(renglon + y + 1)) * nCanales) + 1]++; //G
                intensity[(int(*(renglon + y + 2)) * nCanales) + 2]++; //R 
                if(intensity[*(renglon + y + 0)] > maxBlue)
                    { maxBlue = intensity[*(renglon + y + 0)]; } //Maximo nivel de Azul
                if(intensity[*(renglon + y + 1)] > maxGreen)
                    { maxGreen = intensity[*(renglon + y + 1)]; }//Maximo nivel de Verde
                if(intensity[*(renglon + y + 2)] > maxRed)
                    { maxRed = intensity[*(renglon + y + 2)]; }//Maximo nivel de Rojo
            }else{
                intensity[*(renglon + y)]++;    //Grises
                if(intensity[*(renglon + y)] > maxGray)
                    { maxGray = intensity[*(renglon + y)]; }//Maximo Gris
            }
        }
    }
    //Normalizar cantidades
    for(i = 0; i < (range * nCanales); i += nCanales)
    {
        if(nCanales == 3)
        {
            intensity[i+0] = int(float( (intensity[i+0] * 100) / maxBlue));
            intensity[i+1] = int(float( (intensity[i+1] * 100) / maxGreen));
            intensity[i+2] = int(float( (intensity[i+2] * 100) / maxRed));
        }else{
            intensity[i] = int(float( (intensity[i] * 100) / maxGray));  
        }
    }
    //Graficar    
    int thicknessLine = 1;  //Grosor de la linea
    int desplazamiento = 100;   //Invertir el eje Y
    if(nCanales == 3)
    {
        for(i = 0; i < ((range-1) * nCanales); i+=nCanales) //Puntos finales: X(n-1) y Xn
        {
            x1 = i;
            x2 = i+nCanales;

            int x1Blue = x1 + 0, x2Blue = x2 + 0;
            y1 = abs(intensity[x1Blue] - desplazamiento); //Invertir el eje Y
            y2 = abs(intensity[x2Blue] - desplazamiento); //Invertir el eje Y
            line(image,Point(x1Blue,y1),Point(x2Blue,y2),Scalar(255,0,0),thicknessLine);  //Graficar linea

            int x1Green = x1 + 1, x2Green = x2 + 1;
            y1 = abs(intensity[x1Green] - desplazamiento); //Invertir el eje Y
            y2 = abs(intensity[x2Green] - desplazamiento); //Invertir el eje Y
            line(image,Point(x1Green,y1),Point(x2Green,y2),Scalar(0,255,0),thicknessLine);  //Graficar linea

            int x1Red = x1 + 2, x2Red = x2 + 2;
            y1 = abs(intensity[x1Red] - desplazamiento); //Invertir el eje Y
            y2 = abs(intensity[x2Red] - desplazamiento); //Invertir el eje Y
            line(image,Point(x1Red,y1),Point(x2Red,y2),Scalar(0,0,255),thicknessLine);  //Graficar linea  
        }
    }
    else   //Black & White
    {
        for(i = 0; i < range; i+=nCanales) //Necesito las 256 posciciones
        {
            x1 = i;
            x2 = i+nCanales;
            y1 = abs(intensity[x1] - desplazamiento); //Invertir el eje Y
            if(i >= min && i <= max)
                rectangle(image,Point(x1,y1),Point(x2,100),CV_RGB(0,0,255),CV_FILLED);
            else
                rectangle(image,Point(x1,y1),Point(x2,100),CV_RGB(155,155,155),CV_FILLED);
        }
    }
    return image;
}
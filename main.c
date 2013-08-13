#include <stdio.h>
#include <stdlib.h>
#include <opencv/highgui.h>
#include <opencv/cv.h>
#define Mat(img,i,j) (img->imageData +(i)*img->widthStep +(j)* img->nChannels)


int cmpcolor(uchar* pixel,uchar ref1,uchar ref2,uchar ref3,float tolerance){
	return (((abs(*pixel -ref1) < tolerance) && 
			 (abs(*(pixel+1)-ref2)<tolerance) && 
			 (abs(*(pixel+2)-ref3)<tolerance)) ? 1 : 0);
}

void bordures(IplImage * image, int *bordurehaut,int *bordurebas, CvScalar couleur, float tolerance){
	/* La fonction va chercher dans l'image les bordures de la page du livres,
	 * bordurehaut/bordurebas sont des tableau de sizex(image)
	 * bordurehaut[i]/bas[i] qui contient l'ordonné de du debut/fin de page à l'abscisse i 
	 * couleur est la couleur de fond de l'image */
	 
	 int x,h,b;
	 char *ligne;
	 for(x=0;x< image->width;x++){
		 h=0;
		 b=image->height-1;
		 ligne = x*image->nChannels +image->imageData;
		 while (cmpcolor((uchar *)ligne+h*image->widthStep,couleur.val[0],couleur.val[1],couleur.val[2],tolerance) && h<image->height){
			 h++;
		}
		while(cmpcolor((uchar *)ligne+b*image->widthStep,couleur.val[0],couleur.val[1],couleur.val[2],tolerance) && b>0	){
			b--;
		}
		printf("%d|",h);
		*(bordurebas+x)=b;
		*(bordurehaut+x)=h;
	}
}
void dewarp(IplImage * src,IplImage * dst,int *bordurehaut, int *bordurebas,int pos){
	
	IplImage *matx,*maty;
	matx=cvCreateImage(cvSize(src->height,src->width),IPL_DEPTH_32F,1);
	maty=cvCreateImage(cvSize(src->height,src->width),IPL_DEPTH_32F,1);
	dst->origin = src->origin;
	int inf=0;
	int sup=src->width;
		int x,y;
	for(x=0;x<src->width;x++){
			if (bordurehaut[x]<bordurehaut[inf]){
				inf=x;
			}
			if(bordurebas[x]>bordurebas[sup]){
				sup=x;
			}
		}
		int supp,inff;
		supp=bordurebas[sup];
		inff=bordurehaut[inf];

	/*for(x=0;x<src->height;x++){
		for(y=0;y<src->width;y++){
			*(matx->imageData+y*src->widthStep+x)=x;
			*(maty->imageData+y*src->widthStep+x)=(bordurebas[x]-bordurehaut[x])*y/(sup-inf) y;
		}
	}
	cvRemap(src,dst,matx,maty,CV_INTER_LINEAR,cvScalarAll(255));*/
	int i;
	for(x=0;x<src->width;x++){
		for(y=0;y<src->height;y++){
			for(i=0;i<3;i++){
				if (y<pos){
					*(Mat(dst,y,x)+i)=*(i+Mat(src,(int) pos -(pos-bordurehaut[x])*(pos-y)/(pos-inff),x));
				}
				else{
					*(Mat(dst,y,x)+i)=*(i+Mat(src,(int) pos -(y-pos)*(bordurebas[x]-pos)/(pos-supp) ,x));
				}
			}
		}
	}

	
	cvNamedWindow ("essai", CV_WINDOW_NORMAL);
	cvShowImage ("essai", dst);
	cvWaitKey(0);
	
	cvReleaseImage(&matx);
	cvReleaseImage(&maty);
}
	
	 



int main (int argc, char* argv[])
{
	IplImage* img = NULL;
  int error = 0;
 
  /* Vérification des arguments */
  if (argc < 2)
  {
    fprintf (stderr, "usage: %s SRC_IMAGE \n", argv[0]);
    return EXIT_FAILURE;
  }
 
  /* Chargement du fichier image à convertir */
  if (!(img = cvLoadImage (argv[1], CV_LOAD_IMAGE_UNCHANGED)))
  {
    fprintf (stderr, "couldn't open image file: %s\n", argv[1]);
    return EXIT_FAILURE;
  }
 
	cvNamedWindow ("test", CV_WINDOW_NORMAL);
	cvShowImage ("test", img);
	cvWaitKey(0);
	
	IplImage *dst;
	dst=cvCreateImage(cvSize(img->width,img->height),IPL_DEPTH_8U,3);
	
	int *tabx=malloc((img->width)*sizeof(int));
	int *taby = malloc((img->width)*sizeof(int));
	CvScalar pixel;
	pixel.val[0]=0;
	pixel.val[1]=0;
	pixel.val[2]=0;
	bordures(img,tabx,taby,pixel,100);
	dewarp(img,dst,tabx,taby,500);
	
	/*uchar * p;
	int i=0;
	p= (uchar *)img->imageData;
	while(p-(uchar *)img->imageData<img->imageSize-1){
		if (i > img->width-1 ) {
			i=0;
			printf("\n");
		}
		i++;
		printf("%d|", *p);
		p+=4;
	}     
	printf("\n");*/
  /* Libération de la mémoire */
  cvReleaseImage (&img);
  cvReleaseImage(&dst);
	free(tabx);
	free(taby);
  return (error ? EXIT_FAILURE : EXIT_SUCCESS);
	
}

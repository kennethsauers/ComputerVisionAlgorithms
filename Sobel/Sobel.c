#include <stdio.h>
#include <stdlib.h>
#include <math.h>

        int pic[256][256];
        int outpicx[256][256];
        int outpicy[256][256];
        int maskx[3][3] = {{-1,0,1},{-2,0,2},{-1,0,1}};
        int masky[3][3] = {{1,2,1},{0,0,0},{-1,-2,-1}};
        double ival[256][256],maxival = 0;

int main(argc,argv)
int argc;
char **argv;
{
        int i,j,p,q,mr = 1,sum1,sum2;
        double threshold1, threshold2;
        FILE *fo1, *fo2, *fo3, *fp1, *fopen();
        char *foobar;

  argc--; argv++;
  foobar = *argv;
  fp1=fopen(foobar,"rb");

	argc--; argv++;
	foobar = *argv;
	fo1=fopen(foobar,"wb");

  argc--; argv++;
  foobar = *argv;
  fo2=fopen(foobar,"wb");

  argc--; argv++;
  foobar = *argv;
  fo3=fopen(foobar,"wb");

  argc--; argv++;
	foobar = *argv;
	threshold1 = atof(foobar);

  argc--; argv++;
  foobar = *argv;
  threshold2 = atof(foobar);

  fprintf(fo1,"P5\n256 256\n255");
  fprintf(fo2,"P5\n256 256\n255");
  fprintf(fo3,"P5\n256 256\n255");

  // Read Input Image
  for (i=0;i<256;i++)
  {
    for (j=0;j<256;j++)
    {
      pic[i][j]  =  getc (fp1);
      pic[i][j]  &= 0377;
    }
  }
  // Convolve Mask Over image
  for (i=mr;i<256-mr;i++)
  {
    for (j=mr;j<256-mr;j++)
    {
       sum1 = 0;
       sum2 = 0;
       for (p=-mr;p<=mr;p++)
       {
          for (q=-mr;q<=mr;q++)
          {
             sum1 += pic[i+p][j+q] * maskx[p+mr][q+mr];
             sum2 += pic[i+p][j+q] * masky[p+mr][q+mr];
          }
       }
       outpicx[i][j] = sum1;
       outpicy[i][j] = sum2;
    }
  }
  // find magnitude of x & y derivatives
  for (i=mr;i<256-mr;i++)
  {
    for (j=mr;j<256-mr;j++)
    {
       ival[i][j]=sqrt((double)((outpicx[i][j]*outpicx[i][j]) + (outpicy[i][j]*outpicy[i][j])));
       if (ival[i][j] > maxival)
       {
         maxival = ival[i][j];
       }
     }
  }
  // Save Gradient image, Image with Threshold1, Image with threshold2
  for (i=0;i<256;i++)
  {
    for (j=0;j<256;j++)
    {
       ival[i][j] = (ival[i][j] / maxival) * 255;
       fprintf(fo1,"%c",(char)((int)(ival[i][j])));
       fprintf(fo2,"%c",(char)((int)(ival[i][j]) > threshold1 ? (int)(ival[i][j]) : 0));
       fprintf(fo3,"%c",(char)((int)(ival[i][j]) > threshold2 ? (int)(ival[i][j]) : 0));
     }
  }
}

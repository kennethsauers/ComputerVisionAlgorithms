#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define MAXMASK 2

        int pic[256][256];
        double mask[3][3];
        int outpicx[256][256];
        int outpicy[256][256];
        int cand[256][256];
        int final[256][256];
        double outpic1[256][256];
        double maskval = 0;
        int MR = 0;
        int centx = (MAXMASK / 2);
        int centy = (MAXMASK / 2);
        int minThreshold=-1, maxThreshold=-1;
        int slope = -1;
        int maskx[3][3] = {{-1,0,1},{-2,0,2},{-1,0,1}};
        int masky[3][3] = {{1,2,1},{0,0,0},{-1,-2,-1}};
        double percentage;
        double ival[256][256],maxival;

int main(argc,argv)
int argc;
char **argv;
{
        int i,j,p,q,mr,sum1,sum2;
        int topArea = 0, total= 0, cutoff, sum = 0, moreTodo = -1;
        int histogram[256] = {0};
        double sigma;
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
	sigma = atof(foobar);

  argc--; argv++;
	foobar = *argv;
	percentage = atof(foobar);

  fprintf(fo1,"P5\n256 256\n255");
  fprintf(fo2,"P5\n256 256\n255");
  fprintf(fo3,"P5\n256 256\n255");

  // See Sobel.c
  for (i=0;i<256;i++)
  {
    for (j=0;j<256;j++)
    {
      pic[i][j]  =  getc (fp1);
      pic[i][j]  &= 0377;
    }
  }
  // Gaussian Blur
  mr = 1;
  for (p=-mr;p<=mr;p++)
  {
    for (q=-mr;q<=mr;q++)
    {
       maskval = exp(-1*(((p*p)+(q*q))/(2*(sigma*sigma))));
       (mask[p+centy][q+centx]) = maskval;
       sum += maskval;
    }
  }
  for (p=-mr;p<=mr;p++)
  {
    for (q=-mr;q<=mr;q++)
    {
      mask[p][q] /= sum;
    }
  }
  for (i=mr;i<=255-mr;i++)
  {
    for (j=mr;j<=255-mr;j++)
    {
       int sum = 0;
       for (p=-mr;p<=mr;p++)
       {
          for (q=-mr;q<=mr;q++)
          {
             sum += pic[i+p][j+q] * mask[p+centy][q+centx];
          }
       }
       outpic1[i][j] = sum;
    }
  }
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
           sum1 += outpic1[i+p][j+q] * maskx[p+mr][q+mr];
           sum2 += outpic1[i+p][j+q] * masky[p+mr][q+mr];
        }
      }
      outpicx[i][j] = sum1;
      outpicy[i][j] = sum2;
    }
  }
  maxival = 0;
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
  // Gradient Image Save
  for (i=0;i<256;i++)
  {
    for (j=0;j<256;j++)
    {
     ival[i][j] = (ival[i][j] / maxival) * 255;
     fprintf(fo1,"%c",(char)((int)(ival[i][j])));
    }
  }
  // Detect Peaks Baced on position of surounding Pixels
  for(i=MR;i<256-MR;i++)
  {
    for(j=MR;j<256-MR;j++)
    {
      // Remove Divid by zero problem
      if((outpicx[i][j]) == 0.0)
      {
        outpicx[i][j] = .00001;
      }
      slope = (float)outpicy[i][j]/outpicx[i][j];
      if( (slope <= .4142)&&(slope > -.4142))
      {
        if((ival[i][j] > ival[i][j-1])&&(ival[i][j] > ival[i][j+1]))
        {
          cand[i][j] = 255;
        }
      }
      else if( (slope <= 2.4142)&&(slope > .4142))
      {
         if((ival[i][j] > ival[i-1][j-1])&&(ival[i][j] > ival[i+1][j+1]))
         {
           cand[i][j] = 255;
         }
      }
      else if( (slope <= -.4142)&&(slope > -2.4142))
      {
         if((ival[i][j] > ival[i+1][j-1])&&(ival[i][j] > ival[i-1][j+1]))
         {
           cand[i][j] = 255;
         }
      }
      else
      {
        if((ival[i][j] > ival[i-1][j])&&(ival[i][j] > ival[i+1][j]))
        {
          cand[i][j] = 255;
        }
      }
    }
  }
  // Save Peaks Image
  for (i=0;i<256;i++)
  {
    for (j=0;j<256;j++)
    {
      fprintf(fo2,"%c",(char)((int)(cand[i][j])));
    }
  }
  // scaling thresholds
  for (i=0;i<256;i++)
  {
      for (j=0;j<256;j++)
      {
        histogram[(int)ival[i][j]] += 1;
        total++;
      }
  }
  cutoff = percentage * total;
  for(i = 255; i > 0; i--)
  {
    topArea += histogram[i];
    if(topArea > cutoff)
    {
      break;
    }
  }
  maxThreshold = i;
  minThreshold = percentage * maxThreshold;
  printf("Upper Threshold : %d\nLower Threshold : %d\n", maxThreshold, minThreshold);
  // final image
  // Double threshold
  for (i=0;i<256;i++)
  {
    for (j=0;j<256;j++)
    {
        if (cand[i][j] == 255 )
        {
          if (ival[i][j] > maxThreshold)
          {
            cand[i][j] = 0;
            final[i][j] = 255;
          }
          else if (ival[i][j] < minThreshold)
          {
            cand[i][j] = 0;
            final[i][j] = 0;
          }
        }
      }
    }
  // Canny Edge Detection using iteration
  moreTodo = 1;
  while (moreTodo == 1)
  {
    moreTodo = 0;
    for (i=0;i<256;i++)
    {
      for (j=0;j<256;j++)
      {
        if(cand[i][j] == 255)
        {
          for(int p = -1; p !=2; p++)
          {
            for(int q = -1; q != 2; q++)
            {
              if(final[i+p][j+q] == 255)
              {
                cand[i][j] = 0;
                final[i][j] = 255;
                moreTodo = 1;
              }
            }
          }
        }
      }
    }
  }
  // Save Final Image
  for (i=0;i<256;i++)
  {
    for (j=0;j<256;j++)
    {
      fprintf(fo3,"%c",(char)((int)(final[i][j])));
    }
  }
}

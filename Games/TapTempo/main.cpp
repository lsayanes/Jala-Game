

#include <stdio.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <time.h>
#include <string.h>

#define KEY_SPACE   (32)
#define KEY_SCAPE   (27)
#define KEY_RESET   ('r') 

#define MAX_SAMPLES         (60)
#define MAX_PRED_SAMPLES    (10)

float   fSamples[MAX_SAMPLES*2];
int     nSample = 0;
bool    bFilled = false;

bool kbhit()
{
    termios term;
    tcgetattr(0, &term);

    termios term2 = term;
    term2.c_lflag &= ~ICANON;
    tcsetattr(0, TCSANOW, &term2);

    int byteswaiting;
    ioctl(0, FIONREAD, &byteswaiting);

    tcsetattr(0, TCSANOW, &term);

    return byteswaiting > 0;
}


inline float calcTempo()
{
    float fRet = .0;
    for(unsigned char i = 0; i < MAX_SAMPLES; i++)
        fRet+=fSamples[i];

    return (fRet / (float)MAX_SAMPLES) * 60.0;
}
/*
//UNIT TEST :P
inline void fillSamples(float fTap)
{
    for(int  i = 0; i < MAX_SAMPLES; i++)
        fSamples[i] =  fTap;
}

void test()
{    
    fillSamples(2.0);
    printf("120 -> %f\n", calcTempo());
    fillSamples(1.0);
    printf("60 -> %f\n", calcTempo());
    fillSamples(0.5);
    printf("30 -> %f\n", calcTempo());
    fillSamples(3.0);
    printf("180 -> %f\n", calcTempo());   
}
*/  

int main()
{
    int nKey = 0;
    float fTapCnt = 0.0; 
    time_t lastTime = 0;   
    time_t diffTime = 0;

    char strWaiting[MAX_PRED_SAMPLES];
    memset(strWaiting, '-', MAX_PRED_SAMPLES);
    strWaiting[MAX_PRED_SAMPLES] = 0;

   
    while(KEY_SCAPE != nKey)
    {
        if(kbhit())
        {
           nKey = getchar();
           if(KEY_SPACE == nKey)
           {
                fTapCnt+=1.0;
                if(0 == lastTime)
                    lastTime = time(NULL);
           }
           else if(KEY_RESET == nKey)
           {
                lastTime = 0;
                bFilled = false;
                nSample = 0;
           }  
                    
        }

        if(lastTime)
        {
            diffTime = time(NULL) - lastTime;
            if(diffTime > 0)
            {
                fSamples[nSample] = fTapCnt;
                if(!bFilled)
                {
                    if(nSample >= MAX_PRED_SAMPLES)
                    {
                        int i;
                        float fPred = .0;
                        for(i = 0; i < nSample; i++)
                            fPred+=fSamples[i];
                        fPred/=nSample;

                        for(i = nSample; i < MAX_SAMPLES; i++)
                            fSamples[i] =  fPred;

                        bFilled = true;
                    }
                    else
                    {
                        if(nSample < MAX_PRED_SAMPLES)
                            strWaiting[nSample] = '*';
                        
                        printf("%s\r", strWaiting);
                    }
                }
                else
                {
                    printf("bpp :%f\r\n", calcTempo());
                }

                nSample++;
                nSample%=MAX_SAMPLES;
                fTapCnt = 0.0;
                lastTime = time(NULL); 
            }
        }
        else
        {
            printf("listo!\r");
        }

    }

    return 0;
}
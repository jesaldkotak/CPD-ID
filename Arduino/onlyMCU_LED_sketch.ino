 #include <Arduino_RouterBridge.h>

#include <Arduino_LED_Matrix.h>

#include <vector>


Arduino_LED_Matrix matrix;


//void draw( uint8_t* frame) {

  // if (frame.empty()) {

  //   Serial.println("[sketch] draw called with empty frame");

  //   return;

  // }

  // Serial.print("[sketch] draw called, frame.size=");

  // Serial.println((int)frame.size());

  //matrix.draw(frame);

//}

  int N = 500;



void setup() {

  matrix.begin();

  // Serial.begin(115200);

  // configure grayscale bits to 8 so the display can accept 0..255 brightness

  // The MCU expects full-byte brightness values from the backend.

  matrix.setGrayscaleBits(8);

  matrix.clear();


  // int i = 255;

  

  // uint8_t myMat[104] = {

  // i, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

  // i, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

  // i, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

  // i, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

  // i, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

  // i, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

  // i, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

  // i, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

  // };


  // uint8_t myMat[104] = {0};


  // for(int j=0;j<8;j++)

  //   {

  //     for (int k=0;k<13;k++)

  //     myMat[j*13+k] = i; 

  //   } 

  

  // draw(myMat);

  

  //Bridge.begin();


  // Register the draw provider (by-value parameter). Using by-value avoids

  // RPC wrapper template issues with const reference params.

  //Bridge.provide("draw", draw);

   

}


void loop() {

  int i = 255;

  

  uint8_t myMat[104] = {0};

  

  for(int k=0;k<13;k++)

    {

      if(N>1)

      {

        memset(myMat, 0, sizeof(myMat));

        for (int j=0;j<8;j++)

          {          

            myMat[j*13+k] = i;  

          }

        matrix.draw(myMat);

        delay(N);  

        }


      else

      {

        memset(myMat, 0, sizeof(myMat));

        matrix.draw(myMat);

        delay(100);

        memset(myMat, 255, sizeof(myMat));

        matrix.draw(myMat);

        delay(100);

      }

        

    } 


  // for(int j=0;j<8;j++)

  //   {

  //     memset(myMat, 0, sizeof(myMat));

  //     for (int k=0;k<13;k++)

  //       {          

  //         myMat[j*13+k] = i;  

  //       }

  //     matrix.draw(myMat);

  //     delay(N);

  //   } 

  if(N>100)

    N = N - 100;

  else if (N>10)

    N = N - 10;

  else

    N = 1;

  

  //delay(1);

  

} 
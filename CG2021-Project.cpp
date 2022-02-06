#include <exception>
#include <string.h>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <map>
using namespace std;

SDL_Window* window = NULL;
SDL_Surface* screen = NULL;

#define width 512
#define height 340

#define title "CG2021 - Project - Umut Kaan KAVAKLI, Thomas HERCULE, Ozan BALIK"


void setPixel(int x, int y, Uint8 R, Uint8 G, Uint8 B);
SDL_Color getPixel (int x, int y);

void clearScreen(Uint8 R, Uint8 G, Uint8 B);

void gradiantRGB();
void gradiantFull();

void imposedPalette();
void dedicatedPalette();
void grayScale();

void imposedPaletteDithering();
void dedicatedPaletteDithering();
void grayScaleDithering();

void mainMenu();

void writeFile(string fileName);
void readFile(string fileName);

void setPixel(int x, int y, Uint8 R, Uint8 G, Uint8 B) {
  if ((x>=0) && (x<width) && (y>=0) && (y<height))
  {
    /* We convert individual color components to pixel color format */
    Uint32 pixel = SDL_MapRGB(screen->format, R, G, B);

    /* We retrieve information on how many bytes is one pixel  */
    int bpp = screen->format->BytesPerPixel;

    /* We calculate the pixel address  */
    Uint8 *p1 = (Uint8 *)screen->pixels + (y*2) * screen->pitch + (x*2) * bpp;
    Uint8 *p2 = (Uint8 *)screen->pixels + (y*2+1) * screen->pitch + (x*2) * bpp;
    Uint8 *p3 = (Uint8 *)screen->pixels + (y*2) * screen->pitch + (x*2+1) * bpp;
    Uint8 *p4 = (Uint8 *)screen->pixels + (y*2+1) * screen->pitch + (x*2+1) * bpp;

    /* We set the pixel value, depending on the surface format */
    switch(bpp)
    {
        case 1: //8-bit
            *p1 = pixel;
            *p2 = pixel;
            *p3 = pixel;
            *p4 = pixel;
            break;

        case 2: //16-bit
            *(Uint16 *)p1 = pixel;
            *(Uint16 *)p2 = pixel;
            *(Uint16 *)p3 = pixel;
            *(Uint16 *)p4 = pixel;
            break;

        case 3: //24-bit
            if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
                p1[0] = (pixel >> 16) & 0xff;
                p1[1] = (pixel >> 8) & 0xff;
                p1[2] = pixel & 0xff;
                p2[0] = (pixel >> 16) & 0xff;
                p2[1] = (pixel >> 8) & 0xff;
                p2[2] = pixel & 0xff;
                p3[0] = (pixel >> 16) & 0xff;
                p3[1] = (pixel >> 8) & 0xff;
                p3[2] = pixel & 0xff;
                p4[0] = (pixel >> 16) & 0xff;
                p4[1] = (pixel >> 8) & 0xff;
                p4[2] = pixel & 0xff;
            } else {
                p1[0] = pixel & 0xff;
                p1[1] = (pixel >> 8) & 0xff;
                p1[2] = (pixel >> 16) & 0xff;
                p2[0] = pixel & 0xff;
                p2[1] = (pixel >> 8) & 0xff;
                p2[2] = (pixel >> 16) & 0xff;
                p3[0] = pixel & 0xff;
                p3[1] = (pixel >> 8) & 0xff;
                p3[2] = (pixel >> 16) & 0xff;
                p4[0] = pixel & 0xff;
                p4[1] = (pixel >> 8) & 0xff;
                p4[2] = (pixel >> 16) & 0xff;
            }
            break;

        case 4: //32-bit
            *(Uint32 *)p1 = pixel;
            *(Uint32 *)p2 = pixel;
            *(Uint32 *)p3 = pixel;
            *(Uint32 *)p4 = pixel;
            break;

        }
    }
}

void setPixelSurface(int x, int y, Uint8 R, Uint8 G, Uint8 B) {
  if ((x>=0) && (x<width*2) && (y>=0) && (y<height*2))
  {
    /* We convert individual color components to pixel color format */
    Uint32 pixel = SDL_MapRGB(screen->format, R, G, B);

    /* We retrieve information on how many bytes is one pixel */
    int bpp = screen->format->BytesPerPixel;

    /* We calculate the pixel address */
    Uint8 *p = (Uint8 *)screen->pixels + y * screen->pitch + x * bpp;

    /* We set the pixel value, depending on the surface format */
    switch(bpp)
    {
        case 1: //8-bit
            *p = pixel;
            break;

        case 2: //16-bit
            *(Uint16 *)p = pixel;
            break;

        case 3: //24-bit
            if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
                p[0] = (pixel >> 16) & 0xff;
                p[1] = (pixel >> 8) & 0xff;
                p[2] = pixel & 0xff;
            } else {
                p[0] = pixel & 0xff;
                p[1] = (pixel >> 8) & 0xff;
                p[2] = (pixel >> 16) & 0xff;
            }
            break;

        case 4: //32-bit
            *(Uint32 *)p = pixel;
            break;
        }
    }
}

SDL_Color getPixel(int x, int y) {
    SDL_Color color ;
    Uint32 col = 0 ;
    if ((x>=0) && (x<width) && (y>=0) && (y<height)) {
        //we determine the position
        char* pPosition=(char*)screen->pixels ;

        //offset with respect to y
        pPosition+=(screen->pitch*y*2) ;

        //offset with respect to x
        pPosition+=(screen->format->BytesPerPixel*x*2);

        //copy pixel data
        memcpy(&col, pPosition, screen->format->BytesPerPixel);

        //convert the color
        SDL_GetRGB(col, screen->format, &color.r, &color.g, &color.b);

    }
    return ( color ) ;
}

SDL_Color getPixelSurface(int x, int y, SDL_Surface *surface) {
    SDL_Color color ;
    Uint32 col = 0 ;
    if ((x>=0) && (x<width) && (y>=0) && (y<height)) {
        //we determine the position
        char* pPosition=(char*)surface->pixels ;

        //offset with respect to y
        pPosition+=(surface->pitch*y) ;

        //offset with respect to x
        pPosition+=(surface->format->BytesPerPixel*x);

        //copy pixel data
        memcpy(&col, pPosition, surface->format->BytesPerPixel);

        //convert the color
        SDL_GetRGB(col, surface->format, &color.r, &color.g, &color.b);
    }
    return ( color ) ;
}

void ladujBMP(char const* nazwa, int x, int y) {
    SDL_Surface* bmp = SDL_LoadBMP(nazwa);
    if (!bmp)
    {
        printf("Unable to load bitmap: %s\n", SDL_GetError());
    }
    else
    {
        SDL_Color kolor;
        for (int yy=0; yy<bmp->h; yy++) {
			for (int xx=0; xx<bmp->w; xx++) {
				kolor = getPixelSurface(xx, yy, bmp);
				setPixel(xx, yy, kolor.r, kolor.g, kolor.b);
			}
        }
		SDL_FreeSurface(bmp);
        SDL_UpdateWindowSurface(window);
    }

}

void clearScreen(Uint8 R, Uint8 G, Uint8 B) {
    SDL_FillRect(screen, 0, SDL_MapRGB(screen->format, R, G, B));
    SDL_UpdateWindowSurface(window);
}

/// Generated images

void gradiantRGB() {

    for (int y=0; y<height/2; y++) {
        for (int x=0; x<width/2; x++) {
            if(y<height/6) {
                setPixel(x, y, x, 0, 0);
            } else if (y<height/3) {
                setPixel(x, y, 0, x, 0);
            } else {
                setPixel(x, y, 0, 0, x);
            }
        }
    }
    SDL_UpdateWindowSurface(window);
}

void gradiantFull() {
    for(int y=0; y<height/2; y++) {
            for(int x=0; x<width/12+1; x++) {
                    setPixel(x, y, 255, x*6, 0);
                    setPixel(x+width/12, y, 255-x*6, 255, 0);
                    setPixel(x+2*width/12, y, 0, 255, x*6);
                    setPixel(x+3*width/12, y, 0, 255-x*6, 255);
                    setPixel(x+4*width/12, y, x*6, 0, 255);
                    setPixel(x+5*width/12, y, 255, 0, 255-x*6);
            }
    }
    SDL_UpdateWindowSurface(window);
}


/// Color checking
map<int, SDL_Color> fullPalette;
map<int, int> fullPaletteCounter;
SDL_Color palette[8];

int countPixelWithColor(SDL_Color color) {
    int nbPixel = 0;
    SDL_Color pixelColor;
    for (int y=0; y<height/2; y++) {
        for (int x=0; x<width/2; x++) {
            pixelColor = getPixel(x, y);
            if(color.r == pixelColor.r && color.g == pixelColor.g && color.b == pixelColor.b) {
                nbPixel++;
            }
        }
    }
    return (nbPixel);
}

int addColor(SDL_Color color, int sizeFullPalette) {
    fullPalette.insert({sizeFullPalette+1, color});
    return (sizeFullPalette+1);
}

bool compareColors(SDL_Color c1, SDL_Color c2) {
    if (c1.r != c2.r) return false;
    if (c1.g != c2.g) return false;
    if (c1.b != c2.b) return false;
    return true;
}

int checkColorFullPalette(SDL_Color color) {
    int result = -1;

    int sizeFullPalette = fullPalette.size();
    if (sizeFullPalette > 0) {
        for (int c=0; c<sizeFullPalette; c++) {
            if (compareColors(color, fullPalette[c])) {
                result = c;
            }
        }
    }

    if (result<0) {
        result = addColor(color, sizeFullPalette);
    }

    return result;
}

int checkColorPalette(SDL_Color color) {
    int result = -1;

    for (int c=0; c<8; c++) {
        if (compareColors(color, palette[c])) {
            result = c;
        }
    }

    if (result<0) {
        cout<<"New color !\n"<<endl;
    }

    return result;
}

int pixelDistance(SDL_Color color1, SDL_Color color2){
    int R1 = color1.r, G1 = color1.g, B1 = color1.b, A1 = color1.a;
    int R2 = color2.r, G2 = color2.g, B2 = color2.b, A2 = color2.a;

    return abs(R1-R2) + abs(G1-G2) + abs(B1-B2) + abs(A1-A2);
}

int bestColor(SDL_Color color){
    int c;
    int mini = INT_MAX;
    int distance;
    for (int i = 0; i < 8; i++){
        distance = pixelDistance(palette[i], color);
        if (distance < mini){
            mini = distance;
            c = i;
        }
    }
    return c;
}

void findSimilarColor(map<int, SDL_Color> palette, int& c1id, int& c2id) {
   SDL_Color c1;
   SDL_Color c2;
   int newScore;
   int minScore = INT_MAX;

    for (auto const & [c1tid, c1] : palette) {
        for (auto const & [c2tid, c2] : palette) {
            if(c2tid > c1tid) {
                newScore = pixelDistance(c1,c2);
                if(newScore < minScore) {
                    minScore = newScore;
                    c1id = c1tid;
                    c2id = c2tid;
                }
            }
        }
    }
}

/// Palette generating

void setGrayScalePalette() {
    SDL_Color c0 = {0, 0, 0};
    SDL_Color c1 = {36, 36, 36};
    SDL_Color c2 = {72, 72, 72};
    SDL_Color c3 = {108, 108, 108};
    SDL_Color c4 = {144, 144, 144};
    SDL_Color c5 = {180, 180, 180};
    SDL_Color c6 = {216, 216, 216};
    SDL_Color c7 = {255, 255, 255};

    palette[0] = c0;
    palette[1] = c1;
    palette[2] = c2;
    palette[3] = c3;
    palette[4] = c4;
    palette[5] = c5;
    palette[6] = c6;
    palette[7] = c7;
}

void setImposedRgbPalette() {
    SDL_Color c0 = {0, 0, 0};
    SDL_Color c1 = {255, 0, 0};
    SDL_Color c2 = {0, 255, 0};
    SDL_Color c3 = {0, 0, 255};
    SDL_Color c4 = {255, 255, 0};
    SDL_Color c5 = {255, 0, 255};
    SDL_Color c6 = {0, 255, 255};
    SDL_Color c7 = {255, 255, 255};

    palette[0] = c0;
    palette[1] = c1;
    palette[2] = c2;
    palette[3] = c3;
    palette[4] = c4;
    palette[5] = c5;
    palette[6] = c6;
    palette[7] = c7;
}

void computeDedicatedPalette() {
    fullPalette.clear();
    fullPaletteCounter.clear();

    SDL_Color color;
    Uint16 picWidth = width/2;
    Uint16 picHeight = height/2;

    for (int y=0; y<picHeight; y++) {
        for (int x=0; x<picWidth; x++) {
            color = getPixel(x, y);
            checkColorFullPalette(color);
        }
    }
    cout<<"All image colors have been scaned..."<<endl;

    SDL_Color c1, c2, newColor;
    int c1id, c2id;
    int c1Counter, c2Counter;

    while(fullPalette.size() > 8) {
        findSimilarColor(fullPalette, c1id, c2id);

        c1 = fullPalette.at(c1id);
        c2 = fullPalette.at(c2id);

        if(fullPaletteCounter.count(c1id) == 0) {
            fullPaletteCounter.insert({c1id, countPixelWithColor(c1)});
        }
        if(fullPaletteCounter.count(c2id) == 0) {
            fullPaletteCounter.insert({c2id, countPixelWithColor(c2)});
        }
        c1Counter = fullPaletteCounter[c1id];
        c2Counter = fullPaletteCounter[c2id];

        if(c1Counter > c2Counter) {
            newColor = c1;
        } else {
            newColor = c2;
        }

        fullPalette.erase(c1id);
        fullPalette.erase(c2id);

        fullPaletteCounter.erase(c1id);
        fullPaletteCounter.erase(c2id);

        fullPaletteCounter.insert({c1id, c1Counter + c2Counter});
        fullPalette.insert({c1id,newColor});
    }
    int i = 0;
    for (auto const & [cid, c] : fullPalette)
    {
        palette[i] = c;
        i++;
    }
    cout<<"The dedicated palette have been created!"<<endl;

}

/// Image color

void applyDithering() {
    int c;
    SDL_Color color;
    SDL_Color colorPalette;
    int R,G,B;
    int nR,nG,nB;

    float errorsR[(width/2)+2][(height/2)+1];
    memset(errorsR,0,sizeof(errorsR));

    float errorsG[(width/2)+2][(height/2)+1];
    memset(errorsG,0,sizeof(errorsG));

    float errorsB[(width/2)+2][(height/2)+1];
    memset(errorsB,0,sizeof(errorsB));

    int errorR = 0, errorG = 0,errorB = 0;
    int shift = 1;

    for (int x=0; x<width/2; x++) {
        for (int y=0; y<height/2; y++) {
            R = getPixel(x, y).r;
            G = getPixel(x, y).g;
            B = getPixel(x, y).b;

            R += errorsR[x+shift][y];
            G += errorsG[x+shift][y];
            B += errorsB[x+shift][y];

            if(R > 255) {
                R = 255;
            } else if (R < 0){
                R = 0;
            }

            if(G > 255) {
                G = 255;
            } else if (G < 0){
                G = 0;
            }

            if(B > 255) {
                B = 255;
            } else if (B < 0){
                B = 0;
            }

            color.r = R;
            color.g = G;
            color.b = B;

            c = bestColor(color);

            colorPalette = palette[c];

            nR = colorPalette.r;
            nG = colorPalette.g;
            nB = colorPalette.b;

            errorR = R - nR;
            errorG = G - nG;
            errorB = B - nB;

            setPixel(x+width/2, y, nR, nG, nB);

            errorsR[x+shift+1][y  ] += (errorR*7.0/16);
            errorsR[x+shift+1][y+1] += (errorR*1.0/16);
            errorsR[x+shift  ][y+1] += (errorR*5.0/16);
            errorsR[x+shift+1][y+1] += (errorR*3.0/16);

            errorsG[x+shift+1][y  ] += (errorG*7.0/16);
            errorsG[x+shift+1][y+1] += (errorG*1.0/16);
            errorsG[x+shift  ][y+1] += (errorG*5.0/16);
            errorsG[x+shift+1][y+1] += (errorG*3.0/16);

            errorsB[x+shift+1][y  ] += (errorB*7.0/16);
            errorsB[x+shift+1][y+1] += (errorB*1.0/16);
            errorsB[x+shift  ][y+1] += (errorB*5.0/16);
            errorsB[x+shift+1][y+1] += (errorB*3.0/16);
        }
    }

    SDL_UpdateWindowSurface(window);
}

void displayImage(){
    SDL_Color color;
    int c;
    for (int y = 0; y < height/2; y++){
        for (int x = 0; x < width/2; x++){
            color = getPixel(x, y);

            c = bestColor(color);

            setPixel(x+width/2, y, palette[c].r, palette[c].g, palette[c].b);
        }
    }

    SDL_UpdateWindowSurface(window);
}

void imposedPalette() {
    setImposedRgbPalette();
    displayImage();
}

void dedicatedPalette() {
    computeDedicatedPalette();
    displayImage();
}

void grayScale() {
    setGrayScalePalette();
    displayImage();
}

void displayDitheredImage() {
    int c;
    SDL_Color color;
    SDL_Color colorPalette;
    int R,G,B;
    int nR,nG,nB;

    float errorsR[(width/2)+2][(height/2)+1];
    memset(errorsR,0,sizeof(errorsR));

    float errorsG[(width/2)+2][(height/2)+1];
    memset(errorsG,0,sizeof(errorsG));

    float errorsB[(width/2)+2][(height/2)+1];
    memset(errorsB,0,sizeof(errorsB));

    int errorR = 0, errorG = 0,errorB = 0;
    int shift = 1;

    for (int x=0; x<width/2; x++) {
        for (int y=0; y<height/2; y++) {
            R = getPixel(x, y).r;
            G = getPixel(x, y).g;
            B = getPixel(x, y).b;

            R += errorsR[x+shift][y];
            G += errorsG[x+shift][y];
            B += errorsB[x+shift][y];

            if(R > 255) {
                R = 255;
            } else if (R < 0){
                R = 0;
            }

            if(G > 255) {
                G = 255;
            } else if (G < 0){
                G = 0;
            }

            if(B > 255) {
                B = 255;
            } else if (B < 0){
                B = 0;
            }

            color.r = R;
            color.g = G;
            color.b = B;

            c = bestColor(color);

            colorPalette = palette[c];

            nR = colorPalette.r;
            nG = colorPalette.g;
            nB = colorPalette.b;

            errorR = R - nR;
            errorG = G - nG;
            errorB = B - nB;

            setPixel(x+width/2, y, nR, nG, nB);

            errorsR[x+shift+1][y  ] += (errorR*7.0/16);
            errorsR[x+shift+1][y+1] += (errorR*1.0/16);
            errorsR[x+shift  ][y+1] += (errorR*5.0/16);
            errorsR[x+shift+1][y+1] += (errorR*3.0/16);

            errorsG[x+shift+1][y  ] += (errorG*7.0/16);
            errorsG[x+shift+1][y+1] += (errorG*1.0/16);
            errorsG[x+shift  ][y+1] += (errorG*5.0/16);
            errorsG[x+shift+1][y+1] += (errorG*3.0/16);

            errorsB[x+shift+1][y  ] += (errorB*7.0/16);
            errorsB[x+shift+1][y+1] += (errorB*1.0/16);
            errorsB[x+shift  ][y+1] += (errorB*5.0/16);
            errorsB[x+shift+1][y+1] += (errorB*3.0/16);
        }
    }

    SDL_UpdateWindowSurface(window);
}

void imposedPaletteDithering() {
    setImposedRgbPalette();
    displayDitheredImage();
}

void dedicatedPaletteDithering() {
    computeDedicatedPalette();
    displayDitheredImage();
}

void grayScaleDithering() {
    setGrayScalePalette();
    displayDitheredImage();
}

/// Compression

map<int, int> byteRunCompress(map<int, int> uncompressedData) {
    map<int, int> compressedData;
    int length = uncompressedData.size();
    int i = 0;
    int compressedDataLenght = 0;

    while (i < length)
    {
        if ((i < length-1) && (uncompressedData[i] == uncompressedData[i+1])) {
            int j = 0;
            while ((i+j < length-1) && (uncompressedData[i+j] == uncompressedData[i+j+1]) && (j < 127)) {
                j++;
            }
            compressedData.insert({compressedDataLenght++,(j+127)});
            compressedData.insert({compressedDataLenght++,(int)uncompressedData[i+j]});
            i += (j+1);
        }
        else {
            int j=0;
            while ((i+j < length-1) && (uncompressedData[i+j] != uncompressedData[j+i+1]) && (j < 128)) {
                j++;
            }
            if ((i+j == length-1) && (j < 128)) {
                j++;
            }
            compressedData.insert({compressedDataLenght++,(j-1)});
            for (int k=0; k<j; k++) {
                compressedData.insert({compressedDataLenght++,(int)uncompressedData[i+k]});
            }
            i += j;
        }
    }
    return compressedData;
}

map<int, int> byteRunDecompress(map<int, int> compressedData, int compressedDataSize) {
    map<int, int> uncompressedData;
    int uncompressedDataLenght = 0;
    int i = 0;

    while (i < compressedDataSize) {
        if (compressedData[i] > 127) {
            for (int j=0; j<(compressedData[i]-126); j++) {
                uncompressedData.insert({uncompressedDataLenght++,(int)compressedData[i+1]});
            }
            i += 2;
        }
        else {
            for (int j=0; j<(compressedData[i]+1); j++) {
                uncompressedData.insert({uncompressedDataLenght++,(int)compressedData[i+1+j]});
            }
            i += compressedData[i]+2;
        }
    }

    return uncompressedData;
}

map<int, int> rleCompress(map<int, int> uncompressedData) {
    map<int, int> compressedData;
    int length = uncompressedData.size();
    int i = 0;
    int compressedDataLength = 0;

    while (i < length)
    {
        if ((i < length -1) && (uncompressedData[i] == uncompressedData[i+1])){
            int j = 0;
            while ((i+j < length-1) && (uncompressedData[i+j] == uncompressedData[i+j+1]) && (j < 254)){
                j++;
            }
            compressedData.insert({compressedDataLength++, j+1});
            compressedData.insert({compressedDataLength++, (int)uncompressedData[i+j]});
            i += (j+1);
        }
        else {
            int j = 0;
            while ((i+j < length-1) && (uncompressedData[i+j] != uncompressedData[i+j+1]) && (j < 254)) {
                j++;
            }
            if ((i+j == length-1) && (j < 254)){
                j++;
            }
            compressedData.insert({compressedDataLength++, 0});
            compressedData.insert({compressedDataLength++, j});
            for (int k = 0; k<j; k++){
                compressedData.insert({compressedDataLength++, (int)uncompressedData[i+k]});
            }
            if (j%2 != 0)
                compressedData.insert({compressedDataLength++, 0});

            i += j;
        }
    }
    return compressedData;
}

map<int, int> rleDecompress(map<int, int> compressedData, int compressedDataSize) {
    map<int, int> uncompressedData;
    int uncompressedDataLength = 0;
    int i = 0;
    int counter = 0;

    while (i < compressedDataSize) {
        if (compressedData[i] > 0){
            for (int j = 0; j < compressedData[i]; j++){
                uncompressedData.insert({uncompressedDataLength++, (int)compressedData[i+1]});
            }
            i+=2;
        }
        else {
            counter = compressedData[i+1];
            for (int j = 0; j < counter; j++){
                uncompressedData.insert({uncompressedDataLength++, (int)compressedData[i+1+j+1]});
            }
            i += counter+2;
            if (counter%2 != 0){
                i++;
            }

        }
    }

    return uncompressedData;
}

map<int, int> lzwCompress(map<int, int> uncompressedData) {
}

map<int, int> lzwDecompress(map<int, int> compressedData, int compressedDataSize) {
}

/// Write and Read
char identifier[] = "  ";
char compressionType = 'B';

void writeFile(string fileName, char compression[]) {
    SDL_Color color;
    SDL_Color colorPalette;
    Uint8 value = 0;

    Uint16 picWidth = width/2;
    Uint16 picHeight = height/2;

    map<int, int> compressedData;
    map<int, int> uncompressedData;
    int compressedDataSize = 0;

    cout<<"File '" + fileName + "'.bin is being written ... \n"<<endl;
    ofstream output(fileName + ".bin", ios::binary);
    output.write((char*)&identifier, sizeof(char)*2);
    output.write((char*)&picWidth, sizeof(Uint16));
    output.write((char*)&picHeight, sizeof(Uint16));

    if(identifier[0] == 'D') {
        for (int i=0; i<8; i++) {
            colorPalette = palette[i];
            output.write((char*)&colorPalette.r, sizeof(Uint8));
            output.write((char*)&colorPalette.g, sizeof(Uint8));
            output.write((char*)&colorPalette.b, sizeof(Uint8));
        }
    }

    for (int y=0; y<picHeight; y++) {
        for (int x=0; x<picWidth; x++) {
            color = getPixel(x+picWidth, y);
            value = checkColorPalette(color);
            uncompressedData.insert({x + y*picWidth,value});
        }
    }

    if (strcmp(compression, "no compression") == 0) {
        compressionType = 'N';
        output.write((char*)&compressionType, sizeof(char));
        compressedDataSize = uncompressedData.size();
        output.write((char*)&compressedDataSize, sizeof(Uint16));
        for (int y=0; y<picHeight; y++) {
            for (int x=0; x<picWidth; x++) {
                output.write((char*)&uncompressedData[x + y*picWidth], sizeof(Uint8));
            }
        }
    } else {
        if (strcmp(compression, "byteRun") == 0) {
            compressionType = 'B';
            compressedData = byteRunCompress(uncompressedData);
        } else if (strcmp(compression, "rle") == 0) {
            compressionType = 'R';
            compressedData = rleCompress(uncompressedData);
        } else if (strcmp(compression, "lzw") == 0) {
            compressionType = 'L';
            compressedData = lzwCompress(uncompressedData);
        }
        output.write((char*)&compressionType, sizeof(char));

        compressedDataSize = compressedData.size();
        output.write((char*)&compressedDataSize, sizeof(Uint16));

        for (int y=0; y<picHeight; y++) {
            for (int x=0; x<picWidth; x++) {
                output.write((char*)&compressedData[x + y*picWidth], sizeof(Uint8));
            }
        }
        cout<<"Uncompressed data size :"<< uncompressedData.size() <<"\n"<<endl;
        cout<<"Compressed data size :"<< compressedDataSize <<"\n"<<endl;
    }


    output.close();

    SDL_UpdateWindowSurface(window);
}

void readFile(string fileName) {
    SDL_Color colorPalette;
    SDL_Color color;

    Uint16 picWidth = 0;
    Uint16 picHeight = 0;

    map<int, int> compressedData;
    map<int, int> uncompressedData;
    int compressedDataSize = 0;

    char identifier[] = "  ";
    char compressionType = ' ';

    Uint8 value = 0;

    cout<<"File '" + fileName + "'.bin is being readed ..."<<endl;
    ifstream input(fileName + ".bin", ios::binary);

    input.read((char*)&identifier, sizeof(char)*2);
    input.read((char*)&picWidth, sizeof(Uint16));
    input.read((char*)&picHeight, sizeof(Uint16));

    cout<<"id: "<<identifier<<endl;
    cout<<"width: "<<picWidth<<endl;
    cout<<"height: "<<picHeight<<endl;


    if(identifier[0] == 'D') {
        for (int i=0; i<8; i++) {
            input.read((char*)&colorPalette.r, sizeof(Uint8));
            input.read((char*)&colorPalette.g, sizeof(Uint8));
            input.read((char*)&colorPalette.b, sizeof(Uint8));
            palette[i] = colorPalette;
        }
    } else if (identifier[0] == 'I') {
        setImposedRgbPalette();
    } else if (identifier[0] == 'G') {
        setGrayScalePalette();
    } else {
        cout<<"\n Unknown format !"<<identifier<<endl;
    }

    input.read((char*)&compressionType, sizeof(char));
    input.read((char*)&compressedDataSize, sizeof(Uint16));
    cout<<"compression: "<<compressionType<<endl;

    if (compressionType == 'N') {
        for (int y=0; y<picHeight; y++) {
            for (int x=0; x<picWidth; x++) {
                input.read((char*)&value, sizeof(Uint8));
                color = palette[value];
                setPixel(x, y+picHeight, color.r, color.g, color.b);
            }
        }
    } else {
        cout<<"compressed data size: "<<compressedDataSize<<endl;
        for (int i=0; i<compressedDataSize; i++) {
            input.read((char*)&value, sizeof(Uint8));
            compressedData.insert({i,(int)value});
        }

        if (compressionType == 'B') {
            uncompressedData = byteRunDecompress(compressedData, compressedDataSize);
        } else if (compressionType == 'R') {
            uncompressedData = rleDecompress(compressedData, compressedDataSize);
        } else if (compressionType == 'L') {
            uncompressedData = lzwDecompress(compressedData, compressedDataSize);
        }
        cout<<"uncompressed data size: "<<uncompressedData.size()<<endl;
        for (int y=0; y<picHeight; y++) {
            for (int x=0; x<picWidth; x++) {
                value = uncompressedData[x + y*picWidth];
                color = palette[value];
                setPixel(x, y+picHeight, color.r, color.g, color.b);
            }
        }
    }


    input.close();

    SDL_UpdateWindowSurface(window);
}

/// interface

void modifyImageMenu() {
    cout << "\n What image do you want ? \n" ;
    cout << "a, s, d, f, g, h, j, k (gradianFull), l (gradianRGB) \n" ;

    cout << "\n What modification you want to make ? \n" ;
    cout << "1 for imposed palette, 2 with dithering \n" ;
    cout << "3 for dedicated palette, 4 with dithering \n" ;
    cout << "5 for gray scale, 6 with dithering \n" ;

    cout << "\n q to go back to main menu \n \n" ;

    bool done = false;
    SDL_Event event;
    while (SDL_WaitEvent(&event) && !done) {
        switch (event.type) {
            case SDL_QUIT:
                done = true;
                break;
             case SDL_KEYDOWN: {
                if (event.key.keysym.sym == SDLK_a)
                    ladujBMP("obrazek1.bmp", 0, 0);
                else if (event.key.keysym.sym == SDLK_s)
                    ladujBMP("obrazek2.bmp", 0, 0);
                else if (event.key.keysym.sym == SDLK_d)
                    ladujBMP("obrazek3.bmp", 0, 0);
                else if (event.key.keysym.sym == SDLK_f)
                    ladujBMP("obrazek4.bmp", 0, 0);
                else if (event.key.keysym.sym == SDLK_g)
                    ladujBMP("obrazek5.bmp", 0, 0);
                else if (event.key.keysym.sym == SDLK_h)
                    ladujBMP("obrazek6.bmp", 0, 0);
                else if (event.key.keysym.sym == SDLK_j)
                    ladujBMP("obrazek7.bmp", 0, 0);
                else if (event.key.keysym.sym == SDLK_k)
                    gradiantFull();
                else if (event.key.keysym.sym == SDLK_1) {
                    cout<<"imposedPalette selected"<<endl;
                    strcpy(identifier,"IN");
                    imposedPalette(); //Working
                }
                else if (event.key.keysym.sym == SDLK_2) {
                    cout<<"imposedPaletteDithering selected"<<endl;
                    strcpy(identifier,"ID");
                    imposedPaletteDithering(); //Working
                }
                else if (event.key.keysym.sym == SDLK_3) {
                    cout<<"dedicatedPalette selected"<<endl;
                    strcpy(identifier,"DN");
                    dedicatedPalette(); //Working
                }
                else if (event.key.keysym.sym == SDLK_4) {
                    cout<<"dedicatedPaletteDithering selected"<<endl;
                    strcpy(identifier,"DD");
                    dedicatedPaletteDithering(); //Not working
                }
                else if (event.key.keysym.sym == SDLK_5) {
                    cout<<"grayScale selected"<<endl;
                    strcpy(identifier,"GN");
                    grayScale(); //Working
                }
                else if (event.key.keysym.sym == SDLK_6) {
                    cout<<"grayScaleDithering selected"<<endl;
                    strcpy(identifier,"GD");
                    grayScaleDithering(); //Working
                }
                else if (event.key.keysym.sym == SDLK_q) {
                    done = true;
                }
                else
                    break;
               }
        }
    }

    cout << "\n returning to main menu..."<<endl ;
}

void saveImageMenu() {
    cout << "\n What compression algorithm do you want to use ? \n" ;
    cout << "b for ByteRun \n" ;
    cout << "r for RLE \n" ;
    cout << "l for LZW \n" ;
    cout << "n for no compression \n" ;

    bool done = false;
    SDL_Event event;
    while (SDL_WaitEvent(&event) && !done) {
        switch (event.type) {
            case SDL_QUIT:
                done = true;
                break;
             case SDL_KEYDOWN: {
                if (event.key.keysym.sym == SDLK_b) {
                    cout<<"\n ByteRun compression selected \n"<<endl;
                    writeFile("savedImage", "byteRun");
                    cout << "image saved ! \n" ;
                    done = true;
                }
                else if (event.key.keysym.sym == SDLK_r) {
                    cout<<"\n RLE compression selected \n"<<endl;
                    writeFile("savedImage", "rle");
                    cout << "image saved ! \n" ;
                    done = true;
                }
                else if (event.key.keysym.sym == SDLK_l) {
                    cout<<"\n LZW compression selected \n"<<endl;
                    writeFile("savedImage", "lzw");
                    cout << "image saved ! \n" ;
                    done = true;
                }
                else if (event.key.keysym.sym == SDLK_n) {
                    cout<<"\n No compression selected \n"<<endl;
                    writeFile("savedImage", "no compression");
                    done = true;
                }
                else
                    break;
               }
        }
    }

    cout << "\n returning to main menu... \n" ;
}

void mainMenuMessage() {
    cout << "\n What do you want to do ? \n" ;
    cout << "w to modify an image \n" ;
    cout << "x to save image \n" ;
    cout << "c to show image from file \n" ;
    cout << "v to quit \n" ;
}

void mainMenu() {
    mainMenuMessage();
    bool done = false;
    SDL_Event event;
    while (SDL_WaitEvent(&event) && !done) {
        switch (event.type) {
            case SDL_QUIT:
                done = true;
                break;
             case SDL_KEYDOWN: {
                if (event.key.keysym.sym == SDLK_w) {
                    modifyImageMenu();
                    mainMenuMessage();
                }
                else if (event.key.keysym.sym == SDLK_x) {
                    saveImageMenu();
                    mainMenuMessage();
                }
                else if (event.key.keysym.sym == SDLK_c) {
                    readFile("savedImage");
                    cout << "\n image displayed ! \n" ;
                    cout << "returning to main menu... \n" ;
                    mainMenuMessage();
                }
                else if (event.key.keysym.sym == SDLK_v)
                    done = true;
                else
                    break;
               }
        }
    }
}

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		printf("SDL_Init Error: %s\n", SDL_GetError());
		return EXIT_FAILURE;
    }

    window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width*2, height*2, SDL_WINDOW_SHOWN);

    if (window == NULL) {
        printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    screen = SDL_GetWindowSurface(window);
    if (screen == NULL) {
        fprintf(stderr, "SDL_GetWindowSurface Error: %s\n", SDL_GetError());
    return false;
    }
    SDL_UpdateWindowSurface(window);

    mainMenu();

    if (screen) {
        SDL_FreeSurface(screen);
    }

    if (window) {
        SDL_DestroyWindow(window);
    }

    SDL_Quit();
    return 0;
}

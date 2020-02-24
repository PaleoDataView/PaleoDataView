#include "colorscheme.h"

ColorScheme::ColorScheme()
{

}

QColor ColorScheme::getColor(float value,float min,float max,int n){
    // ColorScheme: Spectrum
    int r_col=0,g_col=0,b_col=0;
    float v=(value-min)/(max-min);
    if (n==0){

        v = v * 6; // Contour color sheme
        if (v < 1) {
            r_col = (int) ((v * 255));
            g_col = (int) (0);
            b_col = (int) ((v * 255));
        }
        if (v >= 1 && v < 2) {
            r_col = (int) (255 - ((v - 1) * 255));
            g_col = (int) (0);
            b_col = (int) (255);
        }
        if (v >= 2 && v < 3) {
            r_col = (int) (0);
            g_col = (int) ((v - 2) * 255);
            b_col = (int) (255 - ((v - 2) * 255));
        }
        if (v >= 3 && v < 4) {
            r_col = (int) ((v - 3) * 255);
            g_col = (int) (255);
            b_col = (int) (0);
        }
        if (v >= 4 && v < 5) {
            r_col = (int) (255);
            g_col = (int) (255 - ((v - 4) * 255));
            b_col = (int) (0);
        }
        if (v >= 5) {
            r_col = (int) (255);
            g_col = (int) ((v - 5) * 255);
            b_col = (int) ((v - 5) * 255);
        }
    }
    // Reverse Rainbow
    if (n==1){

        v = v * 6; // Contour color sheme
        if (v < 1) {
            r_col = (int) (255);
            g_col = (int) (255-(v - 0) * 255);
            b_col = (int) (255-(v - 0) * 255);

        }
        if (v >= 1 && v < 2) {
            r_col = (int) (255);
            g_col = (int) ( ((v - 1) * 255));
            b_col = (int) (0);

        }
        if (v >= 2 && v < 3) {
            r_col = (int) (255-(v - 2) * 255);
            g_col = (int) (255);
            b_col = (int) (0);

        }
        if (v >= 3 && v < 4) {
            r_col = (int) (0);
            g_col = (int) (255-(v - 3) * 255);
            b_col = (int) (((v - 3) * 255));
        }
        if (v >= 4 && v < 5) {
            r_col = (int) ( ((v - 4) * 255));
            g_col = (int) (0);
            b_col = (int) (255);
        }
        if (v >= 5) {
            r_col = (int) (255-((v-5) * 255));
            g_col = (int) (0);
            b_col = (int) (255-((v-5) * 255));
        }

    }
    if (n==2) {
        if (v < 1) {
            r_col = (int) ((v * 255));
            g_col = (int) ((v * 255));
            b_col = (int) ((v * 255));
        }
    }
    if (n==3) {
        v = v * 2; // Contour color sheme
        if (v < 1) {
            QColor c;
            c.setHsv(0,255,v*255);

            r_col = c.red();
            g_col = c.green();
            b_col = c.blue();
        }
        if (v >= 1 && v < 2) {
            QColor c;
            c.setHsv(0,255-(v-1)*255,255);
            r_col = c.red();
            g_col = c.green();
            b_col = c.blue();
        }
    }
    if (n==4) {
        v = v * 2; // Contour color sheme
        if (v < 1) {
            QColor c;
            c.setHsv(120,255,v*255);

            r_col = c.red();
            g_col = c.green();
            b_col = c.blue();
        }
        if (v >= 1 && v < 2) {
            QColor c;
            c.setHsv(120,255-(v-1)*255,255);
            r_col = c.red();
            g_col = c.green();
            b_col = c.blue();
        }
    }
    if (n==5) {
        v = v * 2; // Contour color sheme
        if (v < 1) {
            QColor c;
            c.setHsv(240,255,v*255);

            r_col = c.red();
            g_col = c.green();
            b_col = c.blue();
        }
        if (v >= 1 && v < 2) {
            QColor c;
            c.setHsv(240,255-(v-1)*255,255);
            r_col = c.red();
            g_col = c.green();
            b_col = c.blue();
        }
    }



    return QColor(r_col,g_col,b_col);
}

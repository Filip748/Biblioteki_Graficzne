#ifndef __ChartClass__
#define __ChartClass__

#include <memory>
#include "ConfigClass.h"
class Matrix;

class ChartClass
{
    private:
        std::shared_ptr<ConfigClass> cfg;
        int x_step;         
        double x_min,x_max; 
        double y_min,y_max; 
        double GetFunctionValue(double x); 
        
        void line2d(Matrix t,double *x1,double *y1,double *x2,double *y2);
       
    public:
        ChartClass(std::shared_ptr<ConfigClass> c);
        void Set_Range();
        double Get_Y_min(); 
        double Get_Y_max(); 
        void Draw(wxDC *dc, int w, int h);  
};

#endif

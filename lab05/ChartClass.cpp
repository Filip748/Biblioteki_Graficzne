#include <wx/dc.h>
#include <memory>
#include <cmath>

#include "ChartClass.h"
#include "vecmat.h"

ChartClass::ChartClass(std::shared_ptr<ConfigClass> c)
{
    cfg= std::move(c);
    x_step=200;
}

void ChartClass::Set_Range()
{
 double xmin=9999.9,xmax=-9999.9,ymin=9999.9,ymax=-9999.9;
 double x,y,step;
 int i;

 xmin=cfg->Get_x_start();
 xmax=cfg->Get_x_stop();

 step=(cfg->Get_x_stop()-cfg->Get_x_start())/(double)x_step;
 x=cfg->Get_x_start();

 for (i=0;i<=x_step;i++)
  {
   y=GetFunctionValue(x);
   if (y>ymax) ymax=y;
   if (y<ymin) ymin=y;
   x=x+step;
  }

 y_min=ymin;
 y_max=ymax;
 x_min=xmin;
 x_max=xmax;
}


double ChartClass::GetFunctionValue(double x)
{
 if (cfg->Get_F_type()==1) return x*x;
 if (cfg->Get_F_type()==2) return 0.5*exp(4*x-3*x*x);
 return x+sin(x*4.0);
}

// ---- macierze przeksztalcen (wspolrzedne jednorodne) ----

static Matrix Set_Identity()
{
 Matrix m;
 m.data[0][0]=1.0;
 m.data[1][1]=1.0;
 return m;
}

static Matrix Set_Translate(double tx, double ty)
{
 Matrix m=Set_Identity();
 m.data[0][2]=tx;
 m.data[1][2]=ty;
 return m;
}

static Matrix Set_Scale(double sx, double sy)
{
 Matrix m;
 m.data[0][0]=sx;
 m.data[1][1]=sy;
 return m;
}

// obrot o kat alpha (w stopniach) przeciwnie do ruchu wskazowek zegara
// (w ukladzie ekranu os y skierowana jest w dol)
static Matrix Set_Rotate(double alpha)
{
 double a=alpha*3.14159265358979323846/180.0;
 Matrix m;
 m.data[0][0]= cos(a); m.data[0][1]=sin(a);
 m.data[1][0]=-sin(a); m.data[1][1]=cos(a);
 return m;
}

// "ladny" krok podzialki (1, 2 lub 5 razy potega 10)
static double NiceStep(double range)
{
 double raw=range/4.0;
 double p=pow(10.0,floor(log10(raw)));
 double f=raw/p;
 if (f<1.5) return p;
 if (f<3.5) return 2.0*p;
 if (f<7.5) return 5.0*p;
 return 10.0*p;
}

void ChartClass::Draw(wxDC *dc, int w, int h)
{
 const int margin=10;

 dc->SetBackground(wxBrush(wxColour(255, 255, 255)));
 dc->Clear();
 dc->SetPen(wxPen(wxColour(255, 0, 0)));
 dc->SetBrush(*wxTRANSPARENT_BRUSH);
 dc->DrawRectangle(margin, margin, w - 2*margin, h - 2*margin);
 dc->SetClippingRegion(margin, margin, w - 2*margin, h - 2*margin);

 double x0=cfg->Get_x0(), y0=cfg->Get_y0();
 double x1=cfg->Get_x1(), y1=cfg->Get_y1();
 if (x1==x0 || y1==y0) { dc->DestroyClippingRegion(); return; }

 Set_Range();

 // skalowanie i okienkowanie: prostokat swiata [x0,x1]x[y0,y1] -> obszar wykresu
 double sx=(w-2*margin)/(x1-x0);
 double sy=(h-2*margin)/(y1-y0);
 Matrix window=Set_Translate(margin, h-margin)*Set_Scale(sx,-sy)*Set_Translate(-x0,-y0);

 // przesuniecie zadane przez uzytkownika (w jednostkach swiata)
 Matrix shift=Set_Translate(cfg->Get_dX()*sx, -cfg->Get_dY()*sy);
 Matrix ws=shift*window;

 // punkt obrotu: srodek ekranu lub srodek ukladu (po przesunieciu)
 double cx,cy;
 if (cfg->RotateScreenCenter())
  {
   cx=w/2.0;
   cy=h/2.0;
  }
 else
  {
   Vector o; o.Set(0.0,0.0);
   Vector so=ws*o;
   cx=so.GetX();
   cy=so.GetY();
  }
 double alpha=cfg->Get_Alpha();
 Matrix t=Set_Translate(cx,cy)*Set_Rotate(alpha)*Set_Translate(-cx,-cy)*ws;

 double ax1,ay1,ax2,ay2;

 // osie ukladu
 dc->SetPen(wxPen(wxColour(0, 0, 255)));
 ax1=x0; ay1=0.0; ax2=x1; ay2=0.0;
 line2d(t,&ax1,&ay1,&ax2,&ay2);
 dc->DrawLine(ax1,ay1,ax2,ay2);
 ax1=0.0; ay1=y0; ax2=0.0; ay2=y1;
 line2d(t,&ax1,&ay1,&ax2,&ay2);
 dc->DrawLine(ax1,ay1,ax2,ay2);

 // podzialki z opisami (na kazdej osi co najmniej jedna na czesci ujemnej
 // i jedna na dodatniej)
 const double tick=4.0; // dlugosc podzialki w pikselach
 auto drawTick=[&](double v, bool xAxis)
  {
   double tx1,ty1,tx2,ty2;
   Vector p;
   if (xAxis) { tx1=v; ty1=-tick/sy; tx2=v; ty2=tick/sy; p.Set(v,-2.0*tick/sy); }
   else       { tx1=-tick/sx; ty1=v; tx2=tick/sx; ty2=v; p.Set(2.0*tick/sx,v); }
   line2d(t,&tx1,&ty1,&tx2,&ty2);
   dc->DrawLine(tx1,ty1,tx2,ty2);
   Vector sp=t*p;
   dc->DrawRotatedText(wxString::Format(wxT("%g"), v), sp.GetX(), sp.GetY(), alpha);
  };
 auto drawTicks=[&](double lo, double hi, bool xAxis)
  {
   double step=NiceStep(hi-lo);
   bool neg=false,pos=false;
   for (double v=ceil(lo/step)*step; v<=hi; v+=step)
    {
     if (fabs(v)<step*1e-6) continue;
     drawTick(v,xAxis);
     if (v<0) neg=true; else pos=true;
    }
   if (lo<0 && !neg) drawTick(lo/2.0,xAxis);
   if (hi>0 && !pos) drawTick(hi/2.0,xAxis);
  };
 drawTicks(x0,x1,true);
 drawTicks(y0,y1,false);

 // wykres funkcji
 dc->SetPen(wxPen(wxColour(0, 160, 0)));
 double dx=(x_max-x_min)/(double)x_step;
 for (int i=0;i<x_step;i++)
  {
   ax1=x_min+i*dx;     ay1=GetFunctionValue(ax1);
   ax2=x_min+(i+1)*dx; ay2=GetFunctionValue(ax2);
   line2d(t,&ax1,&ay1,&ax2,&ay2);
   dc->DrawLine(ax1,ay1,ax2,ay2);
  }

 dc->DestroyClippingRegion();
}

void ChartClass::line2d(Matrix t, double *x1, double *y1, double *x2, double *y2)
{
 Vector p1,p2;
 p1.Set(*x1,*y1);
 p2.Set(*x2,*y2);
 p1=t*p1;
 p2=t*p2;
 *x1=p1.GetX(); *y1=p1.GetY();
 *x2=p2.GetX(); *y2=p2.GetY();
}

double ChartClass::Get_Y_min()
{
    Set_Range();
    return y_min;
}

double ChartClass::Get_Y_max()
{
    Set_Range();
    return y_max;
}

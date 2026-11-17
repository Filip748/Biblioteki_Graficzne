#include "GUIMyFrame1.h"
#include <vector>
#include <fstream>
#include <cmath>
#include <algorithm>
#include "vecmat.h"

struct Point {
 float x, y, z;
 Point(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}
};

struct Color {
 int R, G, B;
 Color(int _R, int _G, int _B) : R(_R), G(_G), B(_B) {}
};

struct Segment {
 Point begin, end;
 Color color;
 Segment(Point _begin, Point _end, Color _color) : begin(_begin), end(_end), color(_color) {}
};

std::vector<Segment> data;

GUIMyFrame1::GUIMyFrame1( wxWindow* parent )
:
MyFrame1( parent )
{
 m_button_load_geometry->SetLabel(_("Wczytaj Geometri\u0119"));
 m_staticText25->SetLabel(_("Obr\u00F3t X:"));
 m_staticText27->SetLabel(_("Obr\u00F3t Y:"));
 m_staticText29->SetLabel(_("Obr\u00F3t Z:"));

 WxSB_TranslationX->SetRange(0, 200); WxSB_TranslationX->SetValue(100);
 WxSB_TranslationY->SetRange(0, 200); WxSB_TranslationY->SetValue(100);
 WxSB_TranslationZ->SetRange(0, 200); WxSB_TranslationZ->SetValue(100);

 WxSB_RotateX->SetRange(0, 360); WxSB_RotateX->SetValue(0);
 WxSB_RotateY->SetRange(0, 360); WxSB_RotateY->SetValue(0);
 WxSB_RotateZ->SetRange(0, 360); WxSB_RotateZ->SetValue(0);

 WxSB_ScaleX->SetRange(1, 200); WxSB_ScaleX->SetValue(100);
 WxSB_ScaleY->SetRange(1, 200); WxSB_ScaleY->SetValue(100);
 WxSB_ScaleZ->SetRange(1, 200); WxSB_ScaleZ->SetValue(100);
}

void GUIMyFrame1::WxPanel_Repaint( wxUpdateUIEvent& event )
{
 Repaint();
}

void GUIMyFrame1::m_button_load_geometry_click( wxCommandEvent& event )
{
 wxFileDialog WxOpenFileDialog(this, wxT("Choose a file"), wxT(""), wxT(""), wxT("Geometry file (*.geo)|*.geo"), wxFD_OPEN | wxFD_FILE_MUST_EXIST);

 if (WxOpenFileDialog.ShowModal() == wxID_OK)
 {
  double x1, y1, z1, x2, y2, z2;
  int r, g, b;

  std::ifstream in(WxOpenFileDialog.GetPath().ToAscii());
  if (in.is_open())
  {
   data.clear();
   while (!in.eof())
   {
    in >> x1 >> y1 >> z1 >> x2 >> y2 >> z2 >> r >> g >> b;
    data.push_back(Segment(Point(x1, y1, z1), Point(x2, y2, z2), Color(r, g, b)));
   }
   in.close();
  }
 }
}

void GUIMyFrame1::Scrolls_Updated( wxScrollEvent& event )
{
WxST_TranslationX->SetLabel(wxString::Format(wxT("%g"), (WxSB_TranslationX->GetValue() - 100) / 50.0));
WxST_TranslationY->SetLabel(wxString::Format(wxT("%g"), (WxSB_TranslationY->GetValue() - 100) / 50.0));
WxST_TranslationZ->SetLabel(wxString::Format(wxT("%g"), (WxSB_TranslationZ->GetValue() - 100) / 50.0));

WxST_RotateX->SetLabel(wxString::Format(wxT("%d"), WxSB_RotateX->GetValue()));
WxST_RotateY->SetLabel(wxString::Format(wxT("%d"), WxSB_RotateY->GetValue()));
WxST_RotateZ->SetLabel(wxString::Format(wxT("%d"), WxSB_RotateZ->GetValue()));

WxST_ScaleX->SetLabel(wxString::Format(wxT("%g"), WxSB_ScaleX->GetValue() / 100.0));
WxST_ScaleY->SetLabel(wxString::Format(wxT("%g"), WxSB_ScaleY->GetValue() / 100.0));
WxST_ScaleZ->SetLabel(wxString::Format(wxT("%g"), WxSB_ScaleZ->GetValue() / 100.0));


Repaint();
}


namespace {

const double PI = 3.14159265358979323846;
const double CAMERA_DISTANCE = 2.0;
const double D = 1.0;               

Matrix4 Identity()
{
 Matrix4 m;
 m.data[0][0] = m.data[1][1] = m.data[2][2] = 1.0;
 return m;
}

Matrix4 Translation(double tx, double ty, double tz)
{
 Matrix4 m = Identity();
 m.data[0][3] = tx;
 m.data[1][3] = ty;
 m.data[2][3] = tz;
 return m;
}

Matrix4 Scale(double sx, double sy, double sz)
{
 Matrix4 m;
 m.data[0][0] = sx;
 m.data[1][1] = sy;
 m.data[2][2] = sz;
 return m;
}

Matrix4 RotationX(double deg)
{
 double a = deg * PI / 180.0, c = cos(a), s = sin(a);
 Matrix4 m = Identity();
 m.data[1][1] = c; m.data[1][2] = -s;
 m.data[2][1] = s; m.data[2][2] = c;
 return m;
}

Matrix4 RotationY(double deg)
{
 double a = deg * PI / 180.0, c = cos(a), s = sin(a);
 Matrix4 m = Identity();
 m.data[0][0] = c;  m.data[0][2] = s;
 m.data[2][0] = -s; m.data[2][2] = c;
 return m;
}

Matrix4 RotationZ(double deg)
{
 double a = deg * PI / 180.0, c = cos(a), s = sin(a);
 Matrix4 m = Identity();
 m.data[0][0] = c; m.data[0][1] = -s;
 m.data[1][0] = s; m.data[1][1] = c;
 return m;
}

} // namespace

void GUIMyFrame1::Repaint()
{
 wxClientDC dc1(WxPanel);
 wxBufferedDC dc(&dc1);
 int w, h;
 WxPanel->GetSize(&w, &h);
 DrawScene(&dc, w, h);
}

void GUIMyFrame1::DrawScene(wxDC *dc, int w, int h)
{
 dc->SetBackground(wxBrush(wxColour(255, 255, 255)));
 dc->Clear();
 if (w <= 0 || h <= 0) return;

 double tx = (WxSB_TranslationX->GetValue() - 100) / 50.0;
 double ty = (WxSB_TranslationY->GetValue() - 100) / 50.0;
 double tz = (WxSB_TranslationZ->GetValue() - 100) / 50.0;

 double rx = WxSB_RotateX->GetValue();
 double ry = WxSB_RotateY->GetValue();
 double rz = WxSB_RotateZ->GetValue();

 double sx = WxSB_ScaleX->GetValue() / 100.0;
 double sy = WxSB_ScaleY->GetValue() / 100.0;
 double sz = WxSB_ScaleZ->GetValue() / 100.0;

 Matrix4 rotation = RotationX(rx) * RotationY(ry) * RotationZ(rz);
 Matrix4 model = Translation(tx, ty, tz) * Scale(sx, sy, sz) * rotation;
 Matrix4 view = Translation(0.0, 0.0, CAMERA_DISTANCE);
 Matrix4 transform = view * model;


 double half = std::min(w, h) / 2.0;
 auto toScreen = [&](Vector4 &v) {
  double xp = v.GetX() * D / v.GetZ();
  double yp = v.GetY() * D / v.GetZ();
  return wxPoint((int)(w / 2.0 + xp * half), (int)(h / 2.0 - yp * half));
 };

 for (const Segment &seg : data)
 {
  Vector4 p1, p2;
  p1.Set(seg.begin.x, seg.begin.y, seg.begin.z);
  p2.Set(seg.end.x, seg.end.y, seg.end.z);
  p1 = transform * p1;
  p2 = transform * p2;

  bool out1 = p1.GetZ() < D;
  bool out2 = p2.GetZ() < D;

  if (out1 && out2) continue;

  if (out1 || out2)
  {
   double t = (D - p1.GetZ()) / (p2.GetZ() - p1.GetZ());
   Vector4 cut;
   cut.Set(p1.GetX() + t * (p2.GetX() - p1.GetX()),
           p1.GetY() + t * (p2.GetY() - p1.GetY()),
           D);
   if (out1) p1 = cut; else p2 = cut;
  }

  dc->SetPen(wxPen(wxColour(seg.color.R, seg.color.G, seg.color.B)));
  dc->DrawLine(toScreen(p1), toScreen(p2));
 }
}
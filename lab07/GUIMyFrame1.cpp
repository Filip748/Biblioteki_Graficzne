#include "GUIMyFrame1.h"

GUIMyFrame1::GUIMyFrame1( wxWindow* parent )
:
MyFrame1( parent )
{
 m_staticText1->SetLabel(_(L"Jasno\u015B\u0107"));
 m_b_threshold->SetLabel(_(L"Pr\u00F3g 128"));
 this->SetBackgroundColour(wxColor(192, 192, 192));
 m_scrolledWindow->SetScrollbars(25, 25, 52, 40);
 m_scrolledWindow->SetBackgroundColour(wxColor(192, 192, 192));
#ifdef __WXOSX__
 m_scrolledWindow->Bind(wxEVT_PAINT, [this](wxPaintEvent&) {
  wxPaintDC dc(m_scrolledWindow);
  m_scrolledWindow->DoPrepareDC(dc);
  if (Img_Cpy.IsOk()) dc.DrawBitmap(wxBitmap(Img_Cpy), 0, 0, true);
 });
#endif
}

void GUIMyFrame1::m_scrolledWindow_update( wxUpdateUIEvent& event )
{
#ifdef __WXOSX__
 static wxImage last;
 if (!last.IsSameAs(Img_Cpy)) { last = Img_Cpy; Repaint(); }
#else
 Repaint();
#endif
}

void GUIMyFrame1::m_b_grayscale_click( wxCommandEvent& event )
{
 // TO DO: Konwersja do skali szarosci    
 Img_Cpy = Img_Org.ConvertToGreyscale();
}

void GUIMyFrame1::m_b_blur_click( wxCommandEvent& event )
{
 // TO DO: Rozmywanie obrazu (blur)
 Img_Cpy = Img_Org.Blur(5);
}

void GUIMyFrame1::m_b_mirror_click( wxCommandEvent& event )
{
 // TO DO: Odbicie lustrzane
 Img_Cpy = Img_Org.Mirror(true);
}

void GUIMyFrame1::m_b_replace_click( wxCommandEvent& event )
{
 // TO DO: Zamiana kolorow 
 Img_Cpy = Img_Org.Copy();
 Img_Cpy.Replace(254, 0, 0, 0, 0, 255);
}

void GUIMyFrame1::m_b_rescale_click( wxCommandEvent& event )
{
 // TO DO: Zmiana rozmiarow do 320x240
 Img_Cpy = Img_Org.Scale(320, 240);
 wxClientDC dc(m_scrolledWindow);
 m_scrolledWindow->DoPrepareDC(dc);
 dc.Clear();                        
 m_scrolledWindow->Scroll(0, 0); 
}

void GUIMyFrame1::m_b_rotate_click( wxCommandEvent& event )
{
 // TO DO: Obrot o 30 stopni
 Img_Cpy = Img_Org.Rotate(30.0 * 3.14159265358979 / 180.0, wxPoint(Img_Org.GetWidth() / 2, Img_Org.GetHeight() / 2));
}

void GUIMyFrame1::m_b_rotate_hue_click( wxCommandEvent& event )
{
 // TO DO: Przesuniecie Hue o 180 stopni
 Img_Cpy = Img_Org.Copy();
 Img_Cpy.RotateHue(0.5);         
}

void GUIMyFrame1::m_b_mask_click( wxCommandEvent& event )
{
 // TO DO: Ustawienie maski obrazu
 Img_Cpy = Img_Org.Copy();
 Img_Cpy.SetMaskFromImage(Img_Mask, 0, 0, 0);
 wxClientDC dc(m_scrolledWindow);
 m_scrolledWindow->DoPrepareDC(dc);
 dc.Clear();                       
}

void GUIMyFrame1::m_s_brightness_scroll( wxScrollEvent& event )
{
// Tutaj, w reakcji na zmiane polozenia suwaka, wywolywana jest funkcja
// Brightness(...), ktora zmienia jasnosc. W tym miejscu nic nie
// zmieniamy. Do uzupelnienia pozostaje funkcja Brightness(...)
Brightness(m_s_brightness->GetValue() - 100);
Repaint();
}

void GUIMyFrame1::m_s_contrast_scroll( wxScrollEvent& event )
{
// Tutaj, w reakcji na zmiane polozenia suwaka, wywolywana jest funkcja
// Contrast(...), ktora zmienia kontrast. W tym miejscu nic nie
// zmieniamy. Do uzupelnienia pozostaje funkcja Contrast(...)
Contrast(m_s_contrast->GetValue() - 100);
Repaint();
}

void GUIMyFrame1::m_b_prewitt_click( wxCommandEvent& event )
{
 // TO DO: Pionowa maska Prewitta
 Img_Cpy = Img_Org.Copy();
 const int w = Img_Org.GetWidth(), h = Img_Org.GetHeight();
 const unsigned char* src = Img_Org.GetData();
 unsigned char* dst = Img_Cpy.GetData();
 for (int y = 1; y < h - 1; ++y)
  for (int x = 1; x < w - 1; ++x)
   for (int c = 0; c < 3; ++c)
   {
    int sum = 0;
    for (int dy = -1; dy <= 1; ++dy)
     sum += src[((y + dy) * w + (x + 1)) * 3 + c] - src[((y + dy) * w + (x - 1)) * 3 + c];
    if (sum < 0) sum = 0;
    if (sum > 255) sum = 255;
    dst[(y * w + x) * 3 + c] = (unsigned char)sum;
   }
}

void GUIMyFrame1::m_b_threshold_click( wxCommandEvent& event )
{
 // TO DO: Prog o wartosci 128 dla kazdego kanalu niezaleznie
 Img_Cpy = Img_Org.Copy();
 unsigned char* data = Img_Cpy.GetData();
 const long n = (long)Img_Cpy.GetWidth() * Img_Cpy.GetHeight() * 3;
 for (long i = 0; i < n; ++i)
  data[i] = data[i] > 128 ? 255 : 0;
}


void GUIMyFrame1::Contrast(int value)
{
 // TO DO: Zmiana kontrastu obrazu. value moze przyjmowac wartosci od -100 do 100
 Img_Cpy = Img_Org.Copy();
 unsigned char* data = Img_Cpy.GetData();
 const long n = (long)Img_Cpy.GetWidth() * Img_Cpy.GetHeight() * 3;
 const double factor = (100.0 + value) / (101.0 - value);
 for (long i = 0; i < n; ++i)
 {
  int v = (int)((data[i] - 128) * factor + 128);
  if (v < 0) v = 0;
  if (v > 255) v = 255;
  data[i] = (unsigned char)v;
 }
}

void GUIMyFrame1::Repaint()
{
#ifdef __WXOSX__
 m_scrolledWindow->Refresh();       
 return;
#endif
 wxBitmap bitmap(Img_Cpy);          
 wxClientDC dc(m_scrolledWindow);  
 m_scrolledWindow->DoPrepareDC(dc); 
 dc.DrawBitmap(bitmap, 0, 0, true);
}

void GUIMyFrame1::Brightness(int value)
{
 // TO DO: Zmiana jasnosci obrazu. value moze przyjmowac wartosci od -100 do 100
 Img_Cpy = Img_Org.Copy();
 unsigned char* data = Img_Cpy.GetData();
 const long n = (long)Img_Cpy.GetWidth() * Img_Cpy.GetHeight() * 3;
 for (long i = 0; i < n; ++i)
 {
  int v = data[i] + value;
  if (v < 0) v = 0;
  if (v > 255) v = 255;
  data[i] = (unsigned char)v;
 }
}
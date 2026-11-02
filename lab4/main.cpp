#include <wx/wx.h>
#include <wx/dcbuffer.h>
#include <wx/colordlg.h>
#include <wx/filedlg.h>
#include <cmath>
#include <wx/generic/filedlgg.h>

class MyFrame : public wxFrame {
public:
    MyFrame() : wxFrame(nullptr, wxID_ANY, "Lab04", wxDefaultPosition, wxSize(800, 550)) {
        wxImage::AddHandler(new wxPNGHandler());
        wxBoxSizer* mainSizer = new wxBoxSizer(wxHORIZONTAL);
        m_panel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxFULL_REPAINT_ON_RESIZE);
        m_panel->SetBackgroundColour(*wxWHITE);
        mainSizer->Add(m_panel, 1, wxEXPAND | wxALL, 5);

        wxBoxSizer* ctrlSizer = new wxBoxSizer(wxVERTICAL);

        m_btnSave = new wxButton(this, wxID_ANY, "Save");
        ctrlSizer->Add(m_btnSave, 0, wxALIGN_CENTER | wxALL, 5);

        m_cbBanana = new wxCheckBox(this, wxID_ANY, "banana");
        ctrlSizer->Add(m_cbBanana, 0, wxALIGN_CENTER | wxALL, 5);

        m_scrollBar = new wxScrollBar(this, wxID_ANY, wxDefaultPosition, wxSize(180, -1), wxSB_HORIZONTAL);
        m_scrollBar->SetScrollbar(0, 1, 100, 1);
        ctrlSizer->Add(m_scrollBar, 0, wxEXPAND | wxALL, 5);

        m_gauge = new wxGauge(this, wxID_ANY, 100, wxDefaultPosition, wxSize(180, -1), wxGA_HORIZONTAL);
        ctrlSizer->Add(m_gauge, 0, wxEXPAND | wxALL, 5);

        m_btnColor = new wxButton(this, wxID_ANY, "Star color");
        ctrlSizer->Add(m_btnColor, 0, wxALIGN_CENTER | wxALL, 5);

        m_textCtrl = new wxTextCtrl(this, wxID_ANY, "word", wxDefaultPosition, wxSize(150, -1));
        ctrlSizer->Add(m_textCtrl, 0, wxALIGN_CENTER | wxALL, 5);

        wxString choices[] = { "star", "moon", "sun" };
        m_choice = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxSize(120, -1), 3, choices);
        m_choice->SetSelection(0);
        ctrlSizer->Add(m_choice, 0, wxALIGN_CENTER | wxALL, 5);

        mainSizer->Add(ctrlSizer, 0, wxALIGN_TOP | wxALL, 10);
        SetSizer(mainSizer);

        this->Bind(wxEVT_UPDATE_UI, &MyFrame::OnUpdateUI, this);
        m_panel->Bind(wxEVT_PAINT, &MyFrame::OnPaint, this);

        m_btnSave->Bind(wxEVT_BUTTON, &MyFrame::OnSave, this);
        m_cbBanana->Bind(wxEVT_CHECKBOX, &MyFrame::OnBananaCheck, this);
        m_scrollBar->Bind(wxEVT_SCROLL_THUMBTRACK, &MyFrame::OnScroll, this);
        m_scrollBar->Bind(wxEVT_SCROLL_CHANGED, &MyFrame::OnScroll, this);
        m_scrollBar->Bind(wxEVT_SCROLL_LINEUP, &MyFrame::OnScroll, this);
        m_scrollBar->Bind(wxEVT_SCROLL_LINEDOWN, &MyFrame::OnScroll, this);
        m_scrollBar->Bind(wxEVT_SCROLL_PAGEUP, &MyFrame::OnScroll, this);
        m_scrollBar->Bind(wxEVT_SCROLL_PAGEDOWN, &MyFrame::OnScroll, this);
        m_btnColor->Bind(wxEVT_BUTTON, &MyFrame::OnStarColor, this);
        m_textCtrl->Bind(wxEVT_TEXT, &MyFrame::OnTextChange, this);
        m_choice->Bind(wxEVT_CHOICE, &MyFrame::OnShapeChoice, this);

        Centre();
    }

    void Draw() {
        wxSize size = m_panel->GetClientSize();
        if (size.GetWidth() <= 0 || size.GetHeight() <= 0) return;

        m_bufferBitmap = wxBitmap(size.GetWidth(), size.GetHeight());

        wxClientDC clientDC(m_panel);
        wxBufferedDC dc(&clientDC, m_bufferBitmap);

        dc.SetBackground(*wxWHITE_BRUSH);
        dc.Clear();

        dc.SetDeviceOrigin(size.GetWidth() / 2, size.GetHeight() / 2);

//////////////////// star moon sun
        int shape = m_choice->GetSelection();
        if (shape == 0) { 
            dc.SetPen(wxPen(m_starColor, 1));
            dc.SetBrush(wxBrush(m_starColor));
            wxPoint starPoints[5] = {
                wxPoint(-65, -85),
                wxPoint(-45, -35),
                wxPoint(-90, -65),
                wxPoint(-40, -65),
                wxPoint(-85, -35)
            };
            dc.DrawPolygon(5, starPoints, 0, 0, wxODDEVEN_RULE);
        } else if (shape == 1) { 
            dc.SetPen(wxPen(*wxLIGHT_GREY, 1));
            dc.SetBrush(wxBrush(*wxLIGHT_GREY));
            dc.DrawCircle(-65, -60, 25);
            dc.SetPen(wxPen(*wxWHITE, 1));
            dc.SetBrush(wxBrush(*wxWHITE));
            dc.DrawCircle(-75, -70, 20);
            dc.SetPen(wxPen(*wxBLACK, 2));
        } else if (shape == 2) { 
            dc.SetPen(wxPen(*wxYELLOW, 1));
            dc.SetBrush(wxBrush(*wxYELLOW));
            dc.DrawCircle(-65, -60, 25);
        }

////////////////////////////////
        dc.SetBrush(*wxTRANSPARENT_BRUSH);

        // body
        dc.SetPen(wxPen(*wxBLACK, 3));
        dc.DrawLine(0, -20, 0, 65);

        dc.SetPen(wxPen(*wxBLACK, 3));

        // legs
        dc.DrawLine(0, 65, -35, 105);
        dc.DrawLine(0, 65, 35, 105);

        // move hand
        double rad = (m_angle * 0.8) * M_PI / 180.0;
        int armLength = 50;
        int handX = -static_cast<int>(armLength * cos(rad - 0.4));
        int handY = -static_cast<int>(armLength * sin(rad - 0.4));
        dc.DrawLine(0, 0, handX, handY);

        // arm
        dc.DrawLine(0, 0, 45, 25);

        // head
        dc.DrawCircle(0, -42, 22);

        dc.SetPen(wxPen(*wxBLACK, 1));

        // eyes
        dc.DrawCircle(-9, -46, 4);
        dc.DrawCircle(9, -46, 4);

        // mouth banan
        if (m_hasBanana) {
            dc.DrawEllipticArc(-12, -38, 24, 14, 190, 350);

            if (m_bananaBitmap.IsOk()) {
                dc.DrawBitmap(m_bananaBitmap, handX - m_bananaBitmap.GetWidth() / 2, handY - m_bananaBitmap.GetHeight() / 2, true);
            }
        } else {
            dc.DrawEllipticArc(-12, -32, 24, 14, 10, 170);
        }
        // strings
        dc.SetTextForeground(*wxBLACK);

        wxFont fontSmall(15, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
        dc.SetFont(fontSmall);
        dc.DrawText(m_text, -120, 80);

        wxFont fontLarge(30, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
        dc.SetFont(fontLarge);
        dc.DrawRotatedText(m_text, 80, 80, 90.0);
    }

private:
    wxPanel* m_panel;
    wxButton* m_btnSave;
    wxCheckBox* m_cbBanana;
    wxScrollBar* m_scrollBar;
    wxGauge* m_gauge;
    wxButton* m_btnColor;
    wxTextCtrl* m_textCtrl;
    wxChoice* m_choice;

    wxColour m_starColor = *wxBLACK;
    wxBitmap m_bananaBitmap;
    wxBitmap m_bufferBitmap;
    int m_angle = 0;
    bool m_hasBanana = false;
    wxString m_text = "word";

    void OnPaint(wxPaintEvent& event) {
        Draw();
    }

    void OnUpdateUI(wxUpdateUIEvent& event) {
        Draw();
    }

    void OnBananaCheck(wxCommandEvent& event) {
        m_hasBanana = m_cbBanana->IsChecked();
        if (m_hasBanana) {
            wxImage img;
            if (wxFileExists("banan.png") && img.LoadFile("banan.png", wxBITMAP_TYPE_PNG)) {
                img.Rescale(35, 35, wxIMAGE_QUALITY_HIGH);
                m_bananaBitmap = wxBitmap(img);
            } else {
                wxImage fallback(35, 35);
                fallback.InitAlpha();
                for (int x = 0; x < 35; ++x)
                    for (int y = 0; y < 35; ++y)
                        fallback.SetAlpha(x, y, 0);
                m_bananaBitmap = wxBitmap(fallback);
                wxMemoryDC memDC(m_bananaBitmap);
                memDC.SetPen(wxPen(wxColour(200, 180, 0), 2));
                memDC.SetBrush(*wxYELLOW_BRUSH);
                memDC.DrawEllipticArc(5, 5, 25, 25, 200, 340);
                memDC.SelectObject(wxNullBitmap);
            }
        }
        Draw();
    }

    void OnScroll(wxScrollEvent& event) {
        m_angle = m_scrollBar->GetThumbPosition();
        m_gauge->SetValue(m_angle);
        Draw();
    }

    void OnStarColor(wxCommandEvent& event) {
        wxColourData data;
        data.SetColour(m_starColor);
        wxColourDialog dialog(this, &data);
        if (dialog.ShowModal() == wxID_OK) {
            m_starColor = dialog.GetColourData().GetColour();
            Draw();
        }
    }

    void OnTextChange(wxCommandEvent& event) {
        m_text = m_textCtrl->GetValue();
        Draw();
    }

    void OnShapeChoice(wxCommandEvent& event) {
        Draw();
    }

    void OnSave(wxCommandEvent& event) {
        wxGenericFileDialog saveFileDialog(
            this,
            "Save",
            wxEmptyString,
            "pic.png",
            "File PNG",
            wxFD_SAVE | wxFD_OVERWRITE_PROMPT
        );

        if (saveFileDialog.ShowModal() == wxID_CANCEL)
            return;

        Draw();
        if (m_bufferBitmap.IsOk()) {
            wxImage saveImg = m_bufferBitmap.ConvertToImage();
            saveImg.SaveFile(saveFileDialog.GetPath(), wxBITMAP_TYPE_PNG);
        }
    }
};

class MyApp : public wxApp {
public:
    virtual bool OnInit() override {
        MyFrame* frame = new MyFrame();
        frame->Show(true);
        return true;
    }
};

wxIMPLEMENT_APP(MyApp);
// macOS-only build helper - NOT part of the lab solution.
//
// The template draws the chart with wxClientDC from a wxEVT_UPDATE_UI handler
// (GUIMyFrame1::Repaint). On macOS (Cocoa) drawing through wxClientDC is not
// shown, so the panel stays empty. This file hooks the panel's paint event via a
// global wxEventFilter and draws the same chart with a paint DC, without
// modifying any of the original source files. CMake compiles it only on Apple.

#include <wx/wx.h>
#include <wx/eventfilter.h>
#include <wx/dcbuffer.h>
#include "GUIMyFrame1.h"
#include "ConfigClass.h"
#include "ChartClass.h"

namespace {

struct Snapshot
{
    double v[9];
    int w, h, f;
    bool rot;
    bool operator!=(const Snapshot &o) const
    {
        for (int i = 0; i < 9; i++) if (v[i] != o.v[i]) return true;
        return w != o.w || h != o.h || f != o.f || rot != o.rot;
    }
};

Snapshot TakeSnapshot(GUIMyFrame1 *frame, wxWindow *panel)
{
    ConfigClass &c = *frame->cfg;
    Snapshot s = {{c.Get_x0(), c.Get_y0(), c.Get_x1(), c.Get_y1(), c.Get_Alpha(),
                   c.Get_dX(), c.Get_dY(), c.Get_x_start(), c.Get_x_stop()},
                  0, 0, c.Get_F_type(), c.RotateScreenCenter()};
    panel->GetSize(&s.w, &s.h);
    return s;
}

class MacPaintFilter : public wxEventFilter
{
    Snapshot last = {};

    static GUIMyFrame1 *ChartFrame(wxObject *obj)
    {
        wxWindow *win = wxDynamicCast(obj, wxWindow);
        if (!win) return nullptr;
        GUIMyFrame1 *frame = dynamic_cast<GUIMyFrame1 *>(win->GetParent());
        if (!frame || !frame->cfg || win != frame->WxPanel) return nullptr;
        return frame;
    }

public:
    int FilterEvent(wxEvent &event) override
    {
        const wxEventType t = event.GetEventType();

        if (t == wxEVT_PAINT)
        {
            GUIMyFrame1 *frame = ChartFrame(event.GetEventObject());
            if (!frame) return Event_Skip;
            wxWindow *panel = frame->WxPanel;
            wxBufferedPaintDC dc(panel);
            int w, h;
            panel->GetSize(&w, &h);
            ChartClass(frame->cfg).Draw(&dc, w, h);
            last = TakeSnapshot(frame, panel);
            return Event_Processed;
        }

        if (t == wxEVT_UPDATE_UI)
        {
            // template's Repaint() would draw via wxClientDC (invisible on macOS);
            // instead request a real repaint whenever the chart parameters change
            GUIMyFrame1 *frame = ChartFrame(event.GetEventObject());
            if (!frame) return Event_Skip;
            Snapshot now = TakeSnapshot(frame, frame->WxPanel);
            if (now != last) frame->WxPanel->Refresh(false);
            return Event_Processed;
        }

        return Event_Skip;
    }
};

struct Installer
{
    Installer() { wxEvtHandler::AddFilter(new MacPaintFilter); }
} installer;

} // namespace

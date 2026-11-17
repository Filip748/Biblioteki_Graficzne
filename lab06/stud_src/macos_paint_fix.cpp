// macOS-only build helper - NOT part of the lab solution.
//
// The template draws with wxClientDC from a wxEVT_UPDATE_UI handler
// (GUIMyFrame1::Repaint). On macOS (Cocoa) drawing through wxClientDC is not
// shown, so the panel stays empty. This file hooks the panel's paint event via a
// global wxEventFilter and draws the same scene (GUIMyFrame1::DrawScene) with a
// paint DC. CMake compiles it only on Apple.

#include <wx/wx.h>
#include <wx/eventfilter.h>
#include <wx/dcbuffer.h>
#include "GUIMyFrame1.h"

namespace {

struct Snapshot
{
    int v[9];
    int w, h;
    bool operator!=(const Snapshot &o) const
    {
        for (int i = 0; i < 9; i++) if (v[i] != o.v[i]) return true;
        return w != o.w || h != o.h;
    }
};

Snapshot TakeSnapshot(GUIMyFrame1 *f)
{
    Snapshot s = {{f->WxSB_TranslationX->GetValue(), f->WxSB_TranslationY->GetValue(),
                   f->WxSB_TranslationZ->GetValue(), f->WxSB_RotateX->GetValue(),
                   f->WxSB_RotateY->GetValue(), f->WxSB_RotateZ->GetValue(),
                   f->WxSB_ScaleX->GetValue(), f->WxSB_ScaleY->GetValue(),
                   f->WxSB_ScaleZ->GetValue()}, 0, 0};
    f->WxPanel->GetSize(&s.w, &s.h);
    return s;
}

class MacPaintFilter : public wxEventFilter
{
    Snapshot last = {};
    bool dirty = true;

    static GUIMyFrame1 *FrameOf(wxObject *obj)
    {
        wxWindow *win = wxDynamicCast(obj, wxWindow);
        if (!win) return nullptr;
        GUIMyFrame1 *frame = dynamic_cast<GUIMyFrame1 *>(win->GetParent());
        if (!frame || win != frame->WxPanel) return nullptr;
        return frame;
    }

public:
    int FilterEvent(wxEvent &event) override
    {
        const wxEventType t = event.GetEventType();

        if (t == wxEVT_PAINT)
        {
            GUIMyFrame1 *frame = FrameOf(event.GetEventObject());
            if (!frame) return Event_Skip;
            wxBufferedPaintDC dc(frame->WxPanel);
            int w, h;
            frame->WxPanel->GetSize(&w, &h);
            frame->DrawScene(&dc, w, h);
            last = TakeSnapshot(frame);
            dirty = false;
            return Event_Processed;
        }

        if (t == wxEVT_BUTTON)
        {
            // geometry is (re)loaded after this event - repaint on next update
            dirty = true;
            return Event_Skip;
        }

        if (t == wxEVT_UPDATE_UI)
        {
            // template's Repaint() would draw via wxClientDC (invisible on macOS);
            // instead request a real repaint whenever something changed
            GUIMyFrame1 *frame = FrameOf(event.GetEventObject());
            if (!frame) return Event_Skip;
            if (dirty || TakeSnapshot(frame) != last) frame->WxPanel->Refresh(false);
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

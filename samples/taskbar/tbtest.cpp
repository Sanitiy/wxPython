/////////////////////////////////////////////////////////////////////////////
// Name:        tbtest.cpp
// Purpose:     wxTaskBarIcon demo
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c)
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

// the application icon (under Windows and OS/2 it is in resources)
#if defined(__WXGTK__) || defined(__WXMOTIF__) || defined(__WXMAC__) || defined(__WXMGL__) || defined(__WXX11__)
    #include "../sample.xpm"
#endif

#include "smile.xpm"

#include "wx/taskbar.h"

#include "tbtest.h"

// ----------------------------------------------------------------------------
// global variables
// ----------------------------------------------------------------------------

static MyDialog *gs_dialog = NULL;

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// MyApp
// ----------------------------------------------------------------------------

IMPLEMENT_APP(MyApp)

bool MyApp::OnInit()
{
    if ( !wxApp::OnInit() )
        return false;

    if ( !wxTaskBarIcon::IsAvailable() )
    {
        wxMessageBox
        (
            "There appears to be no system tray support in your current environment. This sample may not behave as expected.",
            "Warning",
            wxOK | wxICON_EXCLAMATION
        );
    }

    // Create the main window
    gs_dialog = new MyDialog(wxT("wxTaskBarIcon Test Dialog"));

    gs_dialog->Show(true);

    return true;
}


// ----------------------------------------------------------------------------
// MyDialog implementation
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(MyDialog, wxDialog)
    EVT_BUTTON(wxID_ABOUT, MyDialog::OnAbout)
    EVT_BUTTON(wxID_OK, MyDialog::OnOK)
    EVT_BUTTON(wxID_EXIT, MyDialog::OnExit)
    EVT_CLOSE(MyDialog::OnCloseWindow)
END_EVENT_TABLE()


MyDialog::MyDialog(const wxString& title)
        : wxDialog(NULL, wxID_ANY, title)
{
    wxSizer * const sizerTop = new wxBoxSizer(wxVERTICAL);

    wxSizerFlags flags;
    flags.Border(wxALL, 10);

    sizerTop->Add(new wxStaticText
                      (
                        this,
                        wxID_ANY,
                        _T("Press 'Hide me' to hide this window, Exit to quit.")
                      ), flags);

    sizerTop->Add(new wxStaticText
                      (
                        this,
                        wxID_ANY,
                        _T("Double-click on the taskbar icon to show me again.")
                      ), flags);

    sizerTop->AddStretchSpacer()->SetMinSize(200, 50);

    wxSizer * const sizerBtns = new wxBoxSizer(wxHORIZONTAL);
    sizerBtns->Add(new wxButton(this, wxID_ABOUT, _T("&About")), flags);
    sizerBtns->Add(new wxButton(this, wxID_OK, _T("&Hide")), flags);
    sizerBtns->Add(new wxButton(this, wxID_EXIT, _T("E&xit")), flags);

    sizerTop->Add(sizerBtns, flags.Align(wxALIGN_CENTER_HORIZONTAL));
    SetSizerAndFit(sizerTop);
    Centre();

    m_taskBarIcon = new MyTaskBarIcon();
#if defined(__WXCOCOA__)
    m_dockIcon = new MyTaskBarIcon(wxTaskBarIcon::DOCK);
#endif

    // we should be able to show up to 128 characters on recent Windows versions
    // (and 64 on Win9x)
    if ( !m_taskBarIcon->SetIcon(wxICON(sample),
                                 "wxTaskBarIcon Sample\n"
                                 "With a very, very, very, very\n"
                                 "long tooltip whose length is\n"
                                 "greater than 64 characters.") )
    {
        wxLogError(wxT("Could not set icon."));
    }
}

MyDialog::~MyDialog()
{
    delete m_taskBarIcon;
#if defined(__WXCOCOA__)
    delete m_dockIcon;
#endif
}

void MyDialog::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    static const char * const title = "About wxWidgets Taskbar Sample";
    static const char * const message
        = "wxWidgets sample showing wxTaskBarIcon class\n"
          "\n"
          "(C) 1997 Julian Smart\n"
          "(C) 2007 Vadim Zeitlin";

#if defined(__WXMSW__) && wxUSE_TASKBARICON_BALLOONS
    m_taskBarIcon->ShowBalloon(title, message, 15000, wxICON_INFORMATION);
#else // !__WXMSW__
    wxMessageBox(message, title, wxICON_INFORMATION|wxOK, this);
#endif // __WXMSW__/!__WXMSW__
}

void MyDialog::OnOK(wxCommandEvent& WXUNUSED(event))
{
    Show(false);
}

void MyDialog::OnExit(wxCommandEvent& WXUNUSED(event))
{
    Close(true);
}

void MyDialog::OnCloseWindow(wxCloseEvent& WXUNUSED(event))
{
    Destroy();
}


// ----------------------------------------------------------------------------
// MyTaskBarIcon implementation
// ----------------------------------------------------------------------------

enum
{
    PU_RESTORE = 10001,
    PU_NEW_ICON,
    PU_EXIT,
    PU_CHECKMARK,
    PU_SUB1,
    PU_SUB2,
    PU_SUBMAIN
};


BEGIN_EVENT_TABLE(MyTaskBarIcon, wxTaskBarIcon)
    EVT_MENU(PU_RESTORE, MyTaskBarIcon::OnMenuRestore)
    EVT_MENU(PU_EXIT,    MyTaskBarIcon::OnMenuExit)
    EVT_MENU(PU_NEW_ICON,MyTaskBarIcon::OnMenuSetNewIcon)
    EVT_MENU(PU_CHECKMARK,MyTaskBarIcon::OnMenuCheckmark)
    EVT_UPDATE_UI(PU_CHECKMARK,MyTaskBarIcon::OnMenuUICheckmark)
    EVT_TASKBAR_LEFT_DCLICK  (MyTaskBarIcon::OnLeftButtonDClick)
    EVT_MENU(PU_SUB1, MyTaskBarIcon::OnMenuSub)
    EVT_MENU(PU_SUB2, MyTaskBarIcon::OnMenuSub)
END_EVENT_TABLE()

void MyTaskBarIcon::OnMenuRestore(wxCommandEvent& )
{
    gs_dialog->Show(true);
}

void MyTaskBarIcon::OnMenuExit(wxCommandEvent& )
{
    gs_dialog->Close(true);
}

static bool check = true;

void MyTaskBarIcon::OnMenuCheckmark(wxCommandEvent& )
{
    check = !check;
}

void MyTaskBarIcon::OnMenuUICheckmark(wxUpdateUIEvent &event)
{
    event.Check(check);
}

void MyTaskBarIcon::OnMenuSetNewIcon(wxCommandEvent&)
{
    wxIcon icon(smile_xpm);

    if (!SetIcon(icon, wxT("wxTaskBarIcon Sample - a different icon")))
        wxMessageBox(wxT("Could not set new icon."));
}

void MyTaskBarIcon::OnMenuSub(wxCommandEvent&)
{
    wxMessageBox(wxT("You clicked on a submenu!"));
}

// Overridables
wxMenu *MyTaskBarIcon::CreatePopupMenu()
{
    wxMenu *menu = new wxMenu;
    menu->Append(PU_RESTORE, _T("&Restore main window"));
    menu->AppendSeparator();
    menu->Append(PU_NEW_ICON, _T("&Set New Icon"));
    menu->AppendSeparator();
    menu->AppendCheckItem(PU_CHECKMARK, _T("Test &check mark"));
    menu->AppendSeparator();
    wxMenu *submenu = new wxMenu;
    submenu->Append(PU_SUB1, _T("One submenu"));
    submenu->AppendSeparator();
    submenu->Append(PU_SUB2, _T("Another submenu"));
    menu->Append(PU_SUBMAIN, _T("Submenu"), submenu);
#ifndef __WXMAC_OSX__ /*Mac has built-in quit menu*/
    menu->AppendSeparator();
    menu->Append(PU_EXIT,    _T("E&xit"));
#endif
    return menu;
}

void MyTaskBarIcon::OnLeftButtonDClick(wxTaskBarIconEvent&)
{
    gs_dialog->Show(true);
}

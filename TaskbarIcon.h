#ifndef TASKBARICON_H_INCLUDED
#define TASKBARICON_H_INCLUDED

#include <wx/wx.h>
#include <wx/taskbar.h>
#include "SomeVKMusicMain.h"

class SomeVKMusicTaskbarIcon : public wxTaskBarIcon{
private:
    SomeVKMusicFrame *m_main_frame = nullptr;

private:
    wxMenuItem  *m_exit_menu,
                *m_play_pause,
                *m_stop,
                *m_next,
                *m_prev;

    enum MenuID{
        ID_PLAY_PAUSE = 600,
        ID_STOP,
        ID_NEXT,
        ID_PREV,
        ID_EXIT
    };

public:
    SomeVKMusicTaskbarIcon(SomeVKMusicFrame *frame) : m_main_frame(frame) {
        Connect(wxEVT_TASKBAR_LEFT_DCLICK, wxCommandEventHandler(SomeVKMusicTaskbarIcon::OnLeftButtonDClick));

        Connect(ID_EXIT, wxEVT_MENU, wxMenuEventHandler(SomeVKMusicTaskbarIcon::OnExitMenu));
        Connect(ID_PLAY_PAUSE, wxEVT_MENU, wxMenuEventHandler(SomeVKMusicTaskbarIcon::OnPlayPause));
        Connect(ID_STOP, wxEVT_MENU, wxMenuEventHandler(SomeVKMusicTaskbarIcon::OnStop));
        Connect(ID_NEXT, wxEVT_MENU, wxMenuEventHandler(SomeVKMusicTaskbarIcon::OnNext));
        Connect(ID_PREV, wxEVT_MENU, wxMenuEventHandler(SomeVKMusicTaskbarIcon::OnPrev));
    }

    virtual wxMenu *CreatePopupMenu(){
        wxMenu *popup_menu = new wxMenu();
        m_play_pause = new wxMenuItem(popup_menu, ID_PLAY_PAUSE, "Играть", wxEmptyString, false);
        m_stop = new wxMenuItem(popup_menu, ID_STOP, "Стоп", wxEmptyString, false);
        m_next = new wxMenuItem(popup_menu, ID_NEXT, "Следующая", wxEmptyString, false);
        m_prev = new wxMenuItem(popup_menu, ID_PREV, "Предыдущая", wxEmptyString, false);

        m_exit_menu = new wxMenuItem(popup_menu, ID_EXIT, "Выключить", wxEmptyString, false);

        popup_menu->Append(m_play_pause);
        popup_menu->Append(m_stop);
        popup_menu->Append(m_next);
        popup_menu->Append(m_prev);

        popup_menu->Append(new wxMenuItem());

        popup_menu->Append(m_exit_menu);

        return popup_menu;
    }

private:
    void OnLeftButtonDClick(wxCommandEvent &event){
        m_main_frame->Restore();
    }
    void OnExitMenu(wxMenuEvent &event){
        m_main_frame->Close(true);
    }

    void OnPlayPause(wxMenuEvent &event){
        if (m_main_frame->GetState() == wxMEDIASTATE_PLAYING)
            m_play_pause->SetItemLabel("Играть");
        else m_play_pause->SetItemLabel("Пауза");

        wxCommandEvent new_event(wxEVT_BUTTON, SomeVKMusicFrame::ID_PLAY_PAUSE);
        wxPostEvent(m_main_frame, new_event);
    }
    void OnStop(wxMenuEvent &event){
        wxCommandEvent new_event(wxEVT_BUTTON, SomeVKMusicFrame::ID_STOP);
        wxPostEvent(m_main_frame, new_event);
    }
    void OnNext(wxMenuEvent &event){
        wxCommandEvent new_event(wxEVT_BUTTON, SomeVKMusicFrame::ID_NEXT);
        wxPostEvent(m_main_frame, new_event);
    }
    void OnPrev(wxMenuEvent &event){
        wxCommandEvent new_event(wxEVT_BUTTON, SomeVKMusicFrame::ID_PREV);
        wxPostEvent(m_main_frame, new_event);
    }
};


#endif // TASKBARICON_H_INCLUDED

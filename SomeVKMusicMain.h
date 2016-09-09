#ifndef SOMEVKMUSICMAIN_H
#define SOMEVKMUSICMAIN_H

#include <wx/wx.h>
#include <wx/aui/aui.h>

#include "SomeVKMusicApp.h"
#include "VKConnection.h"

class SomeVKMusicFrame: public wxFrame{
public:
    SomeVKMusicFrame();
    virtual ~SomeVKMusicFrame();

private:
    void CreateControls();
    void InitLogger();
    void CreateConnection();

    static void AcceptPlaylist(const wxString &response);

private:
    VKConnection *m_connection;

//GUI Elements
private:
    wxAuiManager     m_aui_manager;

    wxStaticText    *m_current_song_name,
                    *m_song_duration_full,
                    *m_song_duration_current;
    wxSlider        *m_current_song_progress;

    wxButton        *m_prev,
                    *m_play_pause,
                    *m_stop,
                    *m_next;

private:
    enum GUIElementsID{
        ID_BUTTON_PREV = 100,
        ID_BUTTON_PLAY_PAUSE,
        ID_BUTTON_STOP,
        ID_BUTTON_NEXT
    };
};


#endif // SOMEVKMUSICMAIN_H

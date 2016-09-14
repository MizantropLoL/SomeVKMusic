#ifndef SOMEVKMUSICMAIN_H
#define SOMEVKMUSICMAIN_H

#include <wx/wx.h>
#include <wx/aui/aui.h>
#include <wx/mediactrl.h>
#include <wx/tglbtn.h>
#include <wx/dir.h>
#include <wx/taskbar.h>

#include "SomeVKMusicApp.h"
#include "VKConnection.h"
#include "VKPlaylistFrame.h"
#include "FileSavingThread.h"

class SomeVKMusicFrame: public wxFrame{
public:
    SomeVKMusicFrame();
    virtual ~SomeVKMusicFrame();

private:
    void CreateControls();
    void CreateMenu();
    void InitLogger();
    void CreateConnection();
    void SetTimers();
    void SetEventHandlers();
    void LoadPlaylist();

private:
    static void AcceptPlaylist(const wxString &response, void **playlist);

    void PlayCurrent(bool from_pause);
    void PlaySelected();
    void PauseCurrent();
    void PlayNext();
    void PlayPrev();
    void PlayRand();
    void StopCurrent();

    void SetCurrentAudioInfo();
    void UpdateForm();
    void ScrollAudio(int to);
    void ChangeVolume(int new_val);
    void TryToStart();
    void CheckPlaying();
    void ChangePlaylistToUsers();
    void ChangePlaylistToSearch();
    void ChangePlaylistToRecommendAll();
    void ChangePlaylistToRecommendSelected();
    void SaveCurrentAudio();

public:
    wxMediaState GetState(){
        return m_player->GetState();
    }

//Event handlers
private:
    void OnPlayPause(wxCommandEvent &event);
    void OnStop(wxCommandEvent &event);
    void OnNext(wxCommandEvent &event);
    void OnPrev(wxCommandEvent &event);
    void OnNextPlayTimer(wxTimerEvent &event);
    void OnAudioFinished(wxTimerEvent &event);
    void OnFormUpdate(wxTimerEvent &event);
    void OnAudioScroll(wxScrollEvent &event);
    void OnVolumeChanged(wxScrollEvent &event);
    void OnShowPlaylist(wxMenuEvent &event);
    void OnPaneClose(wxAuiManagerEvent &event);
    void OnChangePlaylist(wxMenuEvent &event);
    void OnHide(wxIconizeEvent &event){
        if (event.Iconized())
            Hide();
        else Raise();
    }
    void OnSetSavingPath(wxMenuEvent &event);

private:
    VKConnection        *m_connection;
    VKPlaylistFrame     *m_playlist_frame;
    VKPlaylist          *m_playlist;
    wxMediaCtrl         *m_player;
    wxTimer             *m_next_play_timer,
                        *m_duration_timer,
                        *m_form_update_timer;
    wxString             m_save_files_path = "Saved/";
    FileSavingThread    *m_saving_thread = nullptr;

//GUI Elements
private:
    wxAuiManager     m_aui_manager;

    wxStaticText    *m_current_song_name,
                    *m_song_duration_full,
                    *m_song_duration_current;
    wxSlider        *m_current_song_progress,
                    *m_volume;

    wxButton        *m_prev,
                    *m_play_pause,
                    *m_stop,
                    *m_next;
    wxToggleButton  *m_repeat,
                    *m_random;
    wxMenuItem      *m_menu_show_playlist,
                    *m_menu_save_audio,
                    *m_menu_playlist_search,
                    *m_menu_playlist_my,
                    *m_menu_playlist_recommend_all,
                    *m_menu_playlist_recommend_selected,
                    *m_menu_saving_path;
    wxTaskBarIcon   *m_taskbar;

public:
    enum GUIElementsID{
        ID_PLAYLIST_MY = 500,
        ID_PLAYLIST_SEARCH,
        ID_PLAYLIST_RECOMMEND_ALL,
        ID_PLAYLIST_RECOMMEND_SELECTED,

        ID_PLAYER,

        ID_NEXT,
        ID_PLAY_PAUSE,
        ID_STOP,
        ID_PREV
    };
};

namespace{
    const wxString SAVE_TO_PATH_KEY = "Saving path";
    const wxString DEFAULT_SAVE_TO_PATH = "Saved/";

    const wxString DEFAULT_PLAYLIST_KEY = "Default playlist";
    const int DEFAULT_PLAYLIST = SomeVKMusicFrame::ID_PLAYLIST_MY;

    const wxString IS_SAVING_AUDIO_KEY = "Is saving audio";
}

#endif // SOMEVKMUSICMAIN_H

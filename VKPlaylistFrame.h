#ifndef VKPLAYLISTFRAME_H_INCLUDED
#define VKPLAYLISTFRAME_H_INCLUDED

#include <wx/grid.h>
#include "VKResponse.h"

class VKPlaylistFrame : public wxPanel{
private:
    VKPlaylist *m_current_playlist;
    bool m_owner;
    bool m_audio_changed = false;

private:
    wxGrid      *m_playlist_grid;

public:
    VKPlaylistFrame(wxWindow* parent) : wxPanel(parent){
        m_current_playlist = nullptr;

        InitFrame();
    }
    VKPlaylistFrame(wxWindow *parent, VKPlaylist *playlist, bool move = true) : wxPanel(parent){
        m_owner = move;
        m_current_playlist = playlist;

        InitFrame();
    }
    virtual ~VKPlaylistFrame(){
        if (m_owner)
            delete m_current_playlist;
    }

public:
    const VKPlaylist *SetPlaylist(VKPlaylist *playlist, bool move = true){
        VKPlaylist *to_return = m_current_playlist;

        m_owner = move;
        m_current_playlist = playlist;

        UpdatePlaylist();

        return to_return;
    }
    const VKPlaylist *GetPlaylist() const{
        return m_current_playlist;
    }

    bool IsChanged(){
        if (!m_audio_changed) return m_audio_changed;
        else{
            m_audio_changed = false;
            return true;
        }
    }

    void UpdateStatus(int old_audio, int new_audio){
        m_playlist_grid->SetCellBackgroundColour(old_audio, 0, wxColour(255, 255, 255));
        m_playlist_grid->SetCellBackgroundColour(old_audio, 1, wxColour(255, 255, 255));
        m_playlist_grid->SetCellBackgroundColour(old_audio, 2, wxColour(255, 255, 255));

        m_playlist_grid->SetCellBackgroundColour(new_audio, 0, wxColour(0, 200, 200));
        m_playlist_grid->SetCellBackgroundColour(new_audio, 1, wxColour(0, 200, 200));
        m_playlist_grid->SetCellBackgroundColour(new_audio, 2, wxColour(0, 200, 200));

        m_playlist_grid->Refresh();
    }

private:
    void InitFrame(){
        CreateControls();
        SetEventsHandlers();

        if (m_current_playlist) UpdatePlaylist();
    }

    void CreateControls(){
        wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
        SetSizer(sizer);

        m_playlist_grid = new wxGrid(this, wxNewId());
        m_playlist_grid->CreateGrid(0, 3, wxGrid::wxGridSelectRows);

        m_playlist_grid->SetRowLabelSize(0);
        m_playlist_grid->EnableGridLines(false);

        m_playlist_grid->SetColLabelValue(0, "Автор");
        m_playlist_grid->SetColumnWidth(0, 190);
        m_playlist_grid->SetColLabelValue(1, "Композиция");
        m_playlist_grid->SetColumnWidth(1, 350);
        m_playlist_grid->SetColLabelValue(2, "Продолжительность");
        m_playlist_grid->SetColumnWidth(2, 140);

        sizer->Add(m_playlist_grid, 1, wxEXPAND);
    }
    void SetEventsHandlers(){
        Connect(m_playlist_grid->GetId(), wxEVT_GRID_CELL_LEFT_DCLICK, wxGridEventHandler(VKPlaylistFrame::OnAudioSelect));
    }
    void UpdatePlaylist(){
        if (m_current_playlist == nullptr) return;

        if (m_playlist_grid->GetRows() > 0)
            m_playlist_grid->DeleteRows(0, m_playlist_grid->GetRows());

        m_playlist_grid->AppendRows(m_current_playlist->Size());
        m_playlist_grid->SetEditable(false);

        for (size_t i = 0; i < m_current_playlist->Size(); i++){
            m_playlist_grid->SetCellValue(i, 0, m_current_playlist->GetSong(i)->Artist);
            m_playlist_grid->SetCellValue(i, 1, m_current_playlist->GetSong(i)->Title);
            m_playlist_grid->SetCellValue(i, 2, m_current_playlist->GetSong(i)->DurationString());
            m_playlist_grid->SetCellAlignment(i, 2, wxALIGN_RIGHT, wxALIGN_CENTER);
        }
    }

private:
    void OnAudioSelect(wxGridEvent &event){
        UpdateStatus(m_current_playlist->GetCurrentNo(), event.GetRow());

        m_current_playlist->SetCurrent(event.GetRow());

        m_audio_changed = true;
    }
};

#endif // VKPLAYLISTFRAME_H_INCLUDED

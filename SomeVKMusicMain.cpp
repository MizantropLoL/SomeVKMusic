#include <wx/sizer.h>
#include <wx/log.h>
#include <wx/config.h>

#include <fstream>

#include "TaskbarIcon.h"
#include "SomeVKMusicMain.h"
#include "GUIDef.h"
#include "VKPlaylist.h"

SomeVKMusicFrame::SomeVKMusicFrame()
    : wxFrame(nullptr, wxNewId(), "SomeVKMusic", wxDefaultPosition,
              wxSize(500, 120), wxMINIMIZE_BOX|wxDEFAULT_DIALOG_STYLE){
    InitLogger();
    CreateConnection();
    CreateControls();
    CreateMenu();
    SetTimers();
    SetEventHandlers();

    m_form_update_timer->Start(1000);

    wxConfigBase::Get()->SetPath("/Common");
    bool save_audio = wxConfigBase::Get()->ReadBool(IS_SAVING_AUDIO_KEY, false);
    m_menu_save_audio->Check(save_audio);

    LoadPlaylist();
}

SomeVKMusicFrame::~SomeVKMusicFrame(){
    delete m_connection;
    delete m_taskbar;

    wxConfigBase::Get()->Write(IS_SAVING_AUDIO_KEY, m_menu_save_audio->IsChecked());

    m_aui_manager.UnInit();
}

void SomeVKMusicFrame::InitLogger(){
    std::ofstream *log_stream = new std::ofstream("Log.txt", std::ios::app);

    delete wxLog::SetActiveTarget(new wxLogStream(log_stream));
}

void SomeVKMusicFrame::CreateConnection(){
    try{
        m_connection = new VKConnection();
    }catch(wxString &error){
        wxLogError(error);
    }
}

void SomeVKMusicFrame::AcceptPlaylist(const wxString &response, void **playlist){
    *playlist = VKResponse::GetVKPlaylist(response);
}

void SomeVKMusicFrame::SetTimers(){
    m_next_play_timer = new wxTimer(this, wxNewId());
    m_duration_timer = new wxTimer(this, wxNewId());
    m_form_update_timer = new wxTimer(this, wxNewId());
}

void SomeVKMusicFrame::SetEventHandlers(){
    Connect(m_next->GetId(), wxEVT_BUTTON, (wxObjectEventFunction)&SomeVKMusicFrame::OnNext);
    Connect(m_play_pause->GetId(), wxEVT_BUTTON, (wxObjectEventFunction)&SomeVKMusicFrame::OnPlayPause);
    Connect(m_stop->GetId(), wxEVT_BUTTON, (wxObjectEventFunction)&SomeVKMusicFrame::OnStop);
    Connect(m_prev->GetId(), wxEVT_BUTTON, (wxObjectEventFunction)&SomeVKMusicFrame::OnPrev);

    Connect(m_next_play_timer->GetId(), wxEVT_TIMER, wxTimerEventHandler(SomeVKMusicFrame::OnNextPlayTimer));
    Connect(m_duration_timer->GetId(), wxEVT_TIMER, wxTimerEventHandler(SomeVKMusicFrame::OnAudioFinished));
    Connect(m_form_update_timer->GetId(), wxEVT_TIMER, wxTimerEventHandler(SomeVKMusicFrame::OnFormUpdate));

    Connect(m_current_song_progress->GetId(), wxEVT_SCROLL_CHANGED, wxScrollEventHandler(SomeVKMusicFrame::OnAudioScroll));
    Connect(m_volume->GetId(), wxEVT_SCROLL_CHANGED, wxScrollEventHandler(SomeVKMusicFrame::OnVolumeChanged));

    Connect(m_menu_show_playlist->GetId(), wxEVT_MENU, wxMenuEventHandler(SomeVKMusicFrame::OnShowPlaylist));
    Connect(m_menu_saving_path->GetId(), wxEVT_MENU, wxMenuEventHandler(SomeVKMusicFrame::OnSetSavingPath));

    Connect(wxEVT_AUI_PANE_CLOSE, wxAuiManagerEventHandler(SomeVKMusicFrame::OnPaneClose));

    Connect(ID_PLAYLIST_MY, ID_PLAYLIST_RECOMMEND_SELECTED, wxEVT_MENU, wxMenuEventHandler(SomeVKMusicFrame::OnChangePlaylist));

    Connect(this->GetId(), wxEVT_ICONIZE, wxIconizeEventHandler(SomeVKMusicFrame::OnHide));
}

void SomeVKMusicFrame::LoadPlaylist(){
    int playlist_type;
    wxConfigBase::Get()->Read(DEFAULT_PLAYLIST_KEY, &playlist_type, DEFAULT_PLAYLIST);
    wxMenuEvent event(wxEVT_MENU, playlist_type);
    wxPostEvent(this, event);
}

//----------------------------------------
void SomeVKMusicFrame::PlayCurrent(bool from_pause){
    m_play_pause->SetLabel("||");
    if (!m_player->Play()) wxLogError("Can't play sound");

    m_duration_timer->Stop();
    if (!from_pause){
        SetCurrentAudioInfo();
        if (m_menu_save_audio->IsChecked())
            SaveCurrentAudio();
    }

    m_duration_timer->StartOnce((m_current_song_progress->GetMax() -
                                 m_current_song_progress->GetValue()) * 1000);

    CheckPlaying();
}

void SomeVKMusicFrame::PlaySelected(){
    if (!m_player->Load(m_playlist->GetCurrent()->URI)) wxLogError("Can't load sound");

    TryToStart();
}

void SomeVKMusicFrame::PauseCurrent(){
    m_duration_timer->Stop();
    m_play_pause->SetLabel(">");
    if (!m_player->Pause()) wxLogError("Can't pause sound");
}

void SomeVKMusicFrame::PlayNext(){
    int current = m_playlist->GetCurrentNo();
    if (!m_player->Load(m_playlist->GetNext()->URI)) wxLogError("Can't load sound");
    m_playlist_frame->UpdateStatus(current, m_playlist->GetCurrentNo());

    TryToStart();
}

void SomeVKMusicFrame::PlayPrev(){
    int current = m_playlist->GetCurrentNo();
    if (!m_player->Load(m_playlist->GetPrev()->URI)) wxLogError("Can't load sound");
    m_playlist_frame->UpdateStatus(current, m_playlist->GetCurrentNo());

    TryToStart();
}

void SomeVKMusicFrame::PlayRand(){
    int current = m_playlist->GetCurrentNo();
    if (m_player->Load(m_playlist->GetRandom()->URI)) wxLogError("Can't load sound");
    m_playlist_frame->UpdateStatus(current, m_playlist->GetCurrentNo());

    TryToStart();
}

void SomeVKMusicFrame::StopCurrent(){
    m_play_pause->SetLabel(">");
    if (!m_player->Stop()) wxLogError("Can't stop sound");

    m_duration_timer->Stop();
}

void SomeVKMusicFrame::TryToStart(){
    m_next_play_timer->Stop();
    m_next_play_timer->StartOnce(500);
}

void SomeVKMusicFrame::SetCurrentAudioInfo(){
    const VKSong *current_song = m_playlist->GetCurrent();
    m_current_song_name->SetLabel(current_song->Artist + " - " + current_song->Title);

    m_song_duration_current->SetLabel("00:00");
    m_song_duration_full->SetLabel(current_song->DurationString());

    m_current_song_progress->SetMax(current_song->DurationFull());
    m_current_song_progress->SetValue(0);
}

void SomeVKMusicFrame::UpdateForm(){
    long duration = (m_player->Tell() < 0) ? 0 : m_player->Tell() / 1000;

    m_current_song_progress->SetValue(duration);
    m_song_duration_current->SetLabel(VKSong::DurationToString(duration));

    if (m_playlist_frame->IsChanged()) PlaySelected();

    if (m_playlist_frame->GetPlaylist() == nullptr && m_playlist != nullptr){
        m_playlist_frame->SetPlaylist(m_playlist, false);
        m_player->Load(m_playlist->GetCurrent()->URI);
    }
}

void SomeVKMusicFrame::ScrollAudio(int to){
    m_player->Seek(to * 1000);
    m_duration_timer->Stop();
    m_duration_timer->StartOnce((m_current_song_progress->GetMax() -
                                m_current_song_progress->GetValue()) * 1000);
}

void SomeVKMusicFrame::ChangeVolume(int new_val){
    m_player->SetVolume(new_val / 100.0);
}

void SomeVKMusicFrame::CheckPlaying(){
    if (m_player->GetState() != wxMEDIASTATE_PLAYING)
        TryToStart();
}

void SomeVKMusicFrame::ChangePlaylistToUsers(){
    m_playlist = nullptr;
    delete (m_playlist_frame->SetPlaylist(nullptr));

    m_connection->MakeQuery(new VKQueryPlaylistGet(m_connection->GetUserID()));
    m_connection->ConnectResponseGetter(&SomeVKMusicFrame::AcceptPlaylist, (void **)(&m_playlist));
}

void SomeVKMusicFrame::ChangePlaylistToSearch(){
    wxString q = wxGetTextFromUser("Введите запрос для поиска", "Поиск");
    if (q.IsEmpty()) return;

    m_playlist = nullptr;
    delete m_playlist_frame->SetPlaylist(nullptr);

    m_connection->MakeQuery(new VKQueryPlaylistSearch(q));
    m_connection->ConnectResponseGetter(&SomeVKMusicFrame::AcceptPlaylist, (void **)(&m_playlist));
}

void SomeVKMusicFrame::ChangePlaylistToRecommendAll(){
    m_playlist = nullptr;
    delete m_playlist_frame->SetPlaylist(nullptr);

    m_connection->MakeQuery(new VKQueryPlaylistRecommend(m_connection->GetUserID()));
    m_connection->ConnectResponseGetter(&SomeVKMusicFrame::AcceptPlaylist, (void **)(&m_playlist));
}

void SomeVKMusicFrame::ChangePlaylistToRecommendSelected(){
    if (m_playlist == nullptr) return;

    m_connection->MakeQuery(new VKQueryPlaylistRecommend(m_connection->GetUserID(), m_playlist->GetCurrent()->ID));

    m_playlist = nullptr;
    delete m_playlist_frame->SetPlaylist(nullptr);

    m_connection->ConnectResponseGetter(&SomeVKMusicFrame::AcceptPlaylist, (void **)(&m_playlist));
}

void SomeVKMusicFrame::SaveCurrentAudio(){
    const VKSong *current_song = m_playlist->GetCurrent();
    wxString saving_path;
    saving_path = wxConfigBase::Get()->Read(SAVE_TO_PATH_KEY, DEFAULT_SAVE_TO_PATH)
                + current_song->Artist + " - "
                + current_song->Title + ".mp3";

    if (m_saving_thread) {
        m_saving_thread->Kill();
        m_saving_thread = nullptr;
    }
    m_saving_thread = new FileSavingThread(current_song->URI.BuildURI(),
                                                           saving_path);
    m_saving_thread->Run();
}

//------------------------------------------------

//*********************************************
void SomeVKMusicFrame::OnPlayPause(wxCommandEvent &event){
    switch(m_player->GetState()){
        case wxMEDIASTATE_PLAYING: PauseCurrent(); break;
        case wxMEDIASTATE_PAUSED: PlayCurrent(true); break;
        case wxMEDIASTATE_STOPPED: PlayCurrent(false); break;
    }
}

void SomeVKMusicFrame::OnNext(wxCommandEvent &event){
    if (m_repeat->GetValue()) PlaySelected();
    else if (m_random->GetValue()) PlayRand();
    else PlayNext();
}

void SomeVKMusicFrame::OnPrev(wxCommandEvent &event){
    if (m_repeat->GetValue()) PlaySelected();
    else if (m_random->GetValue()) PlayRand();
    else PlayPrev();
}
void SomeVKMusicFrame::OnStop(wxCommandEvent &event){
    StopCurrent();
}
void SomeVKMusicFrame::OnNextPlayTimer(wxTimerEvent &event){
    PlayCurrent(false);
}
void SomeVKMusicFrame::OnAudioFinished(wxTimerEvent &event){
    if (m_repeat->GetValue()) PlaySelected();
    else if (m_random->GetValue()) PlayRand();
    else PlayNext();
}
void SomeVKMusicFrame::OnFormUpdate(wxTimerEvent &event){
    UpdateForm();
}
void SomeVKMusicFrame::OnAudioScroll(wxScrollEvent &event){
    wxLogMessage("Scroll event");
    ScrollAudio(event.GetPosition());
}
void SomeVKMusicFrame::OnVolumeChanged(wxScrollEvent &event){
    ChangeVolume(event.GetPosition());
}
void SomeVKMusicFrame::OnShowPlaylist(wxMenuEvent &event){
    m_aui_manager.GetPane(m_playlist_frame).Show();
    m_aui_manager.Update();
    m_menu_show_playlist->Enable(false);
    wxConfigBase::Get()->Write(IS_SAVING_AUDIO_KEY, false);
}
void SomeVKMusicFrame::OnPaneClose(wxAuiManagerEvent &event){
    m_menu_show_playlist->Enable(true);
    wxConfigBase::Get()->Write(IS_SAVING_AUDIO_KEY, true);
}
void SomeVKMusicFrame::OnChangePlaylist(wxMenuEvent &event){
    bool my, recommend_all, recommend_selected, search;
    my = recommend_all = recommend_selected = search = false;
    switch (event.GetId()){
        case ID_PLAYLIST_MY:
            my = true;
            ChangePlaylistToUsers();
            break;
        case ID_PLAYLIST_SEARCH:
            search = true;
            ChangePlaylistToSearch();
            break;
        case ID_PLAYLIST_RECOMMEND_ALL:
            recommend_all = true;
            ChangePlaylistToRecommendAll();
            break;
        case ID_PLAYLIST_RECOMMEND_SELECTED:
            recommend_selected = true;
            ChangePlaylistToRecommendSelected();
            break;
    }
    wxConfigBase::Get()->Write(DEFAULT_PLAYLIST_KEY, event.GetId());

    m_menu_playlist_my->Check(my);
    m_menu_playlist_search->Check(search);
    m_menu_playlist_recommend_all->Check(recommend_all);
    m_menu_playlist_recommend_selected->Check(recommend_selected);
}
void SomeVKMusicFrame::OnSetSavingPath(wxMenuEvent &event){
    wxDirDialog dlg(nullptr, "Выберите каталог для сохранения аудио");
    if (dlg.ShowModal() == wxID_OK)
        wxConfigBase::Get()->Write(SAVE_TO_PATH_KEY, dlg.GetPath() + '/');
}
//*********************************************

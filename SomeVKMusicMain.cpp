#include <wx/sizer.h>
#include <wx/log.h>
#include <wx/config.h>

#include <fstream>

#include "TaskbarIcon.h"
#include "SomeVKMusicMain.h"
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

void SomeVKMusicFrame::CreateControls(){
    wxPanel *main_panel;
    wxBoxSizer  *main_controls_sizer,
                *info_nav_sizer,
                *progress_sizer,
                *song_info_sizer,
                *controls_sizer,
                *additional_funcs_sizer;

    m_aui_manager.SetManagedWindow(this);

    main_panel = new wxPanel(this);
    m_player = new wxMediaCtrl(main_panel, ID_PLAYER);
    m_player->Hide();

    main_controls_sizer = new wxBoxSizer(wxHORIZONTAL);
    main_panel->SetSizer(main_controls_sizer);

    //Creating song progress info
    progress_sizer = new wxBoxSizer(wxHORIZONTAL);

    m_song_duration_current = new wxStaticText(main_panel, wxID_ANY, "00:00");
    m_song_duration_full = new wxStaticText(main_panel, wxID_ANY, "00:00");
    m_current_song_progress = new wxSlider(main_panel, wxID_ANY, 0, 0, 1);

    progress_sizer->Add(m_song_duration_current, 0, wxEXPAND|wxALIGN_LEFT);
    progress_sizer->Add(m_current_song_progress, 1, wxEXPAND|wxALIGN_CENTER_HORIZONTAL);
    progress_sizer->Add(m_song_duration_full, 0, wxEXPAND|wxALIGN_RIGHT);

    //Song info
    song_info_sizer = new wxBoxSizer(wxVERTICAL);
    m_current_song_name = new wxStaticText(main_panel, wxID_ANY, wxEmptyString);

    song_info_sizer->Add(m_current_song_name, 1, wxEXPAND|wxALIGN_CENTER);
    song_info_sizer->Add(progress_sizer, 1, wxEXPAND|wxALIGN_CENTER_HORIZONTAL);

    //Controls (next, stop, play, etc)
    controls_sizer = new wxBoxSizer(wxHORIZONTAL);

    m_prev = new wxButton(main_panel, ID_PREV, "<<");
    m_play_pause = new wxButton(main_panel, ID_PLAY_PAUSE, ">");
    m_stop = new wxButton(main_panel, ID_STOP, "[]");
    m_next = new wxButton(main_panel, ID_NEXT, ">>");

    controls_sizer->Add(m_prev, 1, wxEXPAND);
    controls_sizer->Add(m_play_pause, 1, wxEXPAND);
    controls_sizer->Add(m_stop, 1, wxEXPAND);
    controls_sizer->Add(m_next, 1, wxEXPAND);

    //Contains all above
    info_nav_sizer = new wxBoxSizer(wxVERTICAL);
    info_nav_sizer->Add(song_info_sizer, 1, wxEXPAND);
    info_nav_sizer->Add(controls_sizer, 1, wxEXPAND);

    //Contains volume control, repeat, random buttons
    additional_funcs_sizer = new wxBoxSizer(wxVERTICAL);
    m_volume = new wxSlider(main_panel, wxNewId(), 100, 0, 100);
    m_repeat = new wxToggleButton(main_panel, wxNewId(), "Rep");
    m_random = new wxToggleButton(main_panel, wxNewId(), "Rand");

    additional_funcs_sizer->Add(m_volume, 1, wxEXPAND);
    additional_funcs_sizer->Add(m_repeat, 0, wxEXPAND);
    additional_funcs_sizer->Add(m_random, 0, wxEXPAND);


    main_controls_sizer->Add(info_nav_sizer, 3, wxEXPAND);
    main_controls_sizer->Add(additional_funcs_sizer, 1, wxEXPAND);

    m_playlist_frame = new VKPlaylistFrame(this);

    m_aui_manager.AddPane(m_playlist_frame,
                          wxAuiPaneInfo().Bottom().BestSize(700, 500).Float().Resizable(false).Dockable(false).Hide());
    m_aui_manager.AddPane(main_panel,
                          wxAuiPaneInfo().CenterPane());

    m_aui_manager.Update();


    m_taskbar = new SomeVKMusicTaskbarIcon(this);
    wxIcon icon("icon.ico", wxBITMAP_TYPE_ICO);
    if ( !m_taskbar->SetIcon(icon) )
        wxLogError(wxT("Could not set icon."));
    SetIcon(icon);
}

void SomeVKMusicFrame::CreateMenu(){
    wxMenuBar *main_menu_bar = new wxMenuBar();

    wxMenu *options = new wxMenu();
    wxMenu *playlist_type = new wxMenu();
    main_menu_bar->Append(options, "Настройки");
    main_menu_bar->Append(playlist_type, "Тип плейлиста");

    m_menu_show_playlist = new wxMenuItem(options, wxNewId(), "Показать плейлист",
                                          "Открывает плейлист", false);
    options->Append(m_menu_show_playlist);

    m_menu_save_audio = new wxMenuItem(options, wxNewId(), "Сохранять аудиозаписи",
                                          "Сохранять аудиозаписи", true);
    options->Append(m_menu_save_audio);

    m_menu_saving_path = new wxMenuItem(options, wxNewId(), "Куда сохранять",
                                        "Путь для сохранения аудио", false);
    options->Append(m_menu_saving_path);

    m_menu_playlist_my = new wxMenuItem(playlist_type, ID_PLAYLIST_MY, "Свой плейлист",
                                        "Плейлист пользователя", true);
    playlist_type->Append(m_menu_playlist_my);
    m_menu_playlist_my->Check();

    m_menu_playlist_search = new wxMenuItem(playlist_type, ID_PLAYLIST_SEARCH, "Поиск",
                                        "Поиск", true);
    playlist_type->Append(m_menu_playlist_search);

    wxMenu *recommend_type = new wxMenu();
    m_menu_playlist_recommend_all = new wxMenuItem(recommend_type, ID_PLAYLIST_RECOMMEND_ALL, "По плейлисту",
                                                   "Рекомментации по плейлисту", true);
    recommend_type->Append(m_menu_playlist_recommend_all);
    m_menu_playlist_recommend_selected = new wxMenuItem(recommend_type, ID_PLAYLIST_RECOMMEND_SELECTED, "По выбранному",
                                                        "Рекоммендации по выбранной записи", true);

    wxMenuItem *recommend = new wxMenuItem(playlist_type, wxID_ANY, "Рекоммендации",
                                        "Рекоммендации", false, recommend_type);
    recommend_type->Append(m_menu_playlist_recommend_selected);
    playlist_type->Append(recommend);

    SetMenuBar(main_menu_bar);
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
    int duration = (m_player->Tell() < 0) ? 0 : m_player->Tell() / 1000;

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

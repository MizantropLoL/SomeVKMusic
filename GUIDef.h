#ifndef GUIDEF_H_INCLUDED
#define GUIDEF_H_INCLUDED

#include "SomeVKMusicMain.h"

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
    wxIcon icon("MAINICON", wxBITMAP_TYPE_ICO_RESOURCE);
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

#endif // GUIDEF_H_INCLUDED

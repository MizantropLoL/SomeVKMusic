#include <wx/sizer.h>
#include <wx/log.h>

#include <fstream>

#include "SomeVKMusicMain.h"
#include "VKPlaylist.h"

SomeVKMusicFrame::SomeVKMusicFrame()
    : wxFrame(nullptr, wxID_ANY, "SomeVKMusic", wxDefaultPosition,
              wxSize(500, 100), wxDEFAULT_DIALOG_STYLE|wxMINIMIZE_BOX){
    InitLogger();
    CreateConnection();
    CreateControls();

    m_connection->MakeQuery(new VKQueryPlaylistGet(2, 7));
    m_connection->ConnectResponseGetter(&SomeVKMusicFrame::AcceptPlaylist);
}

void SomeVKMusicFrame::AcceptPlaylist(const wxString &response){
    VKPlaylist *playlist = VKResponse::GetVKPlaylist(response);
    for (auto song: playlist->Songs){
        wxLogMessage(song->Artist + " - " + song->Title);
    }
}

SomeVKMusicFrame::~SomeVKMusicFrame(){
    delete m_connection;
    m_aui_manager.UnInit();
}

void SomeVKMusicFrame::CreateControls(){
    wxPanel *main_panel;
    wxBoxSizer  *main_controls_sizer,
                *progress_sizer,
                *song_info_sizer,
                *controls_sizer;

    m_aui_manager.SetManagedWindow(this);

    main_panel = new wxPanel(this);

    //+main_controls_sizer--------------------------------------------
    main_controls_sizer = new wxBoxSizer(wxVERTICAL);
    main_panel->SetSizer(main_controls_sizer);
    //+main_controls_sizer Elements=======================================================

    //+progress_sizer-------------------------------------------
    progress_sizer = new wxBoxSizer(wxVERTICAL);
    //+progress_sizer Elements==================================================

    //+song_info_sizer-------------------------------------
    song_info_sizer = new wxBoxSizer(wxHORIZONTAL);
    //+song_info_sizer Elements========================================================
    m_song_duration_current = new wxStaticText(main_panel, wxID_ANY, "00:00");
    m_song_duration_full = new wxStaticText(main_panel, wxID_ANY, "00:00");
    m_current_song_progress = new wxSlider(main_panel, wxID_ANY, 0, 0, 1);
    //-song_info_sizer Elements========================================================
    song_info_sizer->Add(m_song_duration_current, 0, wxEXPAND);
    song_info_sizer->Add(m_current_song_progress, 1, wxEXPAND);
    song_info_sizer->Add(m_song_duration_full, 0, wxEXPAND);
    //-song_info_sizer-----------------------------------------

    m_current_song_name = new wxStaticText(main_panel, wxID_ANY, wxEmptyString);
    //-progress_sizer Elements=============================================================
    progress_sizer->Add(m_current_song_name, 1, wxEXPAND);
    progress_sizer->Add(song_info_sizer, 1, wxEXPAND);
    //-progress_sizer--------------------------------------
    //+controls_sizer------------------------------------
    controls_sizer = new wxBoxSizer(wxHORIZONTAL);
    //+controls_sizer Elements=======================================================
    m_prev = new wxButton(main_panel, ID_BUTTON_PREV, "<<");
    m_play_pause = new wxButton(main_panel, ID_BUTTON_PLAY_PAUSE, ">");
    m_stop = new wxButton(main_panel, ID_BUTTON_STOP, "[]");
    m_next = new wxButton(main_panel, ID_BUTTON_NEXT, ">>");
    //-controls_sizer Elements=======================================================
    controls_sizer->Add(m_prev, 1, wxEXPAND);
    controls_sizer->Add(m_play_pause, 1, wxEXPAND);
    controls_sizer->Add(m_stop, 1, wxEXPAND);
    controls_sizer->Add(m_next, 1, wxEXPAND);
    //-controls_sizer------------------------------------
    //-main_controls_sizer Elements=======================================================
    main_controls_sizer->Add(progress_sizer, 1, wxEXPAND);
    main_controls_sizer->Add(controls_sizer, 1, wxEXPAND);
    //-main_controls_sizer-----------------------------------------

    m_aui_manager.AddPane(main_panel,
                          wxAuiPaneInfo().Center().CloseButton(false).CaptionVisible(false));

    m_aui_manager.Update();
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

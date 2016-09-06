#include "SomeVKMusicMain.h"
#include <wx/sizer.h>

SomeVKMusicFrame::SomeVKMusicFrame()
    : wxFrame(nullptr, wxID_ANY, "SomeVKMusic", wxDefaultPosition,
              wxSize(500, 100), wxDEFAULT_DIALOG_STYLE|wxMINIMIZE_BOX){
    CreateControls();
}

SomeVKMusicFrame::~SomeVKMusicFrame(){
    m_aui_manager.UnInit();
}

void SomeVKMusicFrame::CreateControls(){
    m_aui_manager.SetManagedWindow(this);

    wxPanel main_panel(this);

    //+main_controls_sizer--------------------------------------------
    wxBoxSizer main_controls_sizer(wxVERTICAL);
    main_panel.SetSizer(&main_controls_sizer);
    //+main_controls_sizer Elements=======================================================

    //+progress_sizer-------------------------------------------
    wxBoxSizer progress_sizer(wxVERTICAL);
    //+progress_sizer Elements==================================================

    //+song_info_sizer-------------------------------------
    wxBoxSizer song_info_sizer(wxHORIZONTAL);
    //+song_info_sizer Elements========================================================
    m_song_duration_current = new wxStaticText(this, wxID_ANY, "00:00");
    m_song_duration_full = new wxStaticText(this, wxID_ANY, "00:00");
    m_current_song_progress = new wxSlider(this, wxID_ANY, 0, 0, 1);
    //-song_info_sizer Elements========================================================
    song_info_sizer.Add(m_song_duration_current, 0, wxEXPAND);
    song_info_sizer.Add(m_current_song_progress, 1, wxEXPAND);
    song_info_sizer.Add(m_song_duration_full, 0, wxEXPAND);
    m_current_song_progress->Set
    //-song_info_sizer-----------------------------------------

    m_current_song_name = new wxStaticText(this, wxID_ANY, wxEmptyString);
    //-progress_sizer Elements=============================================================
    progress_sizer.Add(m_current_song_name, 1, wxEXPAND);
    progress_sizer.Add(&song_info_sizer, 1, wxEXPAND);
    //-progress_sizer--------------------------------------
    //+controls_sizer------------------------------------
    wxBoxSizer controls_sizer(wxHORIZONTAL);
    //+controls_sizer Elements=======================================================
    m_prev = new wxButton(this, ID_BUTTON_PREV, "<<");
    m_play_pause = new wxButton(this, ID_BUTTON_PLAY_PAUSE, ">");
    m_stop = new wxButton(this, ID_BUTTON_STOP, "[]");
    m_next = new wxButton(this, ID_BUTTON_NEXT, ">>");
    //-controls_sizer Elements=======================================================
    controls_sizer.Add(m_prev, 1, wxEXPAND);
    controls_sizer.Add(m_play_pause, 1, wxEXPAND);
    controls_sizer.Add(m_stop, 1, wxEXPAND);
    controls_sizer.Add(m_next, 1, wxEXPAND);
    //-controls_sizer------------------------------------
    //-main_controls_sizer Elements=======================================================
    main_controls_sizer.Add(&progress_sizer, 1, wxEXPAND);
    main_controls_sizer.Add(&controls_sizer, 1, wxEXPAND);
    //-main_controls_sizer-----------------------------------------

    m_aui_manager.AddPane(&main_panel,
                          wxAuiPaneInfo().Center().CloseButton(false).CaptionVisible(false));

    m_aui_manager.Update();
}

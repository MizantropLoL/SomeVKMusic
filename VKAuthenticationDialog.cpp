#include "VKAuthenticationDialog.h"

VKAuthenticationDialog::VKAuthenticationDialog(wxString *access_token, wxString *user_id)
        : wxDialog(nullptr, wxID_ANY, "Аутентификация", wxDefaultPosition, wxSize(670, 415)),
          m_access_token(access_token), m_user_id(user_id)
{
    wxString auth_uri;
    auth_uri += "https://oauth.vk.com/authorize?client_id=" + APP_ID
             +  "&scope=" + APP_SCOPE
             +  "&redirect_uri=" + REDIRECT_URI
             +  "&response_type=" + RESPONCE_TYPE;

    m_auth_view = wxWebView::New(this, wxID_ANY, auth_uri);

    Bind(wxEVT_WEBVIEW_LOADED, &VKAuthenticationDialog::OnDownload, this);
    Connect(wxEVT_CLOSE_WINDOW, (wxObjectEventFunction)&VKAuthenticationDialog::OnClose);
}

VKAuthenticationDialog::~VKAuthenticationDialog(){
    delete m_auth_view;
}

void VKAuthenticationDialog::OnClose(wxCommandEvent &event){
    EndModal(wxCANCEL);
}

void VKAuthenticationDialog::OnDownload(wxWebViewEvent &event){
    wxString url = m_auth_view->GetCurrentURL();
    if (url.Find("access_token") == wxString::npos)
        return;

    size_t access_token_start = url.Find("=")+1,
           access_token_end   = url.find("&")-1,
           user_id_start      = url.find_last_of("=")+1;

    *m_access_token = url.SubString(access_token_start, access_token_end);
    *m_user_id = url.substr(user_id_start);

    EndModal(wxOK);
}

#ifndef VKAUTHENTICATIONDIALOG_H_INCLUDED
#define VKAUTHENTICATIONDIALOG_H_INCLUDED

#include <wx/wx.h>
#include <wx/webview.h>

namespace{
    const wxString APP_ID = "5473592";
    const wxString APP_SCOPE = "8";
    const wxString REDIRECT_URI = "http://oauth.vk.com/blank.html?display=touch";
    const wxString RESPONCE_TYPE = "token";
}

class VKAuthenticationDialog : public wxDialog{
public:
    VKAuthenticationDialog(wxString *access_token, wxString *user_id);
    virtual ~VKAuthenticationDialog();

private:
    void OnDownload(wxWebViewEvent &event);
    void OnClose(wxCommandEvent &event);

private:
    wxWebView   *m_auth_view;
    wxString    *m_access_token,
                *m_user_id;
};

#endif // VKAUTHENTICATIONDIALOG_H_INCLUDED

#ifndef VKCONNECTION_H
#define VKCONNECTION_H

#include <wx/wx.h>
#include <wx/xml/xml.h>
#include <wx/webview.h>
#include "VKQuery.h"
#include "VKResponse.h"

class VKConnection : public wxEvtHandler{
public:
    VKConnection();
    virtual ~VKConnection();

public:
    void MakeQuery(VKQuery *query);
    void ConnectResponseGetter(void (*receipt_method)(const wxString &response, void **out), void **out);
    wxString GetUserID() const;

private:
    bool Authenticate();
    void InitWebView();
    void TakeResponse();

private:
    void OnWebSourceLoaded(wxWebViewEvent &event);

private:
    wxString        m_access_token,
                    m_user_id;

    wxWebView      *m_web_source;
    wxFrame        *m_web_frame;

    void (*m_ReceiptMethod)(const wxString &response, void **out) = nullptr;
    void **m_responce_out = nullptr;
};

#endif // VKCONNECTION_H

#include "VKConnection.h"
#include "VKAuthenticationDialog.h"

VKConnection::VKConnection(){
    if (!Authenticate()) throw _("Authentication error");
    InitWebView();
}

VKConnection::~VKConnection(){
    delete m_web_frame;
}

bool VKConnection::Authenticate(){
    VKAuthenticationDialog auth(&m_access_token, &m_user_id);
    return (auth.ShowModal() == wxOK);
}
void VKConnection::InitWebView(){
    m_web_frame = new wxFrame(nullptr, wxID_ANY, wxEmptyString);
    m_web_source = wxWebView::New(m_web_frame, wxID_ANY, "localhost");

    m_web_frame->Connect(wxEVT_WEBVIEW_LOADED,
            wxWebViewEventHandler(VKConnection::OnWebSourceLoaded), nullptr, this);
}

void VKConnection::OnWebSourceLoaded(wxWebViewEvent &event){
    TakeResponse();
}

void VKConnection::TakeResponse(){
    if (m_ReceiptMethod != nullptr && m_responce_out != nullptr)
        m_ReceiptMethod(m_web_source->GetPageText(), m_responce_out);
}

void VKConnection::MakeQuery(VKQuery *query){
    query->SetAccessToken(m_access_token);

    m_web_source->LoadURL(query->GetFullQuery());

    delete query;
}

void VKConnection::ConnectResponseGetter(void (*receipt_method)(const wxString &response, void **out), void **out){
    m_ReceiptMethod = receipt_method;
    m_responce_out = out;
}


wxString VKConnection::GetUserID() const{
    return m_user_id;
}

#ifndef VKQUERY_H_INCLUDED
#define VKQUERY_H_INCLUDED

#include <wx/wx.h>
#include "VKPlaylist.h"

class VKConnection;

namespace{
    const wxString ARG_OWNER_ID     = "owner_id=";
    const wxString ARG_ACCESS_TOKEN = "access_token=";
    const wxString ARG_OFFSET       = "offset=";
    const wxString ARG_COUNT        = "count=";


    const wxString M_BASE_QUERY_AUDIO = "https://api.vk.com/method/audio.get.xml?"
                                        "owner_id="
                                        "&offset="
                                        "&count="
                                        "&access_token=";
}

class VKQuery{
friend VKConnection;
protected:
    wxString m_query_str;

protected:
    bool     m_user_id = false,
             m_access_token = false;

public:
    virtual ~VKQuery() {};

protected:
    virtual wxString GetFullQuery() const{
        return (m_user_id&&m_access_token) ? m_query_str.data() : wxEmptyString;
    }

private:

    void SetOwnerID(const wxString &user_id){
        m_query_str.insert(GetTokenPos(ARG_OWNER_ID), user_id);
        m_user_id = true;
    }
    void SetAccessToken(const wxString &access_token){
        m_query_str.insert(GetTokenPos(ARG_ACCESS_TOKEN), access_token);
        m_access_token = true;
    }

protected:
    int GetTokenPos(const wxString &token){
        return m_query_str.Find(token) + token.Len();
    }

protected:
    VKQuery() {};
};

class VKQueryPlaylistGet : public VKQuery{
private:
    bool m_offset = false,
         m_count = false;

public:
    VKQueryPlaylistGet() {m_query_str = M_BASE_QUERY_AUDIO;}
    VKQueryPlaylistGet(int offset, int count){
        m_query_str = M_BASE_QUERY_AUDIO;
        wxLogMessage(m_query_str);

        SetOffset(offset);
        SetCount(count);

        wxLogMessage(m_query_str);
    }

    virtual ~VKQueryPlaylistGet() {};

    virtual wxString GetFullQuery() const{
        return (m_offset&&m_count) ? VKQuery::GetFullQuery().data() : wxEmptyString;
    }

    void SetOffset(int offset){
        m_query_str.insert(GetTokenPos(ARG_OFFSET), wxString::FromDouble(offset));
        m_offset = true;
    }
    void SetCount(int count){
        m_query_str.insert(GetTokenPos(ARG_COUNT), wxString::FromDouble(count));
        m_count = true;
    }
};

#endif // VKQUERY_H_INCLUDED

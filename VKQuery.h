#ifndef VKQUERY_H_INCLUDED
#define VKQUERY_H_INCLUDED

#include <wx/wx.h>
#include "VKPlaylist.h"

class VKConnection;

namespace{
    const wxString ARG_OWNER_ID     = "owner_id=";
    const wxString ARG_ACCESS_TOKEN = "access_token=";
    const wxString ARG_Q            = "?q=";
    const wxString ARG_TARGET_AUDIO = "target_audio=";


    const wxString M_BASE_QUERY_AUDIO       = "https://api.vk.com/method/audio.get.xml?"
                                                "owner_id="
                                                "&offset=0"
                                                "&count=6000"
                                                "&access_token=";
    const wxString M_BASE_QUERY_SEARCH      = "https://api.vk.com/method/audio.search.xml?"
                                                "q="
                                                "&auto_complete=1"
                                                "&offset=0"
                                                "&count=1000"
                                                "&access_token=";
    const wxString M_BASE_QUERY_RECOMMEND   = "https://api.vk.com/method/audio.getRecommendations.xml?"
                                                "target_audio="
                                                "&owner_id="
                                                "&offset=0"
                                                "&count=1000"
                                                "&shuffle=1"
                                                "&access_token=";
}

class VKQuery{
protected:
    wxString m_query_str;

private:
    wxString m_access_token;

public:
    virtual ~VKQuery() {};

public:
    virtual wxString GetFullQuery(){
        if (m_access_token.IsEmpty()) return wxEmptyString;
        m_query_str.insert(GetTokenPos(ARG_ACCESS_TOKEN), m_access_token);

        return m_query_str;
    }

    void SetAccessToken(const wxString &access_token){
        m_access_token = access_token;
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
    wxString m_user_id;

public:
    VKQueryPlaylistGet(const wxString &user_id = wxEmptyString){
        m_query_str = M_BASE_QUERY_AUDIO;

        SetOwnerID(user_id);
    }

    virtual ~VKQueryPlaylistGet() {}

    virtual wxString GetFullQuery(){
        if (m_user_id.IsEmpty()) return wxEmptyString;

        m_query_str.insert(GetTokenPos(ARG_OWNER_ID), m_user_id);

        return VKQuery::GetFullQuery();
    }

    void SetOwnerID(const wxString &user_id){
        m_user_id = user_id;
    }
};

class VKQueryPlaylistSearch : public VKQuery{
private:
    wxString m_q;

public:
    VKQueryPlaylistSearch(const wxString &q = wxEmptyString){
        m_query_str = M_BASE_QUERY_SEARCH;

        SetQ(q);
    }
    virtual ~VKQueryPlaylistSearch() {}

    virtual wxString GetFullQuery(){
        if (m_q.IsEmpty()) return wxEmptyString;

        m_query_str.insert(GetTokenPos(ARG_Q), m_q);

        return VKQuery::GetFullQuery();
    }

    void SetQ(const wxString &q){
        m_q = q;
    }
};

class VKQueryPlaylistRecommend : public VKQuery{
private:
    wxString m_user_id;
    wxString m_target_audio;

public:
    VKQueryPlaylistRecommend(const wxString &user_id, const wxString &targer_audio = wxEmptyString){
        m_query_str = M_BASE_QUERY_RECOMMEND;

        SetTargetAudio(targer_audio);
        SetOwnerID(user_id);
    }
    virtual ~VKQueryPlaylistRecommend() {}

    virtual wxString GetFullQuery(){
        if (m_user_id.IsEmpty()) return wxEmptyString;

        m_query_str.insert(GetTokenPos(ARG_OWNER_ID), m_user_id);
        if (!m_target_audio.IsEmpty())
            m_query_str.insert(GetTokenPos(ARG_TARGET_AUDIO), m_user_id + "_" + m_target_audio);
        else
            //Do not delete "?", but delete "&"
            m_query_str.erase(GetTokenPos(ARG_TARGET_AUDIO) - ARG_TARGET_AUDIO.Len()+1, ARG_TARGET_AUDIO.Len());

        return VKQuery::GetFullQuery();
    }

    void SetOwnerID(const wxString &user_id){
        m_user_id = user_id;
    }

    void SetTargetAudio(const wxString &target_audio){
        m_target_audio = target_audio;
    }
};

#endif // VKQUERY_H_INCLUDED

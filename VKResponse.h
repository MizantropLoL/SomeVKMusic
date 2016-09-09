#ifndef VKRESPONSE_H_INCLUDED
#define VKRESPONSE_H_INCLUDED

#include <wx/wx.h>
#include <wx/xml/xml.h>
#include <wx/sstream.h>

struct VKSong{
    wxString Artist;
    wxString Title;
    wxString URL;

public:
    VKSong(const wxXmlNode *song_node){
        for (auto info = song_node->GetChildren(); info != nullptr; info = info->GetNext()){
            wxString tag_name = info->GetName();
            wxString tag_data = info->GetNodeContent();
            if      (tag_name == "artist")      Artist = tag_data;
            else if (tag_name == "title")       Title = tag_data;
            else if (tag_name == "url")         URL = tag_data;
            else if (tag_name == "duration")    m_duration = tag_data;
        }

        long duration_tmp;

        m_duration.ToLong(&duration_tmp);
        m_duration_minutes = duration_tmp / 60;
        m_duration_seconds = duration_tmp % 60;
    }

    wxString DurationString()       {return m_duration;}
    int      DurationFullMinutes()  {return m_duration_minutes;}
    int      DurationFullSeconds()  {return m_duration_seconds;}

private:
    wxString m_duration;
    int m_duration_minutes;
    int m_duration_seconds;
};


struct VKPlaylist{
    wxVector<VKSong *> Songs;

    VKPlaylist(wxString response){
        response.Replace("- <", "<");
        response.Trim(false);
        wxStringInputStream input(response);

        wxXmlDocument *response_xml = new wxXmlDocument;
        if (!response_xml->Load(input)) throw "Bad response from VK";

        for (auto audio = response_xml->GetRoot()->GetChildren()->GetNext();
             audio != nullptr;
             audio = audio->GetNext())
        {
            Songs.push_back(new VKSong(audio));
        }
    }
};

class VKResponse{
public:
    static VKPlaylist *GetVKPlaylist(const wxString &response){
        return new VKPlaylist(response);
    }
};

#endif // VKRESPONSE_H_INCLUDED

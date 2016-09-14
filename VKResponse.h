#ifndef VKRESPONSE_H_INCLUDED
#define VKRESPONSE_H_INCLUDED

#include <wx/wx.h>
#include <wx/xml/xml.h>
#include <wx/sstream.h>
#include <wx/uri.h>
#include <cstdlib>
#include <ctime>

struct VKSong{
    wxString Artist;
    wxString Title;
    wxURI    URI;
    wxString ID;

public:
    VKSong(const wxXmlNode *song_node){
        for (auto info = song_node->GetChildren(); info != nullptr; info = info->GetNext()){
            wxString tag_name = info->GetName();
            wxString tag_data = info->GetNodeContent();
            if      (tag_name == "id")          ID = tag_data;
            else if (tag_name == "artist")      Artist = tag_data;
            else if (tag_name == "title")       Title = tag_data;
            else if (tag_name == "url")         URI = tag_data;
            else if (tag_name == "duration")    m_duration_str = tag_data;
        }

        m_duration_str.ToLong(&m_full_duration);
        m_duration_minutes = m_full_duration / 60;
        m_duration_seconds = m_full_duration % 60;
    }

    int      DurationFullMinutes()  const {return m_duration_minutes;}
    int      DurationFullSeconds()  const {return m_duration_seconds;}
    long     DurationFull()         const {return m_full_duration;}
    wxString DurationString()       const {return DurationToString(m_full_duration);}

    static wxString DurationToString(long duration){
        int minutes = duration / 60;
        int seconds = duration % 60;

        return (minutes < 10 ? "0" : "") + wxString::FromDouble(minutes) +
         ':' + (seconds < 10 ? "0" : "") + wxString::FromDouble(seconds);
    }

private:
    wxString m_duration_str;
    int m_duration_minutes;
    int m_duration_seconds;
    long m_full_duration;
};

class VKPlaylist{
private:
    wxVector<VKSong *> m_songs;
    int m_current_song;

public:
    VKPlaylist(wxString response) : m_current_song(0){
        srand(time(nullptr));

        response.Replace("- <", "<");
        response.Replace("&", "");
        response.Trim(false);

        wxStringInputStream input(response);

        wxXmlDocument *response_xml = new wxXmlDocument();
        if (!response_xml->Load(input)) throw _("Bad response from VK");

        for (auto audio = response_xml->GetRoot()->GetChildren()->GetNext();
             audio != nullptr;
             audio = audio->GetNext())
        {
            m_songs.push_back(new VKSong(audio));
        }
    }

    virtual ~VKPlaylist(){
        for (auto song: m_songs)
            delete song;
    }

    const VKSong *GetSong(int number){
        if (number < 0) throw "Audio № can't be less than 0";
        if (number >= m_songs.size()) throw "Audio № can't be biggest than audios count";

        m_current_song = number;
        return m_songs[m_current_song];
    }

    const VKSong *GetNext(){
        if (++m_current_song == m_songs.size()) m_current_song = 0;
        return m_songs[m_current_song];
    }

    const VKSong *GetPrev(){
        if (m_current_song-- == 0) m_current_song = m_songs.size() - 1;
        return m_songs[m_current_song];
    }

    const VKSong *GetRandom(){
        m_current_song = rand() % m_songs.size();
        return m_songs[m_current_song];
    }

    const VKSong *GetCurrent(){
        return m_songs[m_current_song];
    }

    void SetCurrent(int new_current){
        if (new_current < m_songs.size())
            m_current_song = new_current;
    }

    size_t Size() {return m_songs.size();}

    int GetCurrentNo() {return m_current_song;}
};

class VKResponse{
public:
    static VKPlaylist *GetVKPlaylist(const wxString &response){
        return new VKPlaylist(response);
    }
};

#endif // VKRESPONSE_H_INCLUDED

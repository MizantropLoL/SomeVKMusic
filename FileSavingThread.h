#ifndef FILESAVINGTHREAD_H_INCLUDED
#define FILESAVINGTHREAD_H_INCLUDED

#include <wx/wx.h>
#include <wx/thread.h>
#include <wx/url.h>
#include <wx/wfstream.h>

class FileSavingThread : public wxThread{
private:
    wxURL m_save_from;
    wxString m_save_to;

public:
    FileSavingThread(const wxURL &save_from, const wxString &save_to_file)
                : m_save_from(save_from), m_save_to(save_to_file)
    {}

    virtual ~FileSavingThread() {}

    virtual void *Entry(){
        wxDir dir;
        wxString dir_name = m_save_to.SubString(0, m_save_to.Find('/', true));
        if (!dir.Exists(dir_name)){
            dir.Make(dir_name);
            dir.Close();
        }

        wxFile audio_file;
        audio_file.Create(m_save_to, true);
        wxFileOutputStream audio_out_stream(audio_file);

        wxInputStream *audio_in_stream = m_save_from.GetInputStream();

        audio_out_stream.Write(*audio_in_stream);

        audio_out_stream.GetFile()->Flush();
        audio_out_stream.GetFile()->Close();

        delete audio_in_stream;

        return nullptr;
    }
};

#endif // FILESAVINGTHREAD_H_INCLUDED

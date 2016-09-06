#include "SomeVKMusicApp.h"
#include "SomeVKMusicMain.h"

IMPLEMENT_APP(SomeVKMusicApp);

bool SomeVKMusicApp::OnInit()
{
    SomeVKMusicFrame* frame = new SomeVKMusicFrame();
    frame->Show();

    return true;
}

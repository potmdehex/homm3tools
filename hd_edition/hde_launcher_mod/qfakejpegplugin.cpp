#include "qfakejpegplugin.h"
#include "hook.h"

#include <QTimer>

QFakeJpegPlugin::QFakeJpegPlugin()
{
    // hook CreateProcess to inject DLL into launched HOMM3 2.0.exe
    HookCreateProcess();

    // Modify launcher gui.
    // This constructor is run while the launcher GUI is still being created
    // and not all elements exist yet. We want to modify the GUI once it is
    // already created, so we set a single shot timer which is run when the 
    // main thread is idle AKA when the launcher is done creating its GUI.
    QTimer::singleShot(0, &m_guiExt, SLOT(extendGui()));
}

QFakeJpegPlugin::~QFakeJpegPlugin()
{
    
}

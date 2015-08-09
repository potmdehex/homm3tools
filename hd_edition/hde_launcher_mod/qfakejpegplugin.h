#ifndef __QFAKEPLUGIN_H_DEF__
#define __QFAKEPLUGIN_H_DEF__

#include <QApplication>
#include <QImageIOPlugin>

#include "guimod.h"

class QFakeJpegPlugin : public QImageIOPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QImageIOHandlerFactoryInterface" FILE "qfakejpegplugin.json")
public:
    QFakeJpegPlugin();
    ~QFakeJpegPlugin();
    // Empty implementations for all virtual functions
    Capabilities capabilities(QIODevice *device, const QByteArray &format) const { return 0; };
    QImageIOHandler *create(QIODevice *device, const QByteArray &format) const { return 0; }
    QStringList keys() const { return QStringList(); };
private:
    GuiMod m_guiExt;
};

#endif // __QFAKEPLUGIN_H_DEF__

// This file contains template function(s), so it is meant to be included directly with
// #include "qtu.cpp"

#include <QApplication>
#include <QFileInfo>
#include <QWidget>
#include <QThread>

static QWidget *qtuFindMainWidget(bool retry = false, int interval = 10)
{
    for (;;)
    {
        foreach(QWidget *widget, QApplication::topLevelWidgets())
        {
            if (widget->inherits("QMainWindow"))
            {
                return widget;
            }
        }

        if (!retry)
            break;

        QThread::msleep(interval);
    }

    return NULL;
}

template <class T>
QWidget *qtuFindChildWidget(T *parent, const QString& name = QString(), bool recurse = true)
{
    foreach(QObject *child, parent->children())
    {
        if (!child)
            continue;

        if (name.isNull() || name == child->objectName())
        {
            return qobject_cast<QWidget *>(child);
        }
        if (recurse)
        {
            auto ret = qtuFindChildWidget(child, name, recurse);
            if (ret)
                return ret;
        }
    }
    return 0;
}

// http://stackoverflow.com/a/26991243
static bool qtuFileExists(QString path) {
    QFileInfo checkFile(path);
    // check if file exists and if yes: Is it really a file and no directory?
    if (checkFile.exists() && checkFile.isFile()) {
        return true;
    }
    else {
        return false;
    }
}

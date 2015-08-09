#ifndef __CLICKABLELABEL_H_DEF__
#define __CLICKABLELABEL_H_DEF__

#include <QApplication>
#include <QLabel>

// From http://wiki.qt.io/Make-a-QLabel-Clickable
class ClickableLabel : public QLabel
{
    Q_OBJECT
public:
    explicit ClickableLabel(const QString& text = "", QWidget* parent = 0);
    ~ClickableLabel();
signals:
    void clicked();
protected:
    void mousePressEvent(QMouseEvent* event);
};

#endif

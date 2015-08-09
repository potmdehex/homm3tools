#ifndef __GUIMOD_H_DEF__
#define __GUIMOD_H_DEF__

#include <QObject>
#include <QCheckBox>
#include <QGroupBox>
#include <QLineEdit>

class GuiMod : public QObject
{
    Q_OBJECT
public:
    GuiMod();
    ~GuiMod();
    void createCheckOptions(QWidget *parent, const QFont &font, const QPalette &palette);
    void browseRmg();
public slots:
    void extendGui();
    void playBtnClicked();
    void creditsBtnClicked();
    void donateBtnClicked();
    void browseBtnClicked();
    void optionLabelClicked();
    void rmgCheckToggled(bool toggle);
    void hotkeysLinkActivated(const QString &link);
private:
    QWidget *m_main;
    QGroupBox *m_groupBox;
    QLineEdit *m_rmgEdit;
    QCheckBox *m_rmgCheck;
};

#endif // __GUIMOD_H_DEF__


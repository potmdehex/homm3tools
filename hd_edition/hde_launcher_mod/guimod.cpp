#include "guimod.h"
#include "clickablelabel.h"
#include "qtu.cpp" // contains template function(s) so can't have normal header
#include "stylesheets.h"
#include "version.h"

#include <QApplication>
#include <QCheckBox>
#include <QDesktopServices>
#include <QFileDialog>
#include <QGroupBox>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QTimer>
#include <QUrl>
#include <QMessageBox>

#include <cstdint>

#include <settings_utils.h>

#define SETTINGS_PATH "Software\\h3minternals"

static void _applyCheckBoxSettings(QObject *parent)
{
    QCheckBox *checkbox;
    foreach(QObject *child, parent->children())
    {
        if (NULL == (checkbox = qobject_cast<QCheckBox *>(child))
            || 0 == checkbox->objectName().count('|'))
        {
            _applyCheckBoxSettings(child);
            continue;
        }

        QString name = QString("enable_") + checkbox->objectName().split('|')[0];
        settings_set_int(SETTINGS_PATH, name.toStdString().c_str(), (int)checkbox->isChecked());
    }
}

void GuiMod::createCheckOptions(QWidget *parent, const QFont &font, const QPalette &palette)
{
    const int labelX = 0x0A;
    const int checkX = 0x169;
    const int initialY = 0x35;
    const int incY = 24;
    const char *options[] = { 
        "Fullscreen|fullscreen",
        "Re-visitable Objects (re-visit with spacebar)|revisitable",
        "Extra Hotkeys "
        "<a href=\"fbi.gov :-)\"><span style=\"color: #FFCC4B;\">(?)</span></a>"
        "|hotkeys",
        "60 FPS (default: 30)|fps",
        "Never Hide Cursor|cursor",
        "No Sound Delay|sound",
        "XXL Maps Patch (removes minimap rendering)|xxl",
        "RMG|rmg"};
    bool defaults[] = {
        true,  // fullscreen, unused
        true,  // re-visit
        true,  // hotkeys
        true,  // fps
        true,  // cursor
        false, // sound
        false, // xxl
        false  // rmg
    };

    bool checked = 0;

    for (int i = 0; i < sizeof(options) / sizeof(options[0]); ++i)
    {
        QCheckBox *checkbox = new QCheckBox(parent);
        checkbox->setGeometry(checkX, initialY + (incY * i), 0x32, 0xE);
        checkbox->setStyleSheet(COMBOBOX_ORIG_STYLESHEET);
        checkbox->show();
        checkbox->setObjectName(QString(options[i]).split('|')[1] + QString("|check"));

        checked = defaults[i];
        settings_get_int(SETTINGS_PATH, 
            (QString("enable_") + QString(options[i]).split('|')[1]).toStdString().c_str(), (int *)&checked);
        checkbox->setChecked(checked);

        ClickableLabel *label = new ClickableLabel("", parent);
        label->setGeometry(labelX, initialY + (incY * i) - 4, 0x200, 0x16);
        label->setText(QString(options[i]).split('|')[0]);
        label->setObjectName(QString(options[i]).split('|')[1] + QString("|label"));
        label->setStyleSheet(QString(FULLSCREENLABEL_ORIG_STYLESHEET));
        label->setFont(font);
        label->setPalette(palette);
        label->show();
        connect(label, SIGNAL(clicked()), this, SLOT(optionLabelClicked()));
    }
}

void GuiMod::extendGui()
{
    m_main = qtuFindMainWidget(false);
    QWidget *centralWidget = qtuFindChildWidget(m_main, "centralWidget");
    QWidget *layout = qtuFindChildWidget(m_main, "_layout");
    QGroupBox *m_groupBox = qobject_cast<QGroupBox *>(qtuFindChildWidget(m_main, "videoGBox"));
    QLabel *versionLbl = qobject_cast<QLabel *>(qtuFindChildWidget(m_main, "versionLbl"));
    QLabel *fullscreenLbl = qobject_cast<QLabel *>(qtuFindChildWidget(m_main, "fullscreenLbl"));
    QLabel *resolutionLbl = qobject_cast<QLabel *>(qtuFindChildWidget(m_main, "resolutionLbl"));
    QCheckBox *fullscreenBox = qobject_cast<QCheckBox *>(qtuFindChildWidget(m_main, "fullscreenBox"));
    QPushButton *playBtn = qobject_cast<QPushButton *>(qtuFindChildWidget(m_main, "playBtn"));
    QPushButton *tutorialBtn = qobject_cast<QPushButton *>(qtuFindChildWidget(m_main, "tutorialBtn")); // orig 0xb1, 0x27
    QPushButton *mapEditorBtn = qobject_cast<QPushButton *>(qtuFindChildWidget(m_main, "mapEditorBtn"));

    resolutionLbl->setGeometry(resolutionLbl->x(), resolutionLbl->y() + 4, resolutionLbl->width(), resolutionLbl->height());

    fullscreenLbl->hide();
    fullscreenBox->hide();

    versionLbl->setText(versionLbl->text() + QString(", HDE Mod " VERSION));
    versionLbl->setParent(centralWidget);
    versionLbl->setStyleSheet(VERSIONLABEL_NEW_STYLESHEET);
    versionLbl->setGeometry(versionLbl->x() - 36, versionLbl->y(), 150, 21);
    m_groupBox->setGeometry(0x1F, 0x20, 0x1A2, 0x142);
    m_groupBox->setStyleSheet(GROUPBOX_NEW_STYLESHEET);

    tutorialBtn->hide();

    playBtn->setGeometry(tutorialBtn->geometry());
    playBtn->disconnect();
    playBtn->setStyleSheet(PLAYBTN_ORIG_STYLESHEET);
    connect(playBtn, SIGNAL(clicked()), this, SLOT(playBtnClicked()));

    createCheckOptions(m_groupBox, fullscreenLbl->font(), fullscreenLbl->palette());

    m_rmgCheck = qobject_cast<QCheckBox *>(qtuFindChildWidget(m_main, "rmg|check"));
    m_rmgCheck->setGeometry(m_rmgCheck->x() - 0x15D, m_rmgCheck->y(), m_rmgCheck->width(), m_rmgCheck->height());
    connect(m_rmgCheck, SIGNAL(toggled(bool)), this, SLOT(rmgCheckToggled(bool)));

    QLabel *rmgLabel = qobject_cast<QLabel *>(qtuFindChildWidget(m_main, "rmg|label"));
    m_rmgEdit = new QLineEdit(m_groupBox);
    m_rmgEdit->setGeometry(rmgLabel->x() + 0x36, rmgLabel->y(), 0xDF, 0x16);
    m_rmgEdit->show();
    m_rmgEdit->setPlaceholderText("H3 Complete Path (for RMG)...");
    m_rmgEdit->setStyleSheet(QString(LINEEDIT_NEW_STYLESHEET));
    QFont rmgFont = fullscreenLbl->font();
    m_rmgEdit->setFont(rmgFont);

    char mapedPath[512] = { 0 };
    settings_get_ascii(SETTINGS_PATH, "orig_maped_path", mapedPath, sizeof(mapedPath));
    m_rmgEdit->setText(QString(mapedPath));

    QPushButton *browseBtn = new QPushButton(m_groupBox);
    browseBtn->setGeometry(rmgLabel->x() + 0x117, rmgLabel->y(), 0x76, 0x1A);
    browseBtn->setObjectName("browseBtn");
    browseBtn->setStyleSheet(BROWSEBTN_NEW_STYLESHEET);
    browseBtn->setText("Browse");
    browseBtn->show();
    browseBtn->setFont(fullscreenLbl->font());
    connect(browseBtn, SIGNAL(clicked()), this, SLOT(browseBtnClicked()));
    
    QLabel *newFullscreenLbl = qobject_cast<QLabel *>(qtuFindChildWidget(m_main, "fullscreen|label"));
    newFullscreenLbl->setText(fullscreenLbl->text());
    
    QCheckBox *newFullscreenBox = qobject_cast<QCheckBox *>(qtuFindChildWidget(m_main, "fullscreen|check"));
    newFullscreenBox->setChecked(fullscreenBox->isChecked());
    
    // Connect new checkbox to the slot in the original executable
    connect(newFullscreenBox, SIGNAL(toggled(bool)), m_main, SLOT(fullscreenBoxToggled(bool)));

    QPushButton *creditsBtn = new QPushButton(m_groupBox);
    creditsBtn->setGeometry(0x06, 0x122, 0x76, 0x1A);
    creditsBtn->setObjectName("creditsBtn");
    creditsBtn->setStyleSheet(CREDITSBTN_NEW_STYLESHEET);
    creditsBtn->setText("Credits");
    creditsBtn->show();
    creditsBtn->setFont(fullscreenLbl->font());
    connect(creditsBtn, SIGNAL(clicked()), this, SLOT(creditsBtnClicked()));

    QPushButton *donateBtn = new QPushButton(m_groupBox);
    donateBtn->setGeometry(0x126, 0x122, 0x76, 0x1A);
    donateBtn->setObjectName("donateBtn");
    donateBtn->setStyleSheet(DONATEBTN_NEW_STYLESHEET);
    donateBtn->setText("Donate!");
    donateBtn->show();
    donateBtn->setFont(fullscreenLbl->font());
    connect(donateBtn, SIGNAL(clicked()), this, SLOT(donateBtnClicked()));

    ClickableLabel *hotkeysLbl = qobject_cast<ClickableLabel *>(qtuFindChildWidget(m_main, "hotkeys|label"));
    connect(hotkeysLbl, SIGNAL(linkActivated(const QString &)), this, SLOT(hotkeysLinkActivated(const QString &)));
}

void GuiMod::playBtnClicked()
{
    // Apply settings that will be loaded by DLL injected into HOMM3 2.0.exe
    QString path = QDir::toNativeSeparators(QCoreApplication::applicationDirPath());
    settings_set_ascii(SETTINGS_PATH, "maped_rmg_path", (path + QString("\\maped_rmg.dll")).toStdString().c_str());
    settings_set_ascii(SETTINGS_PATH, "templates_path", (path + QString("\\templates")).toStdString().c_str());
    settings_set_ascii(SETTINGS_PATH, "orig_maped_path", m_rmgEdit->text().toStdString().c_str());
    _applyCheckBoxSettings(m_main);

    // Have the play function in the launcher executable get called, 
    // launching HOMM3 2.0.exe
    QTimer::singleShot(0, m_main, "1playBtnClicked()");
}

void GuiMod::donateBtnClicked()
{
    QDesktopServices::openUrl(QUrl(QString("https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=FH4RSQKJTMWJS")));
}

void GuiMod::creditsBtnClicked()
{
    // TODO: more fancy credits display
    QMessageBox *msg = new QMessageBox();
    msg->setText(
        "Reverse Engineering and Programming:\n"
        "potmdehex\n"
        "\nBug Reports, Localization Support and Testing:\n"
        "000, "
        "Above The Clouds, "
        "AL-KASAB, "
        "Avroveks, "
        "Chirno, "
        "Dahan, "
        "damoaraimbo, "
        "Daninja, "
        "DataPack, "
        "Evilnapkin, "
        "Fastmancz, "
        "Heisenberg, "
        "Hightower, "
        "JakeMunsie, "
        "Merkuri, "
        "Mert, "
        "NecroSando, "
        "primeval1, "
        "Reset., "
        "Schnixxor, "
        "SelvesteSigurd, "
        "simonizer, "
        "Solipso, "
        "Urz, "
        "Vampire_Rush"
        " and "
        "wikidfps"
        );
    msg->setIcon(QMessageBox::Warning); // It's pretty
    msg->show();
}

void GuiMod::browseRmg()
{
    QString path = QDir::toNativeSeparators(QFileDialog::getExistingDirectory(m_main,
        tr("Choose H3 Complete Path (containing h3maped.exe)"),
        m_rmgEdit->text(),
        QFileDialog::ShowDirsOnly
        | QFileDialog::DontResolveSymlinks));

    if (path.isEmpty())
    {
        m_rmgCheck->setChecked(false);
        return;
    }

    if (!qtuFileExists(path + "\\h3maped.exe"))
    {
        QMessageBox messageBox;
        messageBox.critical(0, "Invalid Path", "h3maped.exe not found in selected path!");
        messageBox.show();
        m_rmgCheck->setChecked(false);
        return;
    }

    m_rmgEdit->setText(path);
    m_rmgCheck->setChecked(true);
}

void GuiMod::browseBtnClicked()
{
    browseRmg();
}

void GuiMod::rmgCheckToggled(bool toggle)
{
    if (false == toggle)
    {
        return;
    }

    QString path = m_rmgEdit->text();
    if (!qtuFileExists(path + "\\h3maped.exe"))
    {
        browseRmg();
    }
}

void GuiMod::optionLabelClicked()
{
    QObject *sender = QObject::sender();
    if (NULL == sender)
    {
        return;
    }

    QCheckBox *check = qobject_cast<QCheckBox *>(qtuFindChildWidget(m_main, QString(sender->objectName().split('|')[0] + QString("|check"))));
    if (NULL == check)
    {
        return;
    }

    check->setChecked(!check->isChecked());
}

void GuiMod::hotkeysLinkActivated(const QString &link)
{
    // Hack to re-toggle the checkbox that was toggled when Question mark was clicked
    QCheckBox *check = qobject_cast<QCheckBox *>(qtuFindChildWidget(m_main, "hotkeys|check"));
    check->toggle();

    QMessageBox *msg = new QMessageBox();
    msg->setIcon(QMessageBox::Question);
    msg->setText("Quick Combat Battle Result Screen:\n"
        "ESC: Re-play this battle without Quick Combat\n\n"
        "Hero Trade Screen:\n"
        "Q: Swap creatures and artifacts\n"
        "1: Move all creatures and artifacts to hero 1 (left)\n"
        "2: Move all creatures and artifacts to hero 2 (right)\n"
        "\nTown/Hero/Hero Trade Screen:\n"
        "Ctrl+click: split clicked creature stack into as many stacks of 1 as possible\n"
        "Shift+click: split clicked creature stack evenly\n"
        "Alt+click: join all creatures of this type into this stack\n\n"
        "Moving last stack from hero to hero / town to hero: all creatures except 1 are moved (without mod nothing "
        "happens when trying to move last stack)");
    msg->show();
}

GuiMod::GuiMod()
{
    
}

GuiMod::~GuiMod()
{
    
}

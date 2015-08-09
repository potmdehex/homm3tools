#ifndef __STYLESHEETSHEETS_H_DEF__
#define __STYLESHEETSHEETS_H_DEF__

#define PLAYBTN_ORIG_STYLESHEET "QPushButton#playBtn" \
    "{" \
    "    background-image: url(:/assets/res/bg_small_btn.png);\ncolor: rgb(0, 0, 0);" \
    "    border-style: none;" \
    "}" \
    "QPushButton#playBtn:hover" \
    "{" \
    "    background-image: url(:/assets/res/bg_small_hover_btn.png);\ncolor: rgb(0, 0, 0); " \
    "    border-style: none;" \
    "}" \
    "QPushButton#playBtn:pressed" \
    "{" \
    "    background-image: url(:/assets/res/bg_small_pressed_btn.png);\ncolor: rgb(0, 0, 0);" \
    "    border-style: none;" \
    "    padding-top: 1px;" \
    "}"

#define BROWSEBTN_NEW_STYLESHEET "QPushButton#browseBtn" \
    "{" \
    "    border-image: url(:/assets/res/bg_small_btn.png) 0 0 0 0 stretch stretch;\ncolor: rgb(0, 0, 0);" \
    "    border-style: none;" \
    "}" \
    "QPushButton#browseBtn:hover" \
    "{" \
    "    border-image: url(:/assets/res/bg_small_btn.png) 0 0 0 0 stretch stretch;\ncolor: rgb(0, 0, 0); " \
    "    border-style: none;" \
    "}" \
    "QPushButton#browseBtn:pressed" \
    "{" \
    "    border-image: url(:/assets/res/bg_small_btn.png) 0 0 0 0 stretch stretch;\ncolor: rgb(0, 0, 0);" \
    "    border-style: none;" \
    "    padding-top: 1px;" \
    "}"

#define CREDITSBTN_NEW_STYLESHEET "QPushButton#creditsBtn" \
    "{" \
    "    border-image: url(:/assets/res/bg_small_btn.png) 0 0 0 0 stretch stretch;\ncolor: rgb(0, 0, 0);" \
    "    border-style: none;" \
    "}" \
    "QPushButton#creditsBtn:hover" \
    "{" \
    "    border-image: url(:/assets/res/bg_small_btn.png) 0 0 0 0 stretch stretch;\ncolor: rgb(0, 0, 0); " \
    "    border-style: none;" \
    "}" \
    "QPushButton#creditsBtn:pressed" \
    "{" \
    "    border-image: url(:/assets/res/bg_small_btn.png) 0 0 0 0 stretch stretch;\ncolor: rgb(0, 0, 0);" \
    "    border-style: none;" \
    "    padding-top: 1px;" \
    "}"

#define DONATEBTN_NEW_STYLESHEET "QPushButton#donateBtn" \
    "{" \
    "    border-image: url(:/assets/res/bg_small_btn.png) 0 0 0 0 stretch stretch;\ncolor: rgb(0, 0, 0);" \
    "    border-style: none;" \
    "}" \
    "QPushButton#donateBtn:hover" \
    "{" \
    "    border-image: url(:/assets/res/bg_small_btn.png) 0 0 0 0 stretch stretch;\ncolor: rgb(0, 0, 0); " \
    "    border-style: none;" \
    "}" \
    "QPushButton#donateBtn:pressed" \
    "{" \
    "    border-image: url(:/assets/res/bg_small_btn.png) 0 0 0 0 stretch stretch;\ncolor: rgb(0, 0, 0);" \
    "    border-style: none;" \
    "    padding-top: 1px;" \
    "}"

#define GROUPBOX_ORIG_STYLESHEET "QGroupBox" \
    "{" \
    "    border: 1px solid;" \
    "    border-color: rgb(255, 204, 75);" \
    "    color: rgb(255, 255, 255);" \
    "    background-color: rgba(225, 195, 46, 50);" \
    "}"

#define GROUPBOX_NEW_STYLESHEET "QGroupBox" \
    "{" \
    "    border: 1px solid;" \
    "    border-color: rgb(255, 204, 75);" \
    "    color: rgb(255, 255, 255);" \
    "    background-color: rgba(0, 0, 0, 180);" \
    "}"

#define FULLSCREENLABEL_ORIG_STYLESHEET "QLabel" \
    "{" \
    "    color: rgb(255, 255, 255);" \
    "}"

#define VERSIONLABEL_NEW_STYLESHEET "QLabel" \
    "{" \
    "    color: rgba(255, 204, 75, 140);" \
    "}"

#define COMBOBOX_ORIG_STYLESHEET "QCheckBox::indicator" \
    "{" \
    "     width: 14px;" \
    "     height: 14px;" \
    "}" \
    "QCheckBox::indicator:unchecked" \
    "{" \
    "     image: url(:/assets/res/checkbox_notchecked.png);" \
    "}" \
    "QCheckBox::indicator:checked" \
    "{" \
    "     image: url(:/assets/res/checkbox_checked.png);" \
    "}" \
    "QCheckBox::indicator" \
    "{" \
    "	subcontrol-position: right;" \
    "}" \
    "QCheckBox" \
    "{" \
    "	padding : 4px;" \
    "	color: rgb(255, 255, 255);" \
    "}"

#define LINEEDIT_NEW_STYLESHEET "QLineEdit" \
    "{" \
    "    border: 1px solid;" \
    "    border-color: rgb(255, 204, 75);" \
    "    color: rgb(255, 255, 255);" \
    "    background-color: rgba(0, 0, 0, 130);" \
    "}"

#endif

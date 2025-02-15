/*
 * This file is part of the DSView project.
 * DSView is based on PulseView.
 *
 * Copyright (C) 2013 DreamSourceLab <support@dreamsourcelab.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include <boost/bind.hpp>   
#include <QMetaObject>
#include <QFileDialog> 
#include <deque>
#include <QApplication>

#include "filebar.h"
#include "../ui/msgbox.h"
#include "../config/appconfig.h"
#include "../utility/path.h"

#include "../ui/langresource.h"

namespace pv {
namespace toolbars {

FileBar::FileBar(SigSession *session, QWidget *parent) :
    QToolBar("File Bar", parent),
    _session(session),
    _file_button(this)
{
    setMovable(false);
    setContentsMargins(0,0,0,0);

    _action_load = new QAction(this);
    _action_load->setObjectName(QString::fromUtf8("actionLoad"));
 
    _action_store = new QAction(this);
    _action_store->setObjectName(QString::fromUtf8("actionStore"));
 
    _action_default = new QAction(this);
    _action_default->setObjectName(QString::fromUtf8("actionDefault"));
  
    //second level menu
    _menu_session = new QMenu(this);
    _menu_session->setObjectName(QString::fromUtf8("menuSession"));
    _menu_session->addAction(_action_load);
    _menu_session->addAction(_action_store);
    _menu_session->addAction(_action_default);

    _action_open = new QAction(this);
    _action_open->setObjectName(QString::fromUtf8("actionOpen"));
    
    _action_save = new QAction(this);
    _action_save->setObjectName(QString::fromUtf8("actionSave"));
     
    _action_export = new QAction(this);
    _action_export->setObjectName(QString::fromUtf8("actionExport"));
     
    _action_capture = new QAction(this);
    _action_capture->setObjectName(QString::fromUtf8("actionCapture"));
 
    _file_button.setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    _file_button.setPopupMode(QToolButton::InstantPopup);

    _menu = new QMenu(this);
    _menu->addMenu(_menu_session);
    _menu->addAction(_action_open);
    _menu->addAction(_action_save);
    _menu->addAction(_action_export);
    _menu->addAction(_action_capture);
    _file_button.setMenu(_menu);
    addWidget(&_file_button);

    retranslateUi();

    connect(_action_load, SIGNAL(triggered()), this, SLOT(on_actionLoad_triggered()));
    connect(_action_store, SIGNAL(triggered()), this, SLOT(on_actionStore_triggered()));
    connect(_action_default, SIGNAL(triggered()), this, SLOT(on_actionDefault_triggered()));
    connect(_action_open, SIGNAL(triggered()), this, SLOT(on_actionOpen_triggered()));
    connect(_action_save, SIGNAL(triggered()), this, SIGNAL(sig_save()));
    connect(_action_export, SIGNAL(triggered()), this, SIGNAL(sig_export()));
    connect(_action_capture, SIGNAL(triggered()), this, SLOT(on_actionCapture_triggered()));
}

void FileBar::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange)
        retranslateUi();
    else if (event->type() == QEvent::StyleChange)
        reStyle();
    QToolBar::changeEvent(event);
}



void FileBar::retranslateUi()
{
    _file_button.setText(L_S(STR_PAGE_TOOLBAR, S_ID(IDS_FILEBAR_FILE), "File"));
    _menu_session->setTitle(L_S(STR_PAGE_TOOLBAR, S_ID(IDS_FILEBAR_CONFIGURATION), "Con&fig...")); //load,save session file
    _action_load->setText(L_S(STR_PAGE_TOOLBAR, S_ID(IDS_FILEBAR_LOAD), "&Load..."));
    _action_store->setText(L_S(STR_PAGE_TOOLBAR, S_ID(IDS_FILEBAR_STORE), "S&tore..."));
    _action_default->setText(L_S(STR_PAGE_TOOLBAR, S_ID(IDS_FILEBAR_DEFAULT), "&Default..."));
    _action_open->setText(L_S(STR_PAGE_TOOLBAR, S_ID(IDS_FILEBAR_0PEN), "&Open..."));
    _action_save->setText(L_S(STR_PAGE_TOOLBAR, S_ID(IDS_FILEBAR_SAVE), "&Save..."));
    _action_export->setText(L_S(STR_PAGE_TOOLBAR, S_ID(IDS_FILEBAR_EXPORT), "&Export..."));
    _action_capture->setText(L_S(STR_PAGE_TOOLBAR, S_ID(IDS_FILEBAR_CAPTURE), "&Capture..."));
}

void FileBar::reStyle()
{
    QString iconPath = GetIconPath();

    _action_load->setIcon(QIcon(iconPath+"/open.svg"));
    _action_store->setIcon(QIcon(iconPath+"/save.svg"));
    _action_default->setIcon(QIcon(iconPath+"/gear.svg"));
    _menu_session->setIcon(QIcon(iconPath+"/gear.svg"));
    _action_open->setIcon(QIcon(iconPath+"/open.svg"));
    _action_save->setIcon(QIcon(iconPath+"/save.svg"));
    _action_export->setIcon(QIcon(iconPath+"/export.svg"));
    _action_capture->setIcon(QIcon(iconPath+"/capture.svg"));
    _file_button.setIcon(QIcon(iconPath+"/file.svg"));
}

void FileBar::on_actionOpen_triggered()
{
    //open data file
    AppConfig &app = AppConfig::Instance(); 

    if (_session->have_hardware_data() && _session->is_first_store_confirm()){
        if (MsgBox::Confirm(L_S(STR_PAGE_MSG, S_ID(IDS_MSG_SAVE_CAPDATE), "Save captured data?"))){
            sig_save();
            return;
        }
    }

    // Show the dialog
    const QString file_name = QFileDialog::getOpenFileName(
        this, 
        L_S(STR_PAGE_DLG, S_ID(IDS_DLG_OPEN_FILE), "Open File"), 
        app._userHistory.openDir,
        "DSView Data (*.dsl)");

    if (!file_name.isEmpty()) { 
        QString fname = path::GetDirectoryName(file_name);
        if (fname != app._userHistory.openDir){
            app._userHistory.openDir = fname;
            app.SaveHistory();
        }

        sig_load_file(file_name);
    }
}

void FileBar::on_actionLoad_triggered()
{ 
    //load session file
    AppConfig &app = AppConfig::Instance();      
    const QString file_name = QFileDialog::getOpenFileName(
        this, 
        L_S(STR_PAGE_DLG, S_ID(IDS_DLG_OPEN_SEESION), "Open Session"), 
        app._userHistory.sessionDir, 
        "DSView Session (*.dsc)");

    if (!file_name.isEmpty()) {
        QString fname = path::GetDirectoryName(file_name);
        if (fname != app._userHistory.sessionDir){
            app._userHistory.sessionDir = fname;
            app.SaveHistory();
        }
         
        sig_load_session(file_name);
    }
}

void FileBar::on_actionDefault_triggered()
{ 
    QDir dir(GetResourceDir());
    if (!dir.exists()) { 
          MsgBox::Show(NULL, L_S(STR_PAGE_MSG, S_ID(IDS_MSG_SAVE_CANFIND_SESSIONFILE), "Cannot find default session file for this device!"), this);
          return;
    }

    QString driver_name = _session->get_device()->name();
    QString mode_name = QString::number(_session->get_device()->get_work_mode());

    int language = AppConfig::Instance()._frameOptions.language;
    QString file_name = dir.absolutePath() + "/" + driver_name + mode_name +
                                ".def" + QString::number(language) + ".dsc";
    sig_load_session(file_name);
}

void FileBar::on_actionStore_triggered()
{
    //store session file
  
      AppConfig &app = AppConfig::Instance();  

    QString file_name = QFileDialog::getSaveFileName(
                this, 
                L_S(STR_PAGE_DLG, S_ID(IDS_DLG_SAVE_SEESION), "Save Session"),
                app._userHistory.sessionDir,
                "DSView Session (*.dsc)");

    if (!file_name.isEmpty()) {
        QFileInfo f(file_name);
        if(f.suffix().compare("dsc"))
            file_name.append(".dsc");

        QString fname = path::GetDirectoryName(file_name);
        if (fname != app._userHistory.sessionDir){
            app._userHistory.sessionDir = fname;
            app.SaveHistory();
        }
         
        sig_store_session(file_name);
    }
}

void FileBar::on_actionCapture_triggered()
{
    _file_button.close();
    QCoreApplication::sendPostedEvents();
    QTimer::singleShot(100, this, SIGNAL(sig_screenShot()));
}

void FileBar::update_view_status()
{
    bool bEnable = _session->is_working() == false;
    bool is_hardware = _session->get_device()->is_hardware();
    _file_button.setEnabled(bEnable);
    _menu_session->setEnabled(bEnable && is_hardware); 
}

} // namespace toolbars
} // namespace pv

/*
 * This file is part of the DSView project.
 * DSView is based on PulseView.
 * 
 * Copyright (C) 2021 DreamSourceLab <support@dreamsourcelab.com>
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

#include "appcontrol.h"

#include <libsigrok.h>
#include <libsigrokdecode.h>
#include <QDir>
#include <QCoreApplication>
#include <QWidget>
#include <string>
#include "sigsession.h"
#include "dsvdef.h"
#include "config/appconfig.h"
#include "log.h"
#include "utility/path.h"
#include "utility/encoding.h"

AppControl::AppControl()
{
    _topWindow = NULL; 
    _session = new pv::SigSession();
}

AppControl::AppControl(AppControl &o)
{
    (void)o;
}
 
AppControl::~AppControl()
{ 
    DESTROY_OBJECT(_session);
}

AppControl* AppControl::Instance()
{
    static AppControl *ins = NULL;
    if (ins == NULL){
        ins = new AppControl();
    }
    return ins;
}

void AppControl::Destroy(){
    delete this;
} 

bool AppControl::Init()
{    
    _session->init();

    pv::encoding::init();

    srd_log_set_context(dsv_log_context());

#if defined(_WIN32) && defined(DEBUG_INFO)
    //able run debug with qtcreator
    QString pythonHome = "c:/python";
    QDir pydir;
    if (pydir.exists(pythonHome)){
        const wchar_t *pyhome = reinterpret_cast<const wchar_t*>(pythonHome.utf16());
        srd_set_python_home(pyhome);
    }
  
#endif
    
    //the python script path of decoder
    char path[256] = {0};
    QString dir = GetDecodeScriptDir();   
    strcpy(path, dir.toUtf8().data());

    dsv_info("Decode script files directory:\"%s\"", dir.toUtf8().data());

    // Initialise libsigrokdecode
    if (srd_init(path) != SRD_OK)
    {
        m_error = "ERROR: libsigrokdecode init failed.";
        return false;
    }

    // Load the protocol decoders
    if (srd_decoder_load_all() != SRD_OK)
    {
        m_error = "ERROR: load the protocol decoders failed.";
        return false;
    }
 
    return true;
}

bool AppControl::Start()
{  
    _session->Open(); 
    return true;
}

 void AppControl::Stop()
 {
    _session->Close();  
 }

void AppControl::UnInit()
{  
    // Destroy libsigrokdecode
    srd_exit();

    _session->uninit();
}

const char *AppControl::GetLastError()
{
    return m_error.c_str();
}

 bool AppControl::TopWindowIsMaximized()
 {
     if (_topWindow != NULL){
         return _topWindow->isMaximized();
     }
     return false;
 }

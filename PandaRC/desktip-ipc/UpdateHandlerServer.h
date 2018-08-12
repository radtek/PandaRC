// Copyright (C) 2009,2010,2011,2012 GlavSoft LLC.
// All rights reserved.
//
//-------------------------------------------------------------------------
// This file is part of the TightVNC software.  Please visit our Web site:
//
//                       http://www.tightvnc.com/
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//-------------------------------------------------------------------------
//

#ifndef __UPDATEHANDLERSERVER_H__
#define __UPDATEHANDLERSERVER_H__

#include "DesktopServerProto.h"
#include "desktop/UpdateHandlerImpl.h"
#include "desktop/Win32ScreenDriverFactory.h"

#include <QtWidgets/qwidget.h>

class UpdateHandlerServer : public UpdateListener
{
public:
	UpdateHandlerServer(QWidget* parent);
	virtual ~UpdateHandlerServer();

	std::vector<FrameBuffer>& getUpdateFrameList();

	// Internal dispatcher
	virtual void onRequest(UINT8 reqCode);

protected:
	virtual void onUpdate();

	// At first time server must get init information.
	void serverInit();

	void extractReply();
	void screenPropReply();
	void receiveFullReqReg();
	void receiveExcludingReg();

	Win32ScreenDriverFactory m_scrDriverFactory;

	PixelFormat m_oldPf;
	UpdateHandlerImpl *m_updateHandler;

	QWidget* m_parent;
};

#endif // __UPDATEHANDLERSERVER_H__

// Copyright (C) 2012 GlavSoft LLC.
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

#include "Win32ScreenDriverFactory.h"
#include "Win32MirrorScreenDriver.h"
#include "Win32ScreenDriver.h"
#include "Win8ScreenDriver.h"

Win32ScreenDriverFactory::Win32ScreenDriverFactory()
{
}

Win32ScreenDriverFactory::~Win32ScreenDriverFactory()
{
}

ScreenDriver *Win32ScreenDriverFactory::createScreenDriver(UpdateKeeper *updateKeeper, UpdateListener *updateListener, FrameBuffer* fb, LocalMutex*  fbLocalMutex)
{
	// Try to use Win8 duplication API firstly because it's in preference to other methods.
	//return new Win8ScreenDriver();

	if (isMirrorDriverAllowed()) {
		printf("Mirror driver usage is allowed, try to start it...");
		return createMirrorScreenDriver(updateKeeper, updateListener, fbLocalMutex);
	}
	else
	{
		printf("Mirror driver usage is disallowed");
	}
	printf("Using the standart screen driver");
	return createStandardScreenDriver(updateKeeper, updateListener, fb, fbLocalMutex);
}

ScreenDriver *Win32ScreenDriverFactory::createStandardScreenDriver(UpdateKeeper *updateKeeper, UpdateListener *updateListener, FrameBuffer *fb, LocalMutex *fbLocalMutex)
{
	return new Win32ScreenDriver(updateKeeper, updateListener, fb, fbLocalMutex);
}

ScreenDriver *Win32ScreenDriverFactory::createMirrorScreenDriver(UpdateKeeper *updateKeeper, UpdateListener *updateListener, LocalMutex*  fbLocalMutex)
{
	return new Win32MirrorScreenDriver(updateKeeper, updateListener, fbLocalMutex);
}

bool Win32ScreenDriverFactory::isMirrorDriverAllowed()
{
	return true;
}

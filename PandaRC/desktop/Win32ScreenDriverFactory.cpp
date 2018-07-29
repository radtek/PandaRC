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

ScreenDriver *Win32ScreenDriverFactory::createScreenDriver()
{
	// Try to use Win8 duplication API firstly because it's in preference to other methods.
	//return new Win8ScreenDriver();

	if (isMirrorDriverAllowed()) {
		printf("Mirror driver usage is allowed, try to start it...");
		return createMirrorScreenDriver();
	}
	else
	{
		printf("Mirror driver usage is disallowed");
	}
	printf("Using the standart screen driver");
	return createStandardScreenDriver();
}

ScreenDriver *Win32ScreenDriverFactory::createStandardScreenDriver()
{
	return new Win32ScreenDriver();
}

ScreenDriver *Win32ScreenDriverFactory::createMirrorScreenDriver()
{
	return new Win32MirrorScreenDriver();
}

bool Win32ScreenDriverFactory::isMirrorDriverAllowed()
{
	return true;
}

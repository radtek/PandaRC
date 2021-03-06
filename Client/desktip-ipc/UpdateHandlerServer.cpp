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

#include "UpdateHandlerServer.h"
#include "PandaRC.h"

UpdateHandlerServer::UpdateHandlerServer(QWidget* parent)
{
	m_parent = parent;
	m_updateHandler = new UpdateHandlerImpl(this, &m_scrDriverFactory);

	//dispatcher->registerNewHandle(EXTRACT_REQ, this);
	//dispatcher->registerNewHandle(SCREEN_PROP_REQ, this);
	//dispatcher->registerNewHandle(SET_FULL_UPD_REQ_REGION, this);
	//dispatcher->registerNewHandle(SET_EXCLUDING_REGION, this);
	//dispatcher->registerNewHandle(FRAME_BUFFER_INIT, this);
}

UpdateHandlerServer::~UpdateHandlerServer()
{
	delete m_updateHandler;
}

void UpdateHandlerServer::onUpdate()
{
	const FrameBuffer* fb = m_updateHandler->getFrameBuffer();
	UpdateContainer updCont;
	m_updateHandler->extract(&updCont);

	std::vector<Rect> vecRect;
	std::vector<Rect>::iterator iterRect;
	updCont.changedRegion.getRectVector(&vecRect);

	for (iterRect = vecRect.begin(); iterRect < vecRect.end(); iterRect++) {
		Rect *rect = &(*iterRect);
		PDFRAME* pd = new PDFRAME((int8_t)PDEVENT_TYPE::eFRAME);
		pd->rect = *rect;
		pd->fb.setProperties(rect, &(fb->getPixelFormat()));
		pd->fb.copyFrom(fb, rect->left, rect->top);
		((PandaRC*)m_parent)->getFrameThread()->addServerFrame(pd);
	}

	vecRect.clear();
	updCont.copiedRegion.getRectVector(&vecRect);

	for (iterRect = vecRect.begin(); iterRect < vecRect.end(); iterRect++) {
		Rect *rect = &(*iterRect);
		PDFRAME* pd = new PDFRAME((int8_t)PDEVENT_TYPE::eFRAME);
		pd->rect = *rect;
		pd->fb.setProperties(rect, &(fb->getPixelFormat()));
		pd->fb.copyFrom(fb, rect->left, rect->top);
		((PandaRC*)m_parent)->getFrameThread()->addServerFrame(pd);
	}

	// Send cursor position if it has been changed.
	PDCURSOR* cursor = NULL;
	if (updCont.cursorPosChanged)
	{
		cursor = new PDCURSOR(PDEVENT_TYPE::eCURSOR);
		cursor->pos = updCont.cursorPos;
	}

	// Send cursor shape if it has been changed.
	if (updCont.cursorShapeChanged)
	{
		if (cursor == NULL)
		{
			cursor = new PDCURSOR(PDEVENT_TYPE::eCURSOR);
		}
		const CursorShape *curSh = m_updateHandler->getCursorShape();
		cursor->dim = curSh->getDimension();
		cursor->hotspot = curSh->getHotSpot();
		memcpy(cursor->buffer, curSh->getPixels()->getBuffer(), curSh->getPixelsSize());
		cursor->bufferSize = curSh->getPixelsSize();
		if (curSh->getMaskSize()) {
			memcpy(cursor->mask, curSh->getMask(), curSh->getMaskSize());
			cursor->maskSize = curSh->getMaskSize();
		}
	}
	if (cursor != NULL)
	{
		((PandaRC*)m_parent)->getFrameThread()->addServerFrame(cursor);
	}

	//AutoLock al(m_forwGate);
	//try {
	//  m_forwGate->writeUInt8(UPDATE_DETECTED);
	//} catch (Exception &e) {
	//  glog.error(_T("An error has been occurred while sending the")
	//               _T(" UPDATE_DETECTED message from UpdateHandlerServer: %s"),
	//             e.getMessage());
	//  m_extTerminationListener->onAnObjectEvent();
	//}
}

void UpdateHandlerServer::onRequest(UINT8 reqCode)
{
	//extractReply();

	//switch (reqCode) {
	//case EXTRACT_REQ:
	//  extractReply(backGate);
	//  break;
	//case SCREEN_PROP_REQ:
	//  screenPropReply(backGate);
	//  break;
	//case SET_FULL_UPD_REQ_REGION:
	//  receiveFullReqReg(backGate);
	//  break;
	//case SET_EXCLUDING_REGION:
	//  receiveExcludingReg(backGate);
	//  break;
	//case FRAME_BUFFER_INIT:
	//  // Init from client
	//  serverInit(backGate);
	//  break;
	//default:
	//  StringStorage errMess;
	//  errMess.format(_T("Unknown %d protocol code received from a pipe client"),
	//                 (int)reqCode);
	//  throw Exception(errMess.getString());
	//  break;
	//}
}

void UpdateHandlerServer::extractReply()
{
	UpdateContainer updCont;
	m_updateHandler->extract(&updCont);

	const FrameBuffer *fb = m_updateHandler->getFrameBuffer();

	PixelFormat newPf = fb->getPixelFormat();

	if (!m_oldPf.isEqualTo(&newPf)) {
		updCont.screenSizeChanged = true;
		m_oldPf = newPf;
	}

	PDFRAME* pd = new PDFRAME(PDEVENT_TYPE::eFRAME);
	pd->rect = fb->getDimension().getRect();
	pd->fb.setProperties(&pd->rect, &(fb->getPixelFormat()));
	pd->fb.copyFrom(fb, pd->rect.left, pd->rect.top);
	((PandaRC*)m_parent)->getFrameThread()->addServerFrame(pd);

	//backGate->writeUInt8(updCont.screenSizeChanged);
	//if (updCont.screenSizeChanged) {
	//  // Send new screen properties
	//  sendPixelFormat(&newPf, backGate);
	//  Dimension fbDim = fb->getDimension();
	//  Rect fbRect = fbDim.getRect();
	//  sendDimension(&fbDim, backGate);
	//  sendFrameBuffer(fb, &fbRect, backGate);
	//}

	//// Send video region
	//sendRegion(&updCont.videoRegion, backGate);
	//// Send changed region
	//std::vector<Rect> rects;
	//std::vector<Rect>::iterator iRect;
	//updCont.changedRegion.getRectVector(&rects);
	//unsigned int countChangedRect = (unsigned int)rects.size();
	//_ASSERT(countChangedRect == rects.size());
	//backGate->writeUInt32(countChangedRect);

	//for (iRect = rects.begin(); iRect < rects.end(); iRect++) {
	//  Rect *rect = &(*iRect);
	  //sendRect(rect, backGate);
	//  sendFrameBuffer(fb, rect, backGate);
	//}

	//// Send "copyrect"
	//bool hasCopyRect = !updCont.copiedRegion.isEmpty();
	//backGate->writeUInt8(hasCopyRect);
	//if (hasCopyRect) {
	//  sendPoint(&updCont.copySrc, backGate);
	//  updCont.copiedRegion.getRectVector(&rects);
	//  iRect = rects.begin();
	//  sendRect(&(*iRect), backGate);
	//  sendFrameBuffer(fb, &(*iRect), backGate);
	//}

	//// Send cursor position if it has been changed.
	//backGate->writeUInt8(updCont.cursorPosChanged);
	//sendPoint(&updCont.cursorPos, backGate);

	//// Send cursor shape if it has been changed.
	//backGate->writeUInt8(updCont.cursorShapeChanged);
	//if (updCont.cursorShapeChanged) {
	//  const CursorShape *curSh = m_updateHandler->getCursorShape();
	//  sendDimension(&curSh->getDimension(), backGate);
	//  sendPoint(&curSh->getHotSpot(), backGate);

	//  // Send pixels
	//  backGate->writeFully(curSh->getPixels()->getBuffer(), curSh->getPixelsSize());
	//  // Send mask
	//  if (curSh->getMaskSize()) {
	//    backGate->writeFully((void *)curSh->getMask(), curSh->getMaskSize());
	//  }
	//}
}

void UpdateHandlerServer::screenPropReply()
{
	const FrameBuffer *fb = m_updateHandler->getFrameBuffer();
	//sendPixelFormat(&fb->getPixelFormat(), backGate);
	//sendDimension(&fb->getDimension(), backGate);
}

void UpdateHandlerServer::receiveFullReqReg()
{
	Region region;
	//readRegion(&region, backGate);
	//m_updateHandler->setFullUpdateRequested(&region);
}

void UpdateHandlerServer::receiveExcludingReg()
{
	Region region;
	//readRegion(&region, backGate);
	//m_updateHandler->setExcludedRegion(&region);
}

void UpdateHandlerServer::serverInit()
{
	// FIXME: Use another method to initialize m_backupFrameBuffer
	// because this method use a lot of memory.
	//FrameBuffer fb;
	//readPixelFormat(&m_oldPf, backGate);
	//Dimension dim = readDimension(backGate);
	//fb.setProperties(&dim, &m_oldPf);

	//readFrameBuffer(&fb, &dim.getRect(), backGate);
	//m_updateHandler->initFrameBuffer(&fb);
}

//// copyright (c) 2011,2012 glavsoft llc.
//// all rights reserved.
////
////-------------------------------------------------------------------------
//// this file is part of the tightvnc software.  please visit our web site:
////
////                       http://www.tightvnc.com/
////
//// this program is free software; you can redistribute it and/or modify
//// it under the terms of the gnu general public license as published by
//// the free software foundation; either version 2 of the license, or
//// (at your option) any later version.
////
//// this program is distributed in the hope that it will be useful,
//// but without any warranty; without even the implied warranty of
//// merchantability or fitness for a particular purpose.  see the
//// gnu general public license for more details.
////
//// you should have received a copy of the gnu general public license along
//// with this program; if not, write to the free software foundation, inc.,
//// 51 franklin street, fifth floor, boston, ma 02110-1301 usa.
////-------------------------------------------------------------------------
////
//
//#include "win8screendriver.h"
//#include "util/exception.h"
//
//win8screendriver::win8screendriver() : m_detectionenabled(false)
//{
//  // fixme: this class is not provide thread safety for common usage case but for the updatehandlerimpl
//  // usage case it provides. to fix this issue is needed to think to introduce a mutex for m_drvimpl.
//  m_drvimpl = new win8screendriverimpl();
//}
//
//win8screendriver::~win8screendriver()
//{
//  terminatedetection();
//
//  delete m_drvimpl;
//}
//
//void win8screendriver::executedetection()
//{
//  m_detectionenabled = true;
//  m_drvimpl->executedetection();
//}
//
//void win8screendriver::terminatedetection()
//{
//  m_detectionenabled = false;
//  m_drvimpl->terminatedetection();
//}
//
//dimension win8screendriver::getscreendimension()
//{
//  return m_drvimpl->getscreenbuffer()->getdimension();
//}
//
//framebuffer *win8screendriver::getscreenbuffer()
//{
//  return m_drvimpl->getscreenbuffer();
//}
//
//bool win8screendriver::grabfb(const rect *rect)
//{
//  return m_drvimpl->grabfb(rect);
//}
//
//bool win8screendriver::getscreenpropertieschanged()
//{
//  return !m_drvimpl->isvalid();
//}
//
//bool win8screendriver::getscreensizechanged()
//{
//  return !m_drvimpl->isvalid();
//}
//
//bool win8screendriver::applynewscreenproperties()
//{
//  try {
//    win8screendriverimpl *drvimpl =
//      new win8screendriverimpl(glog, m_updatekeeper, m_fblocalmutex, m_updatelistener, m_detectionenabled);
//    delete m_drvimpl;
//    m_drvimpl = drvimpl;
//  } catch (exception &e) {
//    glog.error(_t("can't apply new screen properties: %s"), e.getmessage());
//    return false;
//  }
//  return true;
//}
//
//bool win8screendriver::grabcursorshape(const pixelformat *pf)
//{
//  m_drvimpl->updatecursorshape(&m_cursorshape);
//  return !m_drvimpl->isvalid();
//}
//
//const cursorshape *win8screendriver::getcursorshape()
//{
//  return &m_cursorshape;
//}
//
//point win8screendriver::getcursorposition()
//{
//  return m_drvimpl->getcursorposition();
//}
//
//void win8screendriver::getcopiedregion(rect *copyrect, point *source)
//{
//  copyrect->clear();
//  source->clear();
//}

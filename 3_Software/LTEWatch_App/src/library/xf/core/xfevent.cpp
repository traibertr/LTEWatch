/* ******************************************************************************************************************
 * Copyright (C) Hes-so VALAIS/WALLIS, HEI Sion, Infotronics. 2022
 * Created by Patrice Rudaz & Thomas Sterren
 * All rights reserved.
 * **************************************************************************************************************** */

/* **************************************************************************************************************** */
/* Header files                                                                                                     */
/* **************************************************************************************************************** */
#include "xfevent.h"

#if (XF_USE_MEMORYPOOL_FOR_DEFAULTEVENTS != 0)
    IMPLEMENT_MEMORYPOOL_OPERATORS(XFEvent);
#endif // XF_USE_MEMORYPOOL_FOR_DEFAULTEVENTS

/* **************************************************************************************************************** */
/* CONSTRUCTOR                                                                                                      */
/* **************************************************************************************************************** */
// ------------------------------------------------------------------------------------------------------------------
XFEvent::XFEvent(uint8_t id, IXFReactive* pBehaviour, bool deleteMe) 
    : IXFEvent(IXFEvent::Event, id, pBehaviour, deleteMe)
{}

// ------------------------------------------------------------------------------------------------------------------
XFEvent::~XFEvent()
{}
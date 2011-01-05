/*
 * sipim.h
 *
 * Support for SIP session mode IM
 *
 * Open Phone Abstraction Library (OPAL)
 *
 * Copyright (c) 2008 Post Increment
 *
 * The contents of this file are subject to the Mozilla Public License
 * Version 1.0 (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS"
 * basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See
 * the License for the specific language governing rights and limitations
 * under the License.
 *
 * The Original Code is Open Phone Abstraction Library.
 *
 * The Initial Developer of the Original Code is Post Increment
 *
 * Contributor(s): ______________________________________.
 *
 * $Revision$
 * $Author$
 * $Date$
 */

#ifndef OPAL_IM_SIPIM_H
#define OPAL_IM_SIPIM_H

#include <ptlib.h>
#include <opal/buildopts.h>
#include <opal/rtpconn.h>
#include <opal/manager.h>
#include <opal/mediastrm.h>
#include <opal/mediatype.h>
#include <opal/mediatype.h>
#include <im/im.h>
#include <sip/sdp.h>
#include <sip/sippdu.h>

#if OPAL_HAS_SIPIM

class OpalSIPIMMediaType : public OpalIMMediaType 
{
  public:
    OpalSIPIMMediaType();
    virtual OpalMediaSession * CreateMediaSession(OpalConnection & conn, unsigned sessionID) const;

    SDPMediaDescription * CreateSDPMediaDescription(const OpalTransportAddress & localAddress);
};

////////////////////////////////////////////////////////////////////////////

/** Class for carrying MSRP session information
  */
class OpalSIPIMMediaSession : public OpalMediaSession
{
  PCLASSINFO(OpalSIPIMMediaSession, OpalMediaSession);
  public:
    OpalSIPIMMediaSession(OpalConnection & connection, unsigned sessionId);
    OpalSIPIMMediaSession(const OpalSIPIMMediaSession & _obj);

    virtual bool Open() { return true; }

    virtual void Close() { }

    virtual PObject * Clone() const { return new OpalSIPIMMediaSession(*this); }

    virtual bool IsActive() const { return true; }

    virtual bool IsRTP() const { return false; }

    virtual bool HasFailed() const { return false; }

    virtual OpalTransportAddress GetLocalMediaAddress() const;

    virtual void SetRemoteMediaAddress(const OpalTransportAddress &, const OpalMediaFormatList & );

    virtual SDPMediaDescription * CreateSDPMediaDescription(
      const OpalTransportAddress & localAddress
    );

    virtual OpalMediaStream * CreateMediaStream(
      const OpalMediaFormat & mediaFormat, 
      unsigned sessionID, 
      PBoolean isSource
    );

    virtual PString GetCallID() const { return callId; }

  protected:
    OpalTransportAddress transportAddress;
    PString localURL;
    PString remoteURL;
    PString callId;
};

////////////////////////////////////////////////////////////////////////////

class OpalSIPIMContext : public OpalConnectionIMContext
{
  public:
    OpalSIPIMContext();

  protected:
    virtual SentStatus InternalSendOutsideCall(OpalIM * message);
    virtual SentStatus InternalSendInsideCall(OpalIM * message);

    virtual bool OnIncomingIM(OpalIM & message);
    void OnCompositionIndicationTimeout();

    PDECLARE_NOTIFIER(PTimer, OpalSIPIMContext, OnCompositionTimerExpire);

    void PopulateParams(SIPMessage::Params & params, OpalIM & message);

    PTimer m_rxCompositionTimeout;

    RFC4103Context m_rfc4103Context;
};


////////////////////////////////////////////////////////////////////////////

#endif // OPAL_HAS_SIPIM

#endif // OPAL_IM_SIPIM_H

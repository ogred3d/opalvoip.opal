/*
 * h323rtp.cxx
 *
 * H.323 RTP protocol handler
 *
 * Open H323 Library
 *
 * Copyright (c) 1999-2000 Equivalence Pty. Ltd.
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
 * The Original Code is Open H323 Library.
 *
 * The Initial Developer of the Original Code is Equivalence Pty. Ltd.
 *
 * Contributor(s): ______________________________________.
 *
 * $Revision$
 * $Author$
 * $Date$
 */

#include <ptlib.h>

#include <opal/buildopts.h>
#if OPAL_H323

#ifdef __GNUC__
#pragma implementation "h323rtp.h"
#endif

#include <h323/h323rtp.h>

#include <opal/manager.h>
#include <h323/h323ep.h>
#include <h323/h323con.h>
#include <h323/h323pdu.h>
#include <h323/transaddr.h>
#include <asn/h225.h>
#include <asn/h245.h>


#define new PNEW


/////////////////////////////////////////////////////////////////////////////

H323RTPSession::H323RTPSession(H323Connection & conn, unsigned sessionId, const OpalMediaType & mediaType)
  : OpalRTPSession(conn, sessionId, mediaType)
  , connection(conn)
{
}


PBoolean H323RTPSession::OnSendingPDU(const H323_RTPChannel & channel,
                                    H245_H2250LogicalChannelParameters & param) const
{
  PTRACE(3, "RTP\tOnSendingPDU");

  param.IncludeOptionalField(H245_H2250LogicalChannelParameters::e_mediaGuaranteedDelivery);
  param.m_mediaGuaranteedDelivery = PFalse;

  // unicast must have mediaControlChannel
  H323TransportAddress mediaControlAddress(GetLocalAddress(), GetLocalControlPort());
  param.IncludeOptionalField(H245_H2250LogicalChannelParameters::e_mediaControlChannel);
  mediaControlAddress.SetPDU(param.m_mediaControlChannel);

  if (channel.GetDirection() == H323Channel::IsReceiver) {
    // set mediaChannel
    H323TransportAddress mediaAddress(GetLocalAddress(), GetLocalDataPort());
    param.IncludeOptionalField(H245_H2250LogicalChannelAckParameters::e_mediaChannel);
    mediaAddress.SetPDU(param.m_mediaChannel);
  }
  else {
    // Set flag for we are going to stop sending audio on silence
    OpalMediaStreamPtr mediaStream = channel.GetMediaStream();
    if (mediaStream != NULL) {
      param.IncludeOptionalField(H245_H2250LogicalChannelParameters::e_silenceSuppression);
      param.m_silenceSuppression = (connection.GetEndPoint ().GetManager ().GetSilenceDetectParams ().m_mode != OpalSilenceDetector::NoSilenceDetection);
    }
  }

  // Set dynamic payload type, if is one
  RTP_DataFrame::PayloadTypes rtpPayloadType = channel.GetDynamicRTPPayloadType();
  if (rtpPayloadType >= RTP_DataFrame::DynamicBase && rtpPayloadType < RTP_DataFrame::IllegalPayloadType) {
    param.IncludeOptionalField(H245_H2250LogicalChannelParameters::e_dynamicRTPPayloadType);
    param.m_dynamicRTPPayloadType = (int)rtpPayloadType;
  }

  // Set the media packetization field if have an option to describe it.
  param.m_mediaPacketization.SetTag(H245_H2250LogicalChannelParameters_mediaPacketization::e_rtpPayloadType);
  if (H323SetRTPPacketization(param.m_mediaPacketization, channel.GetMediaStream()->GetMediaFormat(), rtpPayloadType))
    param.IncludeOptionalField(H245_H2250LogicalChannelParameters::e_mediaPacketization);

  return PTrue;
}


void H323RTPSession::OnSendingAckPDU(const H323_RTPChannel & channel,
                                   H245_H2250LogicalChannelAckParameters & param) const
{
  PTRACE(3, "RTP\tOnSendingAckPDU");

  // set mediaControlChannel
  H323TransportAddress mediaControlAddress(GetLocalAddress(), GetLocalControlPort());
  param.IncludeOptionalField(H245_H2250LogicalChannelAckParameters::e_mediaControlChannel);
  mediaControlAddress.SetPDU(param.m_mediaControlChannel);

  // set mediaChannel
  H323TransportAddress mediaAddress(GetLocalAddress(), GetLocalDataPort());
  param.IncludeOptionalField(H245_H2250LogicalChannelAckParameters::e_mediaChannel);
  mediaAddress.SetPDU(param.m_mediaChannel);

  // Set dynamic payload type, if is one
  int rtpPayloadType = channel.GetDynamicRTPPayloadType();
  if (rtpPayloadType >= RTP_DataFrame::DynamicBase && rtpPayloadType < RTP_DataFrame::IllegalPayloadType) {
    param.IncludeOptionalField(H245_H2250LogicalChannelAckParameters::e_dynamicRTPPayloadType);
    param.m_dynamicRTPPayloadType = rtpPayloadType;
  }
}


PBoolean H323RTPSession::ExtractTransport(const H245_TransportAddress & pdu,
                                    PBoolean isDataPort,
                                    unsigned & errorCode)
{
  if (pdu.GetTag() != H245_TransportAddress::e_unicastAddress) {
    PTRACE(1, "RTP_UDP\tOnly unicast supported at this time");
    errorCode = H245_OpenLogicalChannelReject_cause::e_multicastChannelNotAllowed;
    return PFalse;
  }

  H323TransportAddress transAddr = pdu;

  if (isDataPort ? SetRemoteMediaAddress(transAddr) : SetRemoteControlAddress(transAddr))
    return true;

  PTRACE(1, "RTP_UDP\tIllegal IP address/port in transport address.");
  return PFalse;
}


PBoolean H323RTPSession::OnReceivedPDU(H323_RTPChannel & channel,
                                 const H245_H2250LogicalChannelParameters & param,
                                 unsigned & errorCode)
{
  PBoolean ok = PFalse;

  if (param.HasOptionalField(H245_H2250LogicalChannelParameters::e_mediaControlChannel)) {
    if (!ExtractTransport(param.m_mediaControlChannel, PFalse, errorCode)) {
      PTRACE(1, "RTP_UDP\tFailed to extract mediaControl transport for " << channel);
      return PFalse;
    }
    ok = PTrue;
  }

  if (param.HasOptionalField(H245_H2250LogicalChannelParameters::e_mediaChannel)) {
    if (ok && channel.GetDirection() == H323Channel::IsReceiver)
      PTRACE(2, "RTP_UDP\tIgnoring media transport for " << channel);
    else if (!ExtractTransport(param.m_mediaChannel, PTrue, errorCode)) {
      PTRACE(1, "RTP_UDP\tFailed to extract media transport for " << channel);
      return PFalse;
    }
    ok = PTrue;
  }

  if (param.HasOptionalField(H245_H2250LogicalChannelParameters::e_dynamicRTPPayloadType))
    channel.SetDynamicRTPPayloadType(param.m_dynamicRTPPayloadType);

  PString mediaPacketization;
  if (param.HasOptionalField(H245_H2250LogicalChannelParameters::e_mediaPacketization) &&
      param.m_mediaPacketization.GetTag() == H245_H2250LogicalChannelParameters_mediaPacketization::e_rtpPayloadType)
    mediaPacketization = H323GetRTPPacketization(param.m_mediaPacketization);

  OpalMediaFormat mediaFormat = channel.GetMediaStream()->GetMediaFormat();
  if (mediaFormat.GetOptionString(OpalMediaFormat::MediaPacketizationsOption()) != mediaPacketization ||
      mediaFormat.GetOptionString(OpalMediaFormat::MediaPacketizationOption())  != mediaPacketization) {
    mediaFormat.SetOptionString(OpalMediaFormat::MediaPacketizationsOption(), mediaPacketization);
    mediaFormat.SetOptionString(OpalMediaFormat::MediaPacketizationOption(), mediaPacketization);
    channel.GetMediaStream()->UpdateMediaFormat(mediaFormat);
  }

  if (ok)
    return PTrue;

  PTRACE(1, "RTP_UDP\tNo mediaChannel or mediaControlChannel specified for " << channel);

  if (GetSessionID() == H323Capability::DefaultDataSessionID)
    return PTrue;

  errorCode = H245_OpenLogicalChannelReject_cause::e_unspecified;
  return PFalse;
}


PBoolean H323RTPSession::OnReceivedAckPDU(H323_RTPChannel & channel,
                                    const H245_H2250LogicalChannelAckParameters & param)
{
  if (!param.HasOptionalField(H245_H2250LogicalChannelAckParameters::e_sessionID)) {
    PTRACE(1, "RTP_UDP\tNo session specified");
  }
  
  unsigned errorCode;

  if (param.HasOptionalField(H245_H2250LogicalChannelAckParameters::e_mediaControlChannel)) {
    if (!ExtractTransport(param.m_mediaControlChannel, PFalse, errorCode))
      return PFalse;
  }
  else {
    PTRACE(1, "RTP_UDP\tNo mediaControlChannel specified");
    if (GetSessionID() != H323Capability::DefaultDataSessionID)
      return PFalse;
  }

  if (!param.HasOptionalField(H245_H2250LogicalChannelAckParameters::e_mediaChannel)) {
    PTRACE(1, "RTP_UDP\tNo mediaChannel specified");
    return PFalse;
  }

  if (!ExtractTransport(param.m_mediaChannel, PTrue, errorCode))
    return PFalse;

  if (param.HasOptionalField(H245_H2250LogicalChannelAckParameters::e_dynamicRTPPayloadType))
    channel.SetDynamicRTPPayloadType(param.m_dynamicRTPPayloadType);

  return PTrue;
}


void H323RTPSession::OnSendRasInfo(H225_RTPSession & info)
{
  info.m_sessionId = GetSessionID();
  info.m_ssrc = GetSyncSourceOut();
  info.m_cname = GetCanonicalName();

  H323TransportAddress lda(GetLocalAddress(), GetLocalDataPort());
  lda.SetPDU(info.m_rtpAddress.m_recvAddress);
  H323TransportAddress rda(GetRemoteAddress(), GetRemoteDataPort());
  rda.SetPDU(info.m_rtpAddress.m_sendAddress);

  H323TransportAddress lca(GetLocalAddress(), GetLocalControlPort());
  lca.SetPDU(info.m_rtcpAddress.m_recvAddress);
  H323TransportAddress rca(GetRemoteAddress(), GetRemoteDataPort());
  rca.SetPDU(info.m_rtcpAddress.m_sendAddress);
}


#endif // OPAL_H323

/////////////////////////////////////////////////////////////////////////////

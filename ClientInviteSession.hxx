#if !defined(RESIP_CLIENTINVITESESSION_HXX)
#define RESIP_CLIENTINVITESESSION_HXX

#include "resiprocate/dum/InviteSession.hxx"
#include "resiprocate/dum/Handles.hxx"

namespace resip
{
class SipMessage;
class SdpContents;

class ClientInviteSession : public InviteSession
{
   public:
      ClientInviteSession(DialogUsageManager& dum,
                          Dialog& dialog,
                          const SipMessage& request,
                          const SdpContents* initialOffer,
                          ServerSubscriptionHandle serverSub = ServerSubscriptionHandle::NotValid());

      ClientInviteSessionHandle getHandle();

   public:
      virtual void provideOffer (const SdpContents& offer);
      virtual void provideAnswer (const SdpContents& answer);
      virtual void end ();
      virtual void reject (int statusCode);
      virtual void targetRefresh (const NameAddr& localUri);
      virtual void refer (const NameAddr& referTo);
      virtual void refer (const NameAddr& referTo, InviteSessionHandle sessionToReplace);
      virtual void info (const Contents& contents);

   private:
      virtual void dispatch(const SipMessage& msg);
      virtual void dispatch(const DumTimeout& timer);

      void dispatchStart (const SipMessage& msg);
      void dispatchEarly (const SipMessage& msg);
      void dispatchEarlyWithOffer (const SipMessage& msg);
      void dispatchEarlyWithAnswer (const SipMessage& msg);
      void dispatchWaitingForAnswerFromApp (const SipMessage& msg);
      void dispatchConnected (const SipMessage& msg);
      void dispatchTerminated (const SipMessage& msg);
      void dispatchSentUpdateEarly (const SipMessage& msg);
      void dispatchReceivedUpdateEarly (const SipMessage& msg);
      void dispatchPrackAnswerWait (const SipMessage& msg);
      void dispatchCanceled (const SipMessage& msg);

      // Called by the DialogSet (friend) when the app has CANCELed the request
      void cancel();

   private:
      std::auto_ptr<SdpContents> mEarlyMedia;
      
      int lastReceivedRSeq;
      int lastExpectedRSeq;
      int mStaleCallTimerSeq;
      ServerSubscriptionHandle mServerSub;

#if 0
      void redirected(const SipMessage& msg);
      void sendSipFrag(const SipMessage& response);
      void handlePrackResponse(const SipMessage& response);
      void sendPrack(const SipMessage& response);
#endif

      // disabled
      ClientInviteSession(const ClientInviteSession&);
      ClientInviteSession& operator=(const ClientInviteSession&);

      friend class Dialog;
};

}

#endif

/* ====================================================================
 * The Vovida Software License, Version 1.0
 *
 * Copyright (c) 2000 Vovida Networks, Inc.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the

 *    distribution.
 *
 * 3. The names "VOCAL", "Vovida Open Communication Application Library",
 *    and "Vovida Open Communication Application Library (VOCAL)" must
 *    not be used to endorse or promote products derived from this
 *    software without prior written permission. For written
 *    permission, please contact vocal@vovida.org.
 *
 * 4. Products derived from this software may not be called "VOCAL", nor
 *    may "VOCAL" appear in their name, without prior written
 *    permission of Vovida Networks, Inc.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESSED OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, TITLE AND
 * NON-INFRINGEMENT ARE DISCLAIMED.  IN NO EVENT SHALL VOVIDA
 * NETWORKS, INC. OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT DAMAGES
 * IN EXCESS OF $1,000, NOR FOR ANY INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
 *
 * ====================================================================
 *
 * This software consists of voluntary contributions made by Vovida
 * Networks, Inc. and many individuals on behalf of Vovida Networks,
 * Inc.  For more information on Vovida Networks, Inc., please see
 * <http://www.vovida.org/>.
 *
 */

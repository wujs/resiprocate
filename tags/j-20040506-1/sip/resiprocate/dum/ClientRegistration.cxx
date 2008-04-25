#include "resiprocate/Helper.hxx"

#include "BaseCreator.hxx"
#include "ClientRegistration.hxx"
#include "RegistrationHandler.hxx"
#include "DialogUsageManager.hxx"
#include "Dialog.hxx"
#include "Profile.hxx"

using namespace resip;

ClientRegistration::ClientRegistration(DialogUsageManager& dum,
                                       Dialog& dialog,
                                       const SipMessage& request)
   : BaseUsage(dum, dialog),
     mHandle(dum),
     mLastRequest(request)
{
   if (mLastRequest.exists(h_Contacts))
   {
      mMyContacts = mLastRequest.header(h_Contacts);
   }
}

void 
ClientRegistration::addBinding(const NameAddr& contact)
{
   mMyContacts.push_back(contact);
   mLastRequest.header(h_Contacts) = mMyContacts;
   mLastRequest.header(h_Expires).value() = mDum.getProfile()->getDefaultRegistrationTime();
   mLastRequest.header(h_CSeq).sequence()++;
   // caller prefs
   mDum.send(mLastRequest);
}

void 
ClientRegistration::removeBinding(const NameAddr& contact)
{
   for (NameAddrs::iterator i=mMyContacts.begin(); i != mMyContacts.end(); i++)
   {
      if (i->uri() == contact.uri())
      {
         mMyContacts.erase(i);

         mLastRequest.header(h_Contacts) = mMyContacts;
         mLastRequest.header(h_Expires).value() = mDum.getProfile()->getDefaultRegistrationTime();
         mLastRequest.header(h_CSeq).sequence()++;
         mDum.send(mLastRequest);

         return;
      }
   }

   throw Exception("No such binding", __FILE__, __LINE__);
}

void 
ClientRegistration::removeAll()
{
   mAllContacts.clear();
   mMyContacts.clear();
   
   NameAddr all;
   all.setAllContacts();
   mLastRequest.header(h_Contacts).clear();
   mLastRequest.header(h_Contacts).push_back(all);
   mLastRequest.header(h_Expires).value() = 0;
   mLastRequest.header(h_CSeq).sequence()++;
   mDum.send(mLastRequest);
}

void 
ClientRegistration::removeMyBindings()
{
   for (NameAddrs::iterator i=mMyContacts.begin(); i != mMyContacts.end(); i++)
   {
      i->param(p_expires) = 0;
   }
   // !jf! might want to remove from mAllContacts
   mLastRequest.header(h_Contacts) = mMyContacts;
   mLastRequest.remove(h_Expires);
   mLastRequest.header(h_CSeq).sequence()++;
   mDum.send(mLastRequest);
}

void 
ClientRegistration::requestRefresh()
{
   mLastRequest.header(h_CSeq).sequence()++;
   mDum.send(mLastRequest);
   mLastRequest.header(h_Expires).value() = mDum.getProfile()->getDefaultRegistrationTime();
   mDum.addTimer(DumTimer::Registration, Helper::aBitSmallerThan(mLastRequest.header(h_Expires).value()), mLastRequest.header(h_CSeq).sequence());
}

const NameAddrs& 
ClientRegistration::myContacts()
{
   return mMyContacts;
}

const NameAddrs& 
ClientRegistration::allContacts()
{
   return mAllContacts;
}

void
ClientRegistration::updateMyContacts(const NameAddrs& allContacts)
{
   NameAddrs myNewContacts;
   for (NameAddrs::iterator i=allContacts.begin(); i != allContacts.end(); i++)
   {
      for (NameAddrs::iterator j=mMyContacts.begin(); j != mMyContacts.end(); i++)
      {
         if (i->uri() == j->uri())
         {
            if (i->exists(p_gruu))
            {
               mDum.getProfile()->addGruu(mLastRequest.header(h_To).uri().getAor(), *i);
            }
            myNewContacts.push_back(*i);
         }
      }
   }
   mMyContacts = myNewContacts;
}


void 
ClientRegistration::dispatch(const SipMessage& msg)
{
   // !jf! there may be repairable errors that we can handle here
   assert(msg.isResponse());
   int& code = msg.header(h_StatusLine).statusCode();
   if (code < 200)
   {
      // throw it away
   }
   else if (code < 300) // success
   {
      Profile* profile = mDum.getProfile();
      
      // !jf! consider what to do if no contacts
      mAllContacts = msg.header(h_Contacts);
      updateMyContacts(mAllContacts);
      if (!mMyContacts.empty())
      {
         // make timers to re-register
         mDum.addTimer(DumTimer::Registration, 
                       Helper::aBitSmallerThan(mLastRequest.header(h_Expires).value()), 
                       mLastRequest.header(h_CSeq).sequence());
      }

      mDum.mClientRegistrationHandler->onSuccess(getHandle(), msg);
   }
   else
   {
      if (code == 423) // interval too short
      {
         // maximum 1 day 
         if (msg.exists(h_MinExpires) && msg.header(h_MinExpires).value()  < 86400) 
         {
            mLastRequest.header(h_Expires).value() = msg.header(h_MinExpires).value();
            mLastRequest.header(h_CSeq).sequence()++;
            mDum.send(mLastRequest);
            return;
         }
      }
      mDum.mClientRegistrationHandler->onFailure(getHandle(), msg);      
   }
}

void
ClientRegistration::dispatch(const DumTimer& timer)
{
   if (mLastRequest.header(h_CSeq).sequence() == timer.cseq())
   {
      if (!mMyContacts.empty())
      {
         requestRefresh();
      }
   }
}

ClientRegistration::Handle::Handle(DialogUsageManager& dum)
   : BaseUsage::Handle(dum)
{
}

ClientRegistration* 
ClientRegistration::Handle::operator->()
{
   return static_cast<ClientRegistration*>(get());
}

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
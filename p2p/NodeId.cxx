#include <algorithm>

#include "rutil/Data.hxx"
#include "p2p/NodeId.hxx"

using namespace p2p;

NodeId::NodeId(const resip::Data& data) 
{
   *this = data;
}

NodeId& NodeId::operator=(const resip::Data& data)
{
   std::copy(data.begin(), data.end(), &mValue[0]);
   return *this;
}

NodeId& NodeId::operator=(const NodeId& rhs)
{
   std::copy(&rhs.mValue[0], &rhs.mValue[16], &mValue[0]);
   return *this;
}

bool 
NodeId::operator<(const NodeId &r) const
{
   return memcmp(mValue, r.mValue, 16) < 0;
}

bool
NodeId::operator<=(const NodeId& rhs) const
{
   return *this == rhs || *this < rhs;
}

bool
NodeId::operator==(const NodeId& r) const
{
   return memcmp(mValue, r.mValue, 16) == 0;   
}

const resip::Data
NodeId::getValue() const
{
   resip::Data result(resip::Data::Share, (char*)mValue, 16);
   return result;
}


NodeId::NodeId()
{ 
   for ( int i=0; i<16; i++ )
   {
      mValue[i] = 0;
   }
}


NodeId
DestinationId::asNodeId() const
{
   NodeId id;
   assert(0);
   return id;
}


NodeId
NodeId::add2Pow( int power ) const
{
   assert( power < 128 );
   assert( power > 0 );
   NodeId ret;
   ret = *this;
   
   int byte = 15-power/8;
   assert( byte >= 0 );
   assert( byte < 16 );
   
   int val = 1<<(power%8);
   assert( val >= 1 );
   assert( val <= 128 );
   
   ret.mValue[byte] += val;

   return ret;
}


}

=======
bool
DestinationId::isCompressed()const
{
   assert(0);
   return true;
}

bool
DestinationId::isDestinationId()const
{
   assert(0);
   return true;
}

bool
DestinationId::isNodeId()const
{
   assert(0);
   return true;
}

bool
DestinationId::isResourceId()const
{
   assert(0);
   return true;
}

bool
DestinationId::operator==(const NodeId& nid) const
{
   assert(0);
   return true;
}


>>>>>>> .r8002
/* ======================================================================
 *  Copyright (c) 2008, Various contributors to the Resiprocate project
 *  All rights reserved.
 *  
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *      - Redistributions of source code must retain the above copyright
 *        notice, this list of conditions and the following disclaimer.
 *
 *      - Redistributions in binary form must reproduce the above
 *        copyright notice, this list of conditions and the following
 *        disclaimer in the documentation and/or other materials
 *        provided with the distribution.
 *
 *      - The names of the project's contributors may not be used to
 *        endorse or promote products derived from this software without
 *        specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 *  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 *  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 *  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 *  IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 *  THE POSSIBILITY OF SUCH DAMAGE.
 *====================================================================== */

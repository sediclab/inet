//
// Copyright (C) 2005 Andras Varga
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//

#ifndef NAMTRACEWRITER_H
#define NAMTRACEWRITER_H

#include <omnetpp.h>
#include "INETDefs.h"

/**
 * Writes a nam trace file. See NED file for more info.
 *
 * @author Andras Varga
 */
class INET_API NAMTraceWriter : public cSimpleModule
{
  private:
    void traceInit();
    void tracePacket(const char event, cMessage *msg);

    std::filebuf *namfb;
    std::ostream *nams;

  public:
    Module_Class_Members(NAMTraceWriter, cSimpleModule, 0);
    virtual ~NAMTraceWriter() {}
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
};

#endif

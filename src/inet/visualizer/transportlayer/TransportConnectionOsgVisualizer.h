//
// Copyright (C) 2016 OpenSim Ltd.
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, see <http://www.gnu.org/licenses/>.
//

#ifndef __INET_TRANSPORTCONNECTIONOSGVISUALIZER_H
#define __INET_TRANSPORTCONNECTIONOSGVISUALIZER_H

#include "inet/visualizer/base/TransportConnectionVisualizerBase.h"
#include "inet/visualizer/networknode/NetworkNodeOsgVisualizer.h"

namespace inet {

namespace visualizer {

class INET_API TransportConnectionOsgVisualizer : public TransportConnectionVisualizerBase
{
#ifdef WITH_OSG
  protected:
    class INET_API OsgConnection : public Connection {
      public:
        osg::Node *sourceNode = nullptr;
        osg::Node *destinationNode = nullptr;

      public:
        OsgConnection(osg::Node *sourceNode, osg::Node *destinationNode, int sourceModuleId, int destinationModuleId, int count);
    };

  protected:
    NetworkNodeOsgVisualizer *networkNodeVisualizer = nullptr;

  protected:
    virtual void addConnection(const Connection *connection) override;
    virtual void removeConnection(const Connection *connection) override;

    virtual osg::Node *createConnectionEndNode(tcp::TCPConnection *connection) const;
    virtual const Connection *createConnection(cModule *source, cModule *destination, tcp::TCPConnection *tcpConnection) const override;

#else // ifdef WITH_OSG

  protected:
    virtual const Connection *createConnection(cModule *source, cModule *destination, tcp::TCPConnection *tcpConnection) const override { return nullptr; }

#endif
};

} // namespace visualizer

} // namespace inet

#endif // ifndef __INET_TRANSPORTCONNECTIONOSGVISUALIZER_H


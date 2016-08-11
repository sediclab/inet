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

#ifndef __INET_TRANSPORTCONNECTIONVISUALIZERBASE_H
#define __INET_TRANSPORTCONNECTIONVISUALIZERBASE_H

#include "inet/common/PatternMatcher.h"
#include "inet/transportlayer/tcp/TCPConnection.h"
#include "inet/visualizer/base/VisualizerBase.h"

namespace inet {

namespace visualizer {

class INET_API TransportConnectionVisualizerBase : public VisualizerBase, public cListener
{
  protected:
    class INET_API Connection {
      public:
        int sourceModuleId = -1;
        int destinationModuleId = -1;
        int count = -1;

      public:
        Connection(int sourceModuleId, int destinationModuleId, int count);
        virtual ~Connection() {}
    };

  protected:
    /** @name Parameters */
    //@{
    cModule *subscriptionModule = nullptr;
    PatternMatcher nodeMatcher;
    //@}

    std::vector<const Connection *> connections;

  protected:
    virtual void initialize(int stage) override;

    virtual const Connection *createConnection(cModule *source, cModule *destination, tcp::TCPConnection *tcpConnection) const = 0;
    virtual void addConnection(const Connection *connection);
    virtual void removeConnection(const Connection *connection);

  public:
    virtual void receiveSignal(cComponent *source, simsignal_t signal, cObject *object DETAILS_ARG) override;
};

} // namespace visualizer

} // namespace inet

#endif // ifndef __INET_TRANSPORTCONNECTIONVISUALIZERBASE_H


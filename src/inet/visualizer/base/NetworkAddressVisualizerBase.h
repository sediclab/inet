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

#ifndef __INET_NETWORKADDRESSVISUALIZERBASE_H
#define __INET_NETWORKADDRESSVISUALIZERBASE_H

#include "inet/common/PatternMatcher.h"
#include "inet/networklayer/common/InterfaceEntry.h"
#include "inet/networklayer/common/L3Address.h"
#include "inet/visualizer/base/VisualizerBase.h"

namespace inet {

namespace visualizer {

class INET_API NetworkAddressVisualizerBase : public VisualizerBase, public cListener
{
  protected:
    class INET_API CacheEntry {
      protected:
        int nodeId = -1;
        int interfaceId = -1;

      public:
        CacheEntry(int nodeId, int interfaceId);
    };

  protected:
    /** @name Parameters */
    //@{
    cModule *subscriptionModule = nullptr;
    PatternMatcher nodeMatcher;
    PatternMatcher interfaceMatcher;
    cFigure::Color fontColor;
    cFigure::Color backgroundColor;
    //@}

    std::map<std::pair<int, int>, CacheEntry *> cacheEntries;

  protected:
    virtual void initialize(int stage) override;

    virtual CacheEntry *createCacheEntry(cModule *networkNode, InterfaceEntry *interfaceEntry) = 0;
    virtual CacheEntry *ensureCacheEntry(cModule *networkNode, InterfaceEntry *interfaceEntry);
    virtual void updateNetworkAddress(cModule *networkNode, InterfaceEntry *interfaceEntry, CacheEntry *cacheEntry) = 0;

  public:
    virtual void receiveSignal(cComponent *source, simsignal_t signal, cObject *object DETAILS_ARG) override;
};

} // namespace visualizer

} // namespace inet

#endif // ifndef __INET_NETWORKADDRESSVISUALIZERBASE_H


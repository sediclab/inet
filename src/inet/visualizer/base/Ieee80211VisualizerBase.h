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

#ifndef __INET_IEEE80211VISUALIZERBASE_H
#define __INET_IEEE80211VISUALIZERBASE_H

#include "inet/common/PatternMatcher.h"
#include "inet/networklayer/common/InterfaceEntry.h"
#include "inet/visualizer/base/VisualizerBase.h"

namespace inet {

namespace visualizer {

class INET_API Ieee80211VisualizerBase : public VisualizerBase, public cListener
{
  protected:
    class INET_API CacheEntry {
      public:
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
    //@}

    std::map<std::pair<int, int>, CacheEntry *> cacheEntries;

  protected:
    virtual void initialize(int stage) override;

    virtual CacheEntry *createCacheEntry(cModule *networkNode, InterfaceEntry *interfaceEntry) = 0;
    virtual CacheEntry *getCacheEntry(cModule *networkNode, InterfaceEntry *interfaceEntry);
    virtual void addCacheEntry(CacheEntry *cacheEntry);
    virtual void removeCacheEntry(CacheEntry *cacheEntry);

  public:
    virtual void receiveSignal(cComponent *source, simsignal_t signal, cObject *object DETAILS_ARG) override;
};

} // namespace visualizer

} // namespace inet

#endif // ifndef __INET_IEEE80211VISUALIZERBASE_H


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

#include "inet/common/ModuleAccess.h"
#include "inet/common/NotifierConsts.h"
#include "inet/visualizer/base/Ieee80211VisualizerBase.h"

namespace inet {

namespace visualizer {

Ieee80211VisualizerBase::CacheEntry::CacheEntry(int nodeId, int interfaceId) :
    nodeId(nodeId),
    interfaceId(interfaceId)
{
}

void Ieee80211VisualizerBase::initialize(int stage)
{
    VisualizerBase::initialize(stage);
    if (!hasGUI()) return;
    if (stage == INITSTAGE_LOCAL) {
        subscriptionModule = *par("subscriptionModule").stringValue() == '\0' ? getSystemModule() : getModuleFromPar<cModule>(par("subscriptionModule"), this);
        subscriptionModule->subscribe(NF_L2_ASSOCIATED, this);
        subscriptionModule->subscribe(NF_L2_DISASSOCIATED, this);
        subscriptionModule->subscribe(NF_L2_AP_ASSOCIATED, this);
        subscriptionModule->subscribe(NF_L2_AP_DISASSOCIATED, this);
        nodeMatcher.setPattern(par("nodeFilter"), true, true, true);
    }
}

Ieee80211VisualizerBase::CacheEntry *Ieee80211VisualizerBase::getCacheEntry(cModule *networkNode, InterfaceEntry *interfaceEntry)
{
    auto key = std::pair<int, int>(networkNode->getId(), interfaceEntry->getInterfaceId());
    auto it = cacheEntries.find(key);
    return it == cacheEntries.end() ? nullptr : it->second;
}

void Ieee80211VisualizerBase::addCacheEntry(CacheEntry *cacheEntry)
{
    auto key = std::pair<int, int>(cacheEntry->nodeId, cacheEntry->interfaceId);
    cacheEntries[key] = cacheEntry;
}

void Ieee80211VisualizerBase::removeCacheEntry(CacheEntry *cacheEntry)
{
    cacheEntries.erase(cacheEntries.find(std::pair<int, int>(cacheEntry->nodeId, cacheEntry->interfaceId)));
}


void Ieee80211VisualizerBase::receiveSignal(cComponent *source, simsignal_t signal, cObject *object DETAILS_ARG)
{
    if (signal == NF_L2_ASSOCIATED) {
        auto networkNode = getContainingNode(check_and_cast<cModule *>(source));
        if (nodeMatcher.matches(networkNode->getFullPath().c_str())) {
            auto interfaceEntry = check_and_cast<InterfaceEntry *>(object);
            auto cacheEntry = createCacheEntry(networkNode, interfaceEntry);
            addCacheEntry(cacheEntry);
        }
    }
    else if (signal == NF_L2_DISASSOCIATED) {
        auto networkNode = getContainingNode(check_and_cast<cModule *>(source));
        if (nodeMatcher.matches(networkNode->getFullPath().c_str())) {
            auto interfaceEntry = check_and_cast<InterfaceEntry *>(object);
            auto cacheEntry = getCacheEntry(networkNode, interfaceEntry);
            removeCacheEntry(cacheEntry);
        }
    }
}

} // namespace visualizer

} // namespace inet


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
#include "inet/networklayer/ipv4/IPv4InterfaceData.h"
#include "inet/visualizer/base/NetworkAddressVisualizerBase.h"

namespace inet {

namespace visualizer {

NetworkAddressVisualizerBase::CacheEntry::CacheEntry(int nodeId, int interfaceId) :
    nodeId(nodeId),
    interfaceId(interfaceId)
{
}

void NetworkAddressVisualizerBase::initialize(int stage)
{
    VisualizerBase::initialize(stage);
    if (!hasGUI()) return;
    if (stage == INITSTAGE_LOCAL) {
        subscriptionModule = *par("subscriptionModule").stringValue() == '\0' ? getSystemModule() : getModuleFromPar<cModule>(par("subscriptionModule"), this);
        subscriptionModule->subscribe(NF_INTERFACE_IPv4CONFIG_CHANGED, this);
        nodeMatcher.setPattern(par("nodeFilter"), true, true, true);
        interfaceMatcher.setPattern(par("interfaceFilter"), false, true, true);
        fontColor = cFigure::parseColor(par("fontColor"));
        backgroundColor = cFigure::parseColor(par("backgroundColor"));
    }
}

NetworkAddressVisualizerBase::CacheEntry *NetworkAddressVisualizerBase::ensureCacheEntry(cModule *networkNode, InterfaceEntry *interfaceEntry)
{
    auto key = std::pair<int, int>(networkNode->getId(), interfaceEntry->getInterfaceId());
    auto it = cacheEntries.find(key);
    if (it == cacheEntries.end()) {
        auto cacheEntry = createCacheEntry(networkNode, interfaceEntry);
        cacheEntries[key] = cacheEntry;
        return cacheEntry;
    }
    else
        return it->second;
}

void NetworkAddressVisualizerBase::receiveSignal(cComponent *source, simsignal_t signal, cObject *object DETAILS_ARG)
{
    if (signal == NF_INTERFACE_IPv4CONFIG_CHANGED && object != nullptr && nodeMatcher.matches(source->getFullPath().c_str())) {
        auto interfaceEntryChangeDetails = static_cast<InterfaceEntryChangeDetails *>(object);
        auto interfaceEntry = interfaceEntryChangeDetails->getInterfaceEntry();
        if (interfaceEntryChangeDetails->getFieldId() == IPv4InterfaceData::F_IP_ADDRESS && interfaceMatcher.matches(interfaceEntry->getFullName())) {
            auto networkNode = getContainingNode(static_cast<cModule *>(source));
            auto cacheEntry = ensureCacheEntry(networkNode, interfaceEntry);
            updateNetworkAddress(networkNode, interfaceEntry, cacheEntry);
        }
    }
}

} // namespace visualizer

} // namespace inet


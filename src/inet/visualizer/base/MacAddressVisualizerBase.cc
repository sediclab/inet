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
#include "inet/networklayer/common/InterfaceEntry.h"
#include "inet/visualizer/base/MacAddressVisualizerBase.h"

namespace inet {

namespace visualizer {

MacAddressVisualizerBase::CacheEntry::CacheEntry(int nodeId, int interfaceId) :
    nodeId(nodeId),
    interfaceId(interfaceId)
{
}

void MacAddressVisualizerBase::initialize(int stage)
{
    VisualizerBase::initialize(stage);
    if (!hasGUI()) return;
    if (stage == INITSTAGE_LOCAL) {
        subscriptionModule = *par("subscriptionModule").stringValue() == '\0' ? getSystemModule() : getModuleFromPar<cModule>(par("subscriptionModule"), this);
        subscriptionModule->subscribe(NF_INTERFACE_CONFIG_CHANGED, this);
        nodeMatcher.setPattern(par("nodeFilter"), true, true, true);
        interfaceMatcher.setPattern(par("interfaceFilter"), false, true, true);
        fontColor = cFigure::parseColor(par("fontColor"));
        backgroundColor = cFigure::parseColor(par("backgroundColor"));
    }
}

MacAddressVisualizerBase::CacheEntry *MacAddressVisualizerBase::ensureCacheEntry(cModule *networkNode, InterfaceEntry *interfaceEntry)
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

void MacAddressVisualizerBase::receiveSignal(cComponent *source, simsignal_t signal, cObject *object DETAILS_ARG)
{
    if (signal == NF_INTERFACE_CONFIG_CHANGED && object != nullptr && nodeMatcher.matches(source->getFullPath().c_str())) {
        auto interfaceEntryDetails = static_cast<InterfaceEntryChangeDetails *>(object);
        auto interfaceEntry = interfaceEntryDetails->getInterfaceEntry();
        if (interfaceEntryDetails->getFieldId() == InterfaceEntry::F_IPV4_DATA && interfaceMatcher.matches(interfaceEntry->getFullName())) {
            auto networkNode = getContainingNode(static_cast<cModule *>(source));
            auto cacheEntry = ensureCacheEntry(networkNode, interfaceEntry);
            updateMacAddress(networkNode, interfaceEntry, cacheEntry);
        }
    }
}

} // namespace visualizer

} // namespace inet


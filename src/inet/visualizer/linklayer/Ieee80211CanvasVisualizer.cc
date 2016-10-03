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
#include "inet/visualizer/linklayer/Ieee80211CanvasVisualizer.h"

namespace inet {

namespace visualizer {

Define_Module(Ieee80211CanvasVisualizer);

Ieee80211CanvasVisualizer::CanvasCacheEntry::CanvasCacheEntry(cIconFigure *figure, NetworkNodeCanvasVisualization *visualization, int nodeId, int interfaceId) :
    CacheEntry(nodeId, interfaceId),
    figure(figure),
    visualization(visualization)
{
}

void Ieee80211CanvasVisualizer::initialize(int stage)
{
    Ieee80211VisualizerBase::initialize(stage);
    if (!hasGUI()) return;
    if (stage == INITSTAGE_LOCAL) {
        zIndex = par("zIndex");
        networkNodeVisualizer = getModuleFromPar<NetworkNodeCanvasVisualizer>(par("networkNodeVisualizerModule"), this);
    }
}

Ieee80211VisualizerBase::CacheEntry *Ieee80211CanvasVisualizer::createCacheEntry(cModule *networkNode, InterfaceEntry *interfaceEntry)
{
    auto visualization = networkNodeVisualizer->getNeworkNodeVisualization(networkNode);
    auto iconFigure = new cIconFigure();
    iconFigure->setImageName("misc/signal");
    return new CanvasCacheEntry(iconFigure, visualization, networkNode->getId(), interfaceEntry->getInterfaceId());
}

void Ieee80211CanvasVisualizer::addCacheEntry(CacheEntry *cacheEntry)
{
    Ieee80211VisualizerBase::addCacheEntry(cacheEntry);
    auto canvasCacheEntry = static_cast<const CanvasCacheEntry *>(cacheEntry);
    canvasCacheEntry->visualization->addAnnotation(canvasCacheEntry->figure, canvasCacheEntry->figure->getBounds().getSize());
}

void Ieee80211CanvasVisualizer::removeCacheEntry(CacheEntry *cacheEntry)
{
    Ieee80211VisualizerBase::removeCacheEntry(cacheEntry);
    auto canvasCacheEntry = static_cast<const CanvasCacheEntry *>(cacheEntry);
    canvasCacheEntry->visualization->removeAnnotation(canvasCacheEntry->figure);
}

} // namespace visualizer

} // namespace inet


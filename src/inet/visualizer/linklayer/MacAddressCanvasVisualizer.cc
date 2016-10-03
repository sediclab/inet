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
#include "inet/visualizer/linklayer/MacAddressCanvasVisualizer.h"

namespace inet {

namespace visualizer {

Define_Module(MacAddressCanvasVisualizer);

void MacAddressCanvasVisualizer::initialize(int stage)
{
    MacAddressVisualizerBase::initialize(stage);
    if (!hasGUI()) return;
    if (stage == INITSTAGE_LOCAL) {
        zIndex = par("zIndex");
        networkNodeVisualizer = getModuleFromPar<NetworkNodeCanvasVisualizer>(par("networkNodeVisualizerModule"), this);
    }
}

MacAddressVisualizerBase::CacheEntry *MacAddressCanvasVisualizer::createCacheEntry(cModule *networkNode, InterfaceEntry *interfaceEntry)
{
    return new CanvasCacheEntry(networkNode->getId(), interfaceEntry->getInterfaceId());
}

void MacAddressCanvasVisualizer::updateMacAddress(cModule *networkNode, InterfaceEntry *interfaceEntry, CacheEntry *cacheEntry)
{
    auto canvasCacheEntry = static_cast<CanvasCacheEntry *>(cacheEntry);
    if (canvasCacheEntry->figure == nullptr) {
        auto labelFigure = new BoxedLabelFigure();
        labelFigure->setFontColor(fontColor);
        labelFigure->setBackgroundColor(backgroundColor);
        labelFigure->setZIndex(zIndex);
        auto visualization = networkNodeVisualizer->getNeworkNodeVisualization(networkNode);
        visualization->addAnnotation(labelFigure, labelFigure->getBounds().getSize());
        canvasCacheEntry->figure = labelFigure;
    }
    auto text = interfaceEntry->getMacAddress().str();
    canvasCacheEntry->figure->setText(text.c_str());
}


} // namespace visualizer

} // namespace inet


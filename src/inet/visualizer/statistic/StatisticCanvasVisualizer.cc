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
#include "inet/visualizer/networknode/NetworkNodeCanvasVisualizer.h"
#include "inet/visualizer/statistic/StatisticCanvasVisualizer.h"

namespace inet {

namespace visualizer {

Define_Module(StatisticCanvasVisualizer);

StatisticCanvasVisualizer::CanvasCacheEntry::CanvasCacheEntry(const char *unit, NetworkNodeCanvasVisualization *visualization, BoxedLabelFigure *figure) :
    CacheEntry(unit),
    visualization(visualization),
    figure(figure)
{
}

void StatisticCanvasVisualizer::initialize(int stage)
{
    StatisticVisualizerBase::initialize(stage);
    if (!hasGUI()) return;
    if (stage == INITSTAGE_LOCAL) {
        zIndex = par("zIndex");
    }
}

StatisticVisualizerBase::CacheEntry *StatisticCanvasVisualizer::createCacheEntry(cComponent *source, simsignal_t signal)
{
    auto labelFigure = new BoxedLabelFigure("statistic");
    labelFigure->setFontColor(fontColor);
    labelFigure->setBackgroundColor(backgroundColor);
    labelFigure->setText("");
    labelFigure->setZIndex(zIndex);
    auto networkNodeVisualizer = getModuleFromPar<NetworkNodeCanvasVisualizer>(par("networkNodeVisualizerModule"), this);
    auto networkNode = getContainingNode(check_and_cast<cModule *>(source));
    auto visualization = networkNodeVisualizer->getNeworkNodeVisualization(networkNode);
    return new CanvasCacheEntry(getUnit(source), visualization, labelFigure);
}

void StatisticCanvasVisualizer::addCacheEntry(std::pair<int, int> moduleAndSignal, CacheEntry *cacheEntry)
{
    StatisticVisualizerBase::addCacheEntry(moduleAndSignal, cacheEntry);
    auto canvasCacheEntry = static_cast<CanvasCacheEntry *>(cacheEntry);
    canvasCacheEntry->visualization->addAnnotation(canvasCacheEntry->figure, canvasCacheEntry->figure->getBounds().getSize());
}

void StatisticCanvasVisualizer::removeCacheEntry(std::pair<int, int> moduleAndSignal, CacheEntry *cacheEntry)
{
    StatisticVisualizerBase::removeCacheEntry(moduleAndSignal, cacheEntry);
    auto canvasCacheEntry = static_cast<CanvasCacheEntry *>(cacheEntry);
    canvasCacheEntry->visualization->removeAnnotation(canvasCacheEntry->figure);
}

void StatisticCanvasVisualizer::refreshStatistic(CacheEntry *cacheEntry)
{
    auto canvasCacheEntry = static_cast<CanvasCacheEntry *>(cacheEntry);
    auto text = getText(cacheEntry);
    canvasCacheEntry->figure->setText(text.c_str());
}

} // namespace visualizer

} // namespace inet


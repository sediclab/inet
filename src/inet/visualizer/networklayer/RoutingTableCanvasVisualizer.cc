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

#include "inet/visualizer/networklayer/RoutingTableCanvasVisualizer.h"

namespace inet {

namespace visualizer {

Define_Module(RoutingTableCanvasVisualizer);

RoutingTableCanvasVisualizer::CanvasRoute::CanvasRoute(cLineFigure *figure, int nodeModuleId, int nextHopModuleId) :
    Route(nodeModuleId, nextHopModuleId),
    figure(figure)
{
}

RoutingTableCanvasVisualizer::CanvasRoute::~CanvasRoute()
{
    delete figure;
}

void RoutingTableCanvasVisualizer::initialize(int stage)
{
    RoutingTableVisualizerBase::initialize(stage);
    if (!hasGUI()) return;
    if (stage == INITSTAGE_LOCAL) {
        zIndex = par("zIndex");
        auto canvas = visualizerTargetModule->getCanvas();
        canvasProjection = CanvasProjection::getCanvasProjection(canvas);
        routeGroup = new cGroupFigure();
        routeGroup->setZIndex(zIndex);
        canvas->addFigure(routeGroup);
    }
}

void RoutingTableCanvasVisualizer::addRoute(std::pair<int, int> nodeAndNextHop, const Route *route)
{
    RoutingTableVisualizerBase::addRoute(nodeAndNextHop, route);
    auto canvasRoute = static_cast<const CanvasRoute *>(route);
    routeGroup->addFigure(canvasRoute->figure);
}

void RoutingTableCanvasVisualizer::removeRoute(const Route *route)
{
    RoutingTableVisualizerBase::removeRoute(route);
    auto canvasRoute = static_cast<const CanvasRoute *>(route);
    routeGroup->removeFigure(canvasRoute->figure);
}

void RoutingTableCanvasVisualizer::setPosition(cModule *node, const Coord& position) const
{
    for (auto it : routes) {
        auto route = static_cast<const CanvasRoute *>(it.second);
        auto figure = route->figure;
        if (node->getId() == route->nodeModuleId)
            figure->setStart(canvasProjection->computeCanvasPoint(position));
        else if (node->getId() == route->nextHopModuleId)
            figure->setEnd(canvasProjection->computeCanvasPoint(position));
    }
}

const RoutingTableVisualizerBase::Route *RoutingTableCanvasVisualizer::createRoute(cModule *node, cModule *nextHop) const
{
    auto figure = new cLineFigure();
    figure->setStart(canvasProjection->computeCanvasPoint(getPosition(node)));
    figure->setEnd(canvasProjection->computeCanvasPoint(getPosition(nextHop)));
    figure->setEndArrowhead(cFigure::ARROW_BARBED);
    figure->setLineWidth(lineWidth);
    figure->setLineColor(lineColor);
    figure->setLineStyle(lineStyle);
    return new CanvasRoute(figure, node->getId(), nextHop->getId());
}

} // namespace visualizer

} // namespace inet


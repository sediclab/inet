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
#include "inet/visualizer/linklayer/LinkBreakCanvasVisualizer.h"

namespace inet {

namespace visualizer {

Define_Module(LinkBreakCanvasVisualizer);

LinkBreakCanvasVisualizer::CanvasLinkBreak::CanvasLinkBreak(cIconFigure *figure, simtime_t breakTime) :
    LinkBreak(breakTime),
    figure(figure)
{
}

void LinkBreakCanvasVisualizer::initialize(int stage)
{
    LinkBreakVisualizerBase::initialize(stage);
    if (!hasGUI()) return;
    if (stage == INITSTAGE_LOCAL) {
        zIndex = par("zIndex");
        auto canvas = visualizerTargetModule->getCanvas();
        canvasProjection = CanvasProjection::getCanvasProjection(canvas);
        linkBreakGroup = new cGroupFigure();
        linkBreakGroup->setZIndex(zIndex);
        canvas->addFigure(linkBreakGroup);
    }
}

void LinkBreakCanvasVisualizer::setAlpha(const LinkBreak *linkBreak, double alpha) const
{
    auto canvasLinkBreak = static_cast<const CanvasLinkBreak *>(linkBreak);
    auto figure = canvasLinkBreak->figure;
    figure->setOpacity(alpha);
}

const LinkBreakVisualizerBase::LinkBreak *LinkBreakCanvasVisualizer::createLinkBreak(cModule *transmitter, cModule *receiver) const
{
    auto transmitterPosition = canvasProjection->computeCanvasPoint(getPosition(getContainingNode(transmitter)));
    auto receiverPosition = canvasProjection->computeCanvasPoint(getPosition(getContainingNode(receiver)));
    auto figure = new cIconFigure();
    figure->setImageName(icon);
    figure->setTintAmount(iconTintAmount);
    figure->setTintColor(iconTintColor);
    figure->setPosition((transmitterPosition + receiverPosition) / 2);
    return new CanvasLinkBreak(figure, simTime());
}

void LinkBreakCanvasVisualizer::addLinkBreak(const LinkBreak *linkBreak)
{
    LinkBreakVisualizerBase::addLinkBreak(linkBreak);
    auto canvasLinkBreak = static_cast<const CanvasLinkBreak *>(linkBreak);
    linkBreakGroup->addFigure(canvasLinkBreak->figure);
}

void LinkBreakCanvasVisualizer::removeLinkBreak(const LinkBreak *linkBreak)
{
    LinkBreakVisualizerBase::removeLinkBreak(linkBreak);
    auto canvasLinkBreak = static_cast<const CanvasLinkBreak *>(linkBreak);
    linkBreakGroup->removeFigure(canvasLinkBreak->figure);
}

} // namespace visualizer

} // namespace inet


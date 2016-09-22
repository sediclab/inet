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
#include "inet/visualizer/common/PacketDropCanvasVisualizer.h"

namespace inet {

namespace visualizer {

Define_Module(PacketDropCanvasVisualizer);

PacketDropCanvasVisualizer::CanvasPacketDrop::CanvasPacketDrop(cIconFigure *figure, int moduleId, cPacket *packet, simtime_t dropSimulationTime, double dropAnimationTime, int dropRealTime) :
    PacketDrop(moduleId, packet, dropSimulationTime, dropAnimationTime, dropRealTime),
    figure(figure)
{
}

void PacketDropCanvasVisualizer::initialize(int stage)
{
    PacketDropVisualizerBase::initialize(stage);
    if (!hasGUI()) return;
    if (stage == INITSTAGE_LOCAL) {
        zIndex = par("zIndex");
        auto canvas = visualizerTargetModule->getCanvas();
        canvasProjection = CanvasProjection::getCanvasProjection(canvas);
        packetDropGroup = new cGroupFigure();
        packetDropGroup->setZIndex(zIndex);
        canvas->addFigure(packetDropGroup);
    }
}

void PacketDropCanvasVisualizer::setAlpha(const PacketDrop *packetDrop, double alpha) const
{
    auto canvasPacketDrop = static_cast<const CanvasPacketDrop *>(packetDrop);
    auto figure = canvasPacketDrop->figure;
    figure->setOpacity(alpha);
    auto position = getPosition(getContainingNode(getSimulation()->getModule(packetDrop->moduleId)));
    double dx = 10 / alpha;
    double dy = pow((dx / 4 - 9), 2) - 42;
    figure->setPosition(canvasProjection->computeCanvasPoint(position) + cFigure::Point(dx, dy));
}

const PacketDropVisualizerBase::PacketDrop *PacketDropCanvasVisualizer::createPacketDrop(cModule *module, cPacket *packet) const
{
    auto figure = new cIconFigure();
    figure->setImageName(icon);
    figure->setTintAmount(iconTintAmount);
    figure->setTintColor(iconTintColor);
    figure->setPosition(canvasProjection->computeCanvasPoint(getPosition(getContainingNode(module))));
    return new CanvasPacketDrop(figure, module->getId(), packet, getSimulation()->getSimTime(), getSimulation()->getEnvir()->getAnimationTime(), getRealTime());
}

void PacketDropCanvasVisualizer::addPacketDrop(const PacketDrop *packetDrop)
{
    PacketDropVisualizerBase::addPacketDrop(packetDrop);
    auto canvasPacketDrop = static_cast<const CanvasPacketDrop *>(packetDrop);
    packetDropGroup->addFigure(canvasPacketDrop->figure);
}

void PacketDropCanvasVisualizer::removePacketDrop(const PacketDrop *packetDrop)
{
    PacketDropVisualizerBase::removePacketDrop(packetDrop);
    auto canvasPacketDrop = static_cast<const CanvasPacketDrop *>(packetDrop);
    packetDropGroup->removeFigure(canvasPacketDrop->figure);
}

} // namespace visualizer

} // namespace inet


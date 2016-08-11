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

#include "inet/common/OSGUtils.h"
#include "inet/common/ModuleAccess.h"
#include "inet/visualizer/transportlayer/TransportConnectionOsgVisualizer.h"

namespace inet {

namespace visualizer {

Define_Module(TransportConnectionOsgVisualizer);

TransportConnectionOsgVisualizer::OsgConnection::OsgConnection(osg::Node *sourceNode, osg::Node *destinationNode, int sourceModuleId, int destinationModuleId, int count) :
    Connection(sourceModuleId, destinationModuleId, count),
    sourceNode(sourceNode),
    destinationNode(destinationNode)
{
}

void TransportConnectionOsgVisualizer::addConnection(const Connection *connection)
{
    TransportConnectionVisualizerBase::addConnection(connection);
    auto osgConnection = static_cast<const OsgConnection *>(connection);
    auto sourceModule = getSimulation()->getModule(connection->sourceModuleId);
    auto sourceVisualization = networkNodeVisualizer->getNeworkNodeVisualization(getContainingNode(sourceModule));
    sourceVisualization->addAnnotation(osgConnection->sourceNode, osg::Vec3d(0, 0, 8), 0);
    auto destinationModule = getSimulation()->getModule(connection->destinationModuleId);
    auto destinationVisualization = networkNodeVisualizer->getNeworkNodeVisualization(getContainingNode(destinationModule));
    destinationVisualization->addAnnotation(osgConnection->destinationNode, osg::Vec3d(0, 0, 8), 0);
}

void TransportConnectionOsgVisualizer::removeConnection(const Connection *connection)
{
    TransportConnectionVisualizerBase::removeConnection(connection);
    auto osgConnection = static_cast<const OsgConnection *>(connection);
    auto sourceModule = getSimulation()->getModule(connection->sourceModuleId);
    auto sourceVisualization = networkNodeVisualizer->getNeworkNodeVisualization(getContainingNode(sourceModule));
    sourceVisualization->removeAnnotation(osgConnection->sourceNode);
    auto destinationModule = getSimulation()->getModule(connection->destinationModuleId);
    auto destinationVisualization = networkNodeVisualizer->getNeworkNodeVisualization(getContainingNode(destinationModule));
    destinationVisualization->removeAnnotation(osgConnection->destinationNode);
}

osg::Node *TransportConnectionOsgVisualizer::createConnectionEndNode(tcp::TCPConnection *tcpConnection) const
{
    auto path = resolveResourcePath("misc/marker");
    auto image = inet::osg::createImage(path.c_str());
    auto texture = new osg::Texture2D();
    texture->setImage(image);
    auto geometry = osg::createTexturedQuadGeometry(osg::Vec3(-image->s() / 2, 0.0, 0.0), osg::Vec3(image->s(), 0.0, 0.0), osg::Vec3(0.0, image->t(), 0.0), 0.0, 0.0, 1.0, 1.0);
    auto stateSet = geometry->getOrCreateStateSet();
    stateSet->setTextureAttributeAndModes(0, texture);
    stateSet->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);
    stateSet->setMode(GL_BLEND, osg::StateAttribute::ON);
    stateSet->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
    auto geode = new osg::Geode();
    geode->addDrawable(geometry);
    auto color = cFigure::GOOD_DARK_COLORS[connections.size() % (sizeof(cFigure::GOOD_DARK_COLORS) / sizeof(cFigure::Color))];
    // TODO: use color for tinting
    return geode;
}

const TransportConnectionVisualizerBase::Connection *TransportConnectionOsgVisualizer::createConnection(cModule *source, cModule *destination, tcp::TCPConnection *tcpConnection) const
{
    auto sourceNode = createConnectionEndNode(tcpConnection);
    auto destinationNode = createConnectionEndNode(tcpConnection);
    return new OsgConnection(sourceNode, destinationNode, source->getId(), destination->getId(), 1);
}

} // namespace visualizer

} // namespace inet


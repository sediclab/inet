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
#include "inet/mobility/contract/IMobility.h"
#include "inet/networklayer/common/L3AddressResolver.h"
#include "inet/networklayer/ipv4/IPv4InterfaceData.h"
#include "inet/visualizer/base/RoutingTableVisualizerBase.h"

namespace inet {

namespace visualizer {

RoutingTableVisualizerBase::Route::Route(int nodeModuleId, int nextHopModuleId) :
    nodeModuleId(nodeModuleId),
    nextHopModuleId(nextHopModuleId)
{
}

void RoutingTableVisualizerBase::initialize(int stage)
{
    VisualizerBase::initialize(stage);
    if (!hasGUI()) return;
    if (stage == INITSTAGE_LOCAL) {
        subscriptionModule = *par("subscriptionModule").stringValue() == '\0' ? getSystemModule() : getModuleFromPar<cModule>(par("subscriptionModule"), this);
        subscriptionModule->subscribe(IMobility::mobilityStateChangedSignal, this);
        subscriptionModule->subscribe(NF_ROUTE_ADDED, this);
        subscriptionModule->subscribe(NF_ROUTE_DELETED, this);
        subscriptionModule->subscribe(NF_ROUTE_CHANGED, this);
        destinationMatcher.setPattern(par("destinationFilter"), true, true, true);
        if (*par("lineColor").stringValue() != '\0')
            lineColor = cFigure::Color(par("lineColor"));
        lineWidth = par("lineWidth");
        lineStyle = cFigure::parseLineStyle(par("lineStyle"));
    }
}

void RoutingTableVisualizerBase::receiveSignal(cComponent *source, simsignal_t signal, cObject *object DETAILS_ARG)
{
    if (signal == IMobility::mobilityStateChangedSignal) {
        auto mobility = dynamic_cast<IMobility *>(object);
        auto position = mobility->getCurrentPosition();
        auto module = check_and_cast<cModule *>(source);
        auto node = getContainingNode(module);
        setPosition(node, position);
    }
    else if (signal == NF_ROUTE_ADDED || signal == NF_ROUTE_DELETED || signal == NF_ROUTE_CHANGED)
        updateRoutes(check_and_cast<IPv4RoutingTable *>(source));
}

const RoutingTableVisualizerBase::Route *RoutingTableVisualizerBase::getRoute(std::pair<int, int> route)
{
    auto it = routes.find(route);
    return it == routes.end() ? nullptr : it->second;
}

void RoutingTableVisualizerBase::addRoute(std::pair<int, int> nodeAndNextHop, const Route *route)
{
    routes[nodeAndNextHop] = route;
}

void RoutingTableVisualizerBase::removeRoute(const Route *route)
{
    routes.erase(routes.find(std::pair<int, int>(route->nodeModuleId, route->nextHopModuleId)));
}

std::vector<IPv4Address> RoutingTableVisualizerBase::getDestinations()
{
    L3AddressResolver addressResolver;
    std::vector<IPv4Address> destinations;
    for (cModule::SubmoduleIterator it(getSystemModule()); !it.end(); it++) {
        auto networkNode = *it;
        if (isNetworkNode(networkNode) && destinationMatcher.matches(networkNode->getFullPath().c_str())) {
            auto interfaceTable = addressResolver.findInterfaceTableOf(networkNode);
            for (int i = 0; i < interfaceTable->getNumInterfaces(); i++) {
                auto interface = interfaceTable->getInterface(i);
                if (interface->ipv4Data() != nullptr)
                    destinations.push_back(interface->ipv4Data()->getIPAddress());
            }
        }
    }
    return destinations;
}

void RoutingTableVisualizerBase::addRoutes(IPv4RoutingTable *routingTable)
{
    L3AddressResolver addressResolver;
    auto node = getContainingNode(routingTable);
    for (auto destination : getDestinations()) {
        if (!routingTable->isLocalAddress(destination)) {
            auto route = routingTable->findBestMatchingRoute(destination);
            if (route != nullptr) {
                auto gateway = route->getGateway();
                auto nextHop = addressResolver.findHostWithAddress(gateway.isUnspecified() ? destination : gateway);
                if (nextHop != nullptr) {
                    auto key = std::pair<int, int>(node->getId(), nextHop->getId());
                    if (routes.find(key) == routes.end())
                        addRoute(key, createRoute(node, nextHop));
                }
            }
        }
    }
}

void RoutingTableVisualizerBase::removeRoutes(IPv4RoutingTable *routingTable)
{
    auto node = getContainingNode(routingTable);
    std::vector<const Route*> removedRoutes;
    for (auto it : routes)
        if (it.first.first == node->getId() && it.second)
            removedRoutes.push_back(it.second);
    for (auto it : removedRoutes) {
        removeRoute(it);
        delete it;
    }
}

void RoutingTableVisualizerBase::updateRoutes(IPv4RoutingTable *routingTable)
{
    removeRoutes(routingTable);
    addRoutes(routingTable);
}

} // namespace visualizer

} // namespace inet


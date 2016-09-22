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

#include <algorithm>
#include "inet/common/ModuleAccess.h"
#include "inet/common/NotifierConsts.h"
#include "inet/linklayer/contract/IMACFrame.h"
#include "inet/networklayer/common/L3AddressResolver.h"
#include "inet/visualizer/base/LinkBreakVisualizerBase.h"

namespace inet {

namespace visualizer {

LinkBreakVisualizerBase::LinkBreak::LinkBreak(simtime_t breakSimulationTime, double breakAnimationTime, double breakRealTime) :
    breakSimulationTime(breakSimulationTime),
    breakAnimationTime(breakAnimationTime),
    breakRealTime(breakRealTime)
{
}

void LinkBreakVisualizerBase::initialize(int stage)
{
    VisualizerBase::initialize(stage);
    if (!hasGUI()) return;
    if (stage == INITSTAGE_LOCAL) {
        subscriptionModule = *par("subscriptionModule").stringValue() == '\0' ? getSystemModule() : getModuleFromPar<cModule>(par("subscriptionModule"), this);
        subscriptionModule->subscribe(NF_LINK_BREAK, this);
        nodeMatcher.setPattern(par("nodeFilter"), true, true, true);
        icon = par("icon");
        iconTintAmount = par("iconTintAmount");
        if (iconTintAmount != 0)
            iconTintColor = cFigure::Color(par("iconTintColor"));
        fadeOutMode = par("fadeOutMode");
        fadeOutHalfLife = par("fadeOutHalfLife");
    }
}

void LinkBreakVisualizerBase::refreshDisplay() const
{
    auto currentSimulationTime = simTime();
    double currentRealTime = getRealTime();
    double currentAnimationTime = getSimulation()->getEnvir()->getAnimationTime();
    std::vector<const LinkBreak *> removedLinkBreaks;
    for (auto linkBreak : linkBreaks) {
        double delta;
        if (!strcmp(fadeOutMode, "simulationTime"))
            delta = (currentSimulationTime - linkBreak->breakSimulationTime).dbl();
        else if (!strcmp(fadeOutMode, "animationTime"))
            delta = currentAnimationTime - linkBreak->breakAnimationTime;
        else if (!strcmp(fadeOutMode, "realTime"))
            delta = currentRealTime - linkBreak->breakRealTime;
        else
            throw cRuntimeError("Unknown fadeOutMode: %s", fadeOutMode);
        auto alpha = std::min(1.0, std::pow(2.0, -delta / fadeOutHalfLife));
        if (alpha < 0.01)
            removedLinkBreaks.push_back(linkBreak);
        else
            setAlpha(linkBreak, alpha);
    }
    for (auto linkBreak : removedLinkBreaks) {
        const_cast<LinkBreakVisualizerBase *>(this)->removeLinkBreak(linkBreak);
        delete linkBreak;
    }
}

void LinkBreakVisualizerBase::receiveSignal(cComponent *source, simsignal_t signal, cObject *object DETAILS_ARG)
{
    if (signal == NF_LINK_BREAK) {
        auto frame = dynamic_cast<IMACFrame *>(object);
        if (frame != nullptr) {
            auto transmitter = findNode(frame->getTransmitterAddress());
            auto receiver = findNode(frame->getReceiverAddress());
            if (nodeMatcher.matches(transmitter->getFullPath().c_str()) && nodeMatcher.matches(receiver->getFullPath().c_str()))
                addLinkBreak(createLinkBreak(transmitter, receiver));
        }
    }
}

void LinkBreakVisualizerBase::addLinkBreak(const LinkBreak *linkBreak)
{
    linkBreaks.push_back(linkBreak);
}

void LinkBreakVisualizerBase::removeLinkBreak(const LinkBreak *linkBreak)
{
    linkBreaks.erase(std::remove(linkBreaks.begin(), linkBreaks.end(), linkBreak), linkBreaks.end());
}

// TODO: inefficient, create L2AddressResolver?
cModule *LinkBreakVisualizerBase::findNode(MACAddress address)
{
    L3AddressResolver addressResolver;
    for (cModule::SubmoduleIterator it(getSystemModule()); !it.end(); it++) {
        auto networkNode = *it;
        if (isNetworkNode(networkNode)) {
            auto interfaceTable = addressResolver.findInterfaceTableOf(networkNode);
            for (int i = 0; i < interfaceTable->getNumInterfaces(); i++)
                if (interfaceTable->getInterface(i)->getMacAddress() == address)
                    return networkNode;
        }
    }
    return nullptr;
}

} // namespace visualizer

} // namespace inet


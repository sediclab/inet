//
// Copyright (C) 2015 Andras Varga
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
// along with this program; if not, see http://www.gnu.org/licenses/.
//
// Author: Andras Varga
//
#include <algorithm>

#include "Ieee80211NewMac.h"
#include "inet/networklayer/contract/IInterfaceTable.h"
#include "inet/linklayer/ieee80211/mac/Ieee80211Frame_m.h"
#include "inet/physicallayer/ieee80211/packetlevel/Ieee80211ControlInfo_m.h"
#include "inet/physicallayer/ieee80211/mode/Ieee80211ModeSet.h"
#include "Ieee80211UpperMac.h"
#include "Ieee80211MacRx.h"
#include "Ieee80211MacTx.h"
#include "Ieee80211UpperMacContext.h"
#include "inet/common/INETUtils.h"
#include "inet/common/ModuleAccess.h"

namespace inet {
namespace ieee80211 {

Define_Module(Ieee80211NewMac);

simsignal_t Ieee80211NewMac::stateSignal = SIMSIGNAL_NULL;
simsignal_t Ieee80211NewMac::radioStateSignal = SIMSIGNAL_NULL;


Ieee80211NewMac::Ieee80211NewMac()
{
}

Ieee80211NewMac::~Ieee80211NewMac()
{
    if (pendingRadioConfigMsg)
        delete pendingRadioConfigMsg;
}

void Ieee80211NewMac::initialize(int stage)
{
    MACProtocolBase::initialize(stage);

    if (stage == INITSTAGE_LOCAL)
    {
        EV << "Initializing stage 0\n";

        const Ieee80211ModeSet *modeSet = Ieee80211ModeSet::getModeSet(*par("opMode").stringValue());

        // radio
        cModule *radioModule = gate("lowerLayerOut")->getNextGate()->getOwnerModule();
        radioModule->subscribe(IRadio::radioModeChangedSignal, this);
        radioModule->subscribe(IRadio::receptionStateChangedSignal, this);
        radioModule->subscribe(IRadio::transmissionStateChangedSignal, this);
        radio = check_and_cast<IRadio *>(radioModule);

        upperMac = check_and_cast<IIeee80211UpperMac*>(getModuleByPath(".upperMac"));  //TODO
        rx = check_and_cast<IIeee80211MacRx*>(getModuleByPath(".rx"));  //TODO
        tx = check_and_cast<IIeee80211MacTx*>(getModuleByPath(".tx"));  //TODO

        // initialize parameters
        double bitrate = par("bitrate");
        double basicBitrate = par("basicBitrate");
        int rtsThreshold = par("rtsThresholdBytes");

        const IIeee80211Mode *dataFrameMode = (bitrate == -1) ? modeSet->getFastestMode() : modeSet->getMode(bps(bitrate));
        const IIeee80211Mode *basicFrameMode = (basicBitrate == -1) ? modeSet->getSlowestMode() : modeSet->getMode(bps(basicBitrate));; //TODO ???
        const IIeee80211Mode *controlFrameMode = (basicBitrate == -1) ? modeSet->getSlowestMode() : modeSet->getMode(bps(basicBitrate)); //TODO ???

        int shortRetryLimit = par("retryLimit");
        if (shortRetryLimit == -1)
            shortRetryLimit = 7;
        ASSERT(shortRetryLimit > 0);

        const char *addressString = par("address");
        if (!strcmp(addressString, "auto")) {
            // assign automatic address
            address = MACAddress::generateAutoAddress();
            // change module parameter from "auto" to concrete address
            par("address").setStringValue(address.str().c_str());
        }
        else
            address.setAddress(addressString);

        IIeee80211UpperMacContext *context = new Ieee80211UpperMacContext(address, dataFrameMode, basicFrameMode, controlFrameMode, shortRetryLimit, rtsThreshold, tx);
        upperMac->setContext(context);
        rx->setAddress(address);

        // Initialize self messages
        stateSignal = registerSignal("state");
    //        stateSignal.setEnum("Ieee80211NewMac");
        radioStateSignal = registerSignal("radioState");
    //        radioStateSignal.setEnum("RadioState");
        // interface
        registerInterface();

        // statistics
        numRetry = 0;
        numSentWithoutRetry = 0;
        numGivenUp = 0;
        numCollision = 0;
        numSent = 0;
        numReceived = 0;
        numSentBroadcast = 0;
        numReceivedBroadcast = 0;

        WATCH(numRetry);
        WATCH(numSentWithoutRetry);
        WATCH(numGivenUp);
        WATCH(numCollision);
        WATCH(numSent);
        WATCH(numReceived);
        WATCH(numSentBroadcast);
        WATCH(numReceivedBroadcast);
    }
    else if (stage == INITSTAGE_LINK_LAYER)
    {
        if (isOperational)
            radio->setRadioMode(IRadio::RADIO_MODE_RECEIVER);
        if (isInterfaceRegistered().isUnspecified()) //TODO do we need multi-MAC feature? if so, should they share interfaceEntry??  --Andras
            registerInterface();
    }
}

const MACAddress& Ieee80211NewMac::isInterfaceRegistered()
{
//    if (!par("multiMac").boolValue())
//        return MACAddress::UNSPECIFIED_ADDRESS;

    IInterfaceTable *ift = findModuleFromPar<IInterfaceTable>(par("interfaceTableModule"), this);
    if (!ift)
        return MACAddress::UNSPECIFIED_ADDRESS;
    cModule *interfaceModule = findModuleUnderContainingNode(this);
    if (!interfaceModule)
        throw cRuntimeError("NIC module not found in the host");
    std::string interfaceName = utils::stripnonalnum(interfaceModule->getFullName());
    InterfaceEntry *e = ift->getInterfaceByName(interfaceName.c_str());
    if (e)
        return e->getMacAddress();
    return MACAddress::UNSPECIFIED_ADDRESS;
}

InterfaceEntry *Ieee80211NewMac::createInterfaceEntry()
{
    InterfaceEntry *e = new InterfaceEntry(this);

    // address
    e->setMACAddress(address);
    e->setInterfaceToken(address.formInterfaceIdentifier());

    e->setMtu(par("mtu").longValue());

    // capabilities
    e->setBroadcast(true);
    e->setMulticast(true);
    e->setPointToPoint(false);

    return e;
}

void Ieee80211NewMac::handleSelfMessage(cMessage *msg)
{
    EV << "received self message: " << msg << endl;
    if (msg->getContextPointer() != nullptr)
        ((Ieee80211MacPlugin *)msg->getContextPointer())->handleMessage(msg);
    else
        ASSERT(false);
}

void Ieee80211NewMac::handleUpperPacket(cPacket *msg)
{
    upperMac->upperFrameReceived(check_and_cast<Ieee80211DataOrMgmtFrame*>(msg));
}

void Ieee80211NewMac::handleLowerPacket(cPacket *msg)
{
    rx->lowerFrameReceived(check_and_cast<Ieee80211Frame *>(msg));
}

void Ieee80211NewMac::handleUpperCommand(cMessage *msg)
{
    if (msg->getKind() == RADIO_C_CONFIGURE) {
        EV_DEBUG << "Passing on command " << msg->getName() << " to physical layer\n";
        if (pendingRadioConfigMsg != nullptr) {
            // merge contents of the old command into the new one, then delete it
            Ieee80211ConfigureRadioCommand *oldConfigureCommand = check_and_cast<Ieee80211ConfigureRadioCommand *>(pendingRadioConfigMsg->getControlInfo());
            Ieee80211ConfigureRadioCommand *newConfigureCommand = check_and_cast<Ieee80211ConfigureRadioCommand *>(msg->getControlInfo());
            if (newConfigureCommand->getChannelNumber() == -1 && oldConfigureCommand->getChannelNumber() != -1)
                newConfigureCommand->setChannelNumber(oldConfigureCommand->getChannelNumber());
            if (isNaN(newConfigureCommand->getBitrate().get()) && !isNaN(oldConfigureCommand->getBitrate().get()))
                newConfigureCommand->setBitrate(oldConfigureCommand->getBitrate());
            delete pendingRadioConfigMsg;
            pendingRadioConfigMsg = nullptr;
        }

        if (rx->isMediumFree()) { // TODO: !!!
            EV_DEBUG << "Sending it down immediately\n";
/*
   // Dynamic power
            PhyControlInfo *phyControlInfo = dynamic_cast<PhyControlInfo *>(msg->getControlInfo());
            if (phyControlInfo)
                phyControlInfo->setAdaptiveSensitivity(true);
   // end dynamic power
 */
            sendDown(msg);
        }
        else {
            EV_DEBUG << "Delaying " << msg->getName() << " until next IDLE or DEFER state\n";
            pendingRadioConfigMsg = msg;
        }
    }
    else {
        throw cRuntimeError("Unrecognized command from mgmt layer: (%s)%s msgkind=%d", msg->getClassName(), msg->getName(), msg->getKind());
    }
}


void Ieee80211NewMac::receiveSignal(cComponent *source, simsignal_t signalID, long value)
{
    Enter_Method("receiveSignal()");
    if (signalID == IRadio::receptionStateChangedSignal)
    {
        rx->receptionStateChanged((IRadio::ReceptionState)value);
    }
    else if (signalID == IRadio::transmissionStateChangedSignal)
    {
        auto oldTransmissionState = transmissionState;
        transmissionState = (IRadio::TransmissionState)value;

        bool transmissionFinished = (oldTransmissionState == IRadio::TRANSMISSION_STATE_TRANSMITTING && transmissionState == IRadio::TRANSMISSION_STATE_IDLE);

        if (transmissionFinished) {
            tx->radioTransmissionFinished();
            EV_DEBUG << "changing radio to receiver mode\n";
            configureRadioMode(IRadio::RADIO_MODE_RECEIVER);  //FIXME this is in a very wrong place!!! should be done explicitly from UpperMac!
        }
        rx->transmissionStateChanged(transmissionState);
    }
}

void Ieee80211NewMac::configureRadioMode(IRadio::RadioMode radioMode)
{
    if (radio->getRadioMode() != radioMode) {
        ConfigureRadioCommand *configureCommand = new ConfigureRadioCommand();
        configureCommand->setRadioMode(radioMode);
        cMessage *message = new cMessage("configureRadioMode", RADIO_C_CONFIGURE);
        message->setControlInfo(configureCommand);
        sendDown(message);
    }
}

void Ieee80211NewMac::sendUp(cMessage *message)
{
    Enter_Method("sendUp()");
    take(message);
    MACProtocolBase::sendUp(message);
}

void Ieee80211NewMac::sendFrame(Ieee80211Frame *frame)
{
    Enter_Method("sendFrame()");
    take(frame);
    configureRadioMode(IRadio::RADIO_MODE_TRANSMITTER);
    sendDown(frame);
}

void Ieee80211NewMac::sendDownPendingRadioConfigMsg()
{
    if (pendingRadioConfigMsg != NULL) {
        sendDown(pendingRadioConfigMsg);
        pendingRadioConfigMsg = NULL;
    }
}

// FIXME
bool Ieee80211NewMac::handleNodeStart(IDoneCallback *doneCallback)
{
    if (!doneCallback)
        return true; // do nothing when called from initialize() //FIXME It's a hack, should remove the initializeQueueModule() and setRadioMode() calls from initialize()

    bool ret = MACProtocolBase::handleNodeStart(doneCallback);
//    initializeQueueModule(); TODO
    radio->setRadioMode(IRadio::RADIO_MODE_RECEIVER);
    return ret;
}

// FIXME
bool Ieee80211NewMac::handleNodeShutdown(IDoneCallback *doneCallback)
{
    bool ret = MACProtocolBase::handleNodeStart(doneCallback);
    handleNodeCrash();
    return ret;
}

// FIXME
void Ieee80211NewMac::handleNodeCrash()
{
}

} // namespace ieee80211
} // namespace inet

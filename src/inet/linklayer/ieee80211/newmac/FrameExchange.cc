//
// Copyright (C) 2015 Andras Varga
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
//
// Author: Andras Varga
//

#include "FrameExchange.h"
#include "IUpperMacContext.h"
#include "UpperMac.h"
#include "ITx.h"
#include "inet/linklayer/ieee80211/mac/Ieee80211Frame_m.h"

namespace inet {
namespace ieee80211 {

void FrameExchange::reportSuccess()
{
    finishedCallback->frameExchangeFinished(this, true);
}

void FrameExchange::reportFailure()
{
    finishedCallback->frameExchangeFinished(this, false);
}

//--------

Ieee80211StepBasedFrameExchange::Ieee80211StepBasedFrameExchange(cSimpleModule *ownerModule, IUpperMacContext *context, IFinishedCallback *callback) :
        FrameExchange(ownerModule, context, callback)
{
}

Ieee80211StepBasedFrameExchange::~Ieee80211StepBasedFrameExchange()
{
    if (timeoutMsg)
        cancelAndDelete(timeoutMsg);
}

std::string Ieee80211StepBasedFrameExchange::info() const
{
    std::stringstream out;
    switch (status) {
        case SUCCEEDED: out << "SUCCEEDED in step " << step; break;
        case FAILED: out << "FAILED in step " << step; break;
        case INPROGRESS: out << "step " << step << ", operation=" << stepTypeName(stepType); break;
    }
    return out.str();
}

const char *Ieee80211StepBasedFrameExchange::statusName(Status status)
{
#define CASE(x) case x: return #x
    switch (status) {
        CASE(SUCCEEDED);
        CASE(FAILED);
        CASE(INPROGRESS);
        default: return "???";
    }
#undef CASE
}

const char *Ieee80211StepBasedFrameExchange::stepTypeName(StepType stepType)
{
#define CASE(x) case x: return #x
    switch (stepType) {
        CASE(NONE);
        CASE(TRANSMIT_CONTENTION_FRAME);
        CASE(TRANSMIT_IMMEDIATE_FRAME);
        CASE(EXPECT_REPLY);
        CASE(GOTO_STEP);
        CASE(FAIL);
        CASE(SUCCEED);
        default: return "???";
    }
#undef CASE
}

const char *Ieee80211StepBasedFrameExchange::operationName(StepType stepType)
{
    switch (stepType) {
        case NONE: return "n/a";
        case TRANSMIT_CONTENTION_FRAME: return "transmitContentionFrame()";
        case TRANSMIT_IMMEDIATE_FRAME: return "transmitImmediateFrame()";
        case EXPECT_REPLY: return "expectReply()";
        case GOTO_STEP: return "gotoStep()";
        case FAIL: return "fail()";
        case SUCCEED: return "succeed()";
        default: return "???";
    }
}

void Ieee80211StepBasedFrameExchange::start()
{
    ASSERT(step == 0);
    step--;  // will be incremented in proceed()
    proceed();
}

void Ieee80211StepBasedFrameExchange::proceed()
{
    if (status == INPROGRESS) {
        step++;
        stepType = NONE;
        doStep(step);
        if (status == INPROGRESS) {
            if (stepType == NONE)
                throw cRuntimeError(this, "doStep(step=%d) should have executed an operation like transmitContentionFrame(), transmitImmediateFrame(), expectReply(); gotoStep(), fail(), or succeed()", step);
            if (stepType == GOTO_STEP) {
                step--;
                proceed();
            }
       }
    }
}

bool Ieee80211StepBasedFrameExchange::lowerFrameReceived(Ieee80211Frame* frame)
{
    if (stepType != EXPECT_REPLY)
        return false;  // not ready to process frames
    else {
        bool accepted = processReply(step, frame);
        if (!accepted)
            return false;  // not for us
        else {
            proceed();
            return true;
        }
    }
}

void Ieee80211StepBasedFrameExchange::transmissionFinished()
{
    ASSERT(status == INPROGRESS);
    ASSERT(stepType == TRANSMIT_CONTENTION_FRAME || stepType == TRANSMIT_IMMEDIATE_FRAME);
    proceed();
}

void Ieee80211StepBasedFrameExchange::handleMessage(cMessage* msg)
{
    if (status != INPROGRESS)
        return;  // too late, frame exchange already finished
    ASSERT(msg == timeoutMsg);
    processTimeout(step);
    proceed();
}

void Ieee80211StepBasedFrameExchange::transmitContentionFrame(Ieee80211Frame* frame, int retryCount)
{
    setOperation(TRANSMIT_CONTENTION_FRAME);
    int txIndex = 0; //TODO
    context->transmitContentionFrame(txIndex, frame, context->getDIFS(), context->getEIFS(), context->getMinCW(), context->getMaxCW(), context->getSlotTime(), retryCount, this);
}

void Ieee80211StepBasedFrameExchange::transmitContentionFrame(Ieee80211Frame* frame, simtime_t ifs, simtime_t eifs, int cwMin, int cwMax, simtime_t slotTime, int retryCount)
{
    setOperation(TRANSMIT_CONTENTION_FRAME);
    int txIndex = 0; //TODO
    context->transmitContentionFrame(txIndex, frame, ifs, eifs, cwMin, cwMax, slotTime, retryCount, this);
}

void Ieee80211StepBasedFrameExchange::transmitImmediateFrame(Ieee80211Frame* frame, simtime_t ifs)
{
    setOperation(TRANSMIT_IMMEDIATE_FRAME);
    context->transmitImmediateFrame(frame, ifs, this);
}

void Ieee80211StepBasedFrameExchange::expectReply(simtime_t timeout)
{
    setOperation(EXPECT_REPLY);
    if (!timeoutMsg)
        timeoutMsg = new cMessage("timeout");
    scheduleAt(simTime() + timeout, timeoutMsg);
}

void Ieee80211StepBasedFrameExchange::gotoStep(int step)
{
    setOperation(GOTO_STEP);
    this->step = step;
}

void Ieee80211StepBasedFrameExchange::fail()
{
    setOperation(FAIL);
    status = FAILED;
    reportFailure();
    cleanup();
}

void Ieee80211StepBasedFrameExchange::succeed()
{
    setOperation(SUCCEED);
    status = SUCCEEDED;
    reportSuccess();
    cleanup();
}

void Ieee80211StepBasedFrameExchange::setOperation(StepType newStepType)
{
    if (status != INPROGRESS)
        throw cRuntimeError(this, "cannot do operation %s: frame exchange already terminated (%s)", operationName(newStepType), statusName(status));
    if (stepType != NONE)
        throw cRuntimeError(this, "only one operation is permitted per step: cannot do %s after a %s, in doStep(step=%d)", operationName(newStepType), operationName(stepType), step);
    stepType = newStepType;
}

void Ieee80211StepBasedFrameExchange::cleanup()
{
    if (timeoutMsg)
        cancelEvent(timeoutMsg);
}


} // namespace ieee80211
} // namespace inet

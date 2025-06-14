#include "FsrRouting.h"
#include <inet/common/ModuleAccess.h>
#include <inet/common/ProtocolTag_m.h>
#include <inet/common/TimeTag_m.h>
#include <inet/transportlayer/contract/udp/UdpControlInfo_m.h>
#include <inet/networklayer/ipv4/Ipv4Header_m.h>

Define_Module(FsrRouting);

FsrRouting::FsrRouting()
{
    for (int i = 0; i < SCOPE_LEVELS; ++i)
        scopeTimer[i] = nullptr;
}

FsrRouting::~FsrRouting()
{
    for (int i = 0; i < SCOPE_LEVELS; ++i)
        cancelAndDelete(scopeTimer[i]);
    socket.close();
}

void FsrRouting::initialize(int stage)
{
    RoutingProtocolBase::initialize(stage);
    if (stage == inet::INITSTAGE_LOCAL) {
        // obtain pointers
        routingTable.reference(this, "routingTableModule", true);
        interfaceTable.reference(this, "interfaceTableModule", true);

        udpPort = par("udpPort");
    }
}

void FsrRouting::handleMessageWhenUp(cMessage *msg)
{
    if (msg->isSelfMessage()) {
        handleSelfMessage(msg);
    }
    else if (auto packet = dynamic_cast<Packet*>(msg)) {
        // UDP data or control packets
        if (packet->getTag<PacketProtocolTag>()->getProtocol() == &Protocol::udp) {
            socket.processMessage(packet);
        }
        else {
            processRoutingPacket(packet);
        }
    }
}

void FsrRouting::handleSelfMessage(cMessage *msg)
{
    // find which scope timer triggered
    for (int i = 0; i < SCOPE_LEVELS; ++i) {
        if (msg == scopeTimer[i]) {
            sendScopeUpdate(i);
            scheduleAt(simTime() + scopeInterval[i], scopeTimer[i]);
            break;
        }
    }
}

void FsrRouting::sendScopeUpdate(int level)
{
    // TODO: Construct and send a link-state packet via UDP
    // Hint: create a Packet, attach control info, set TTL=scopeRadius[level],
    // then use socket.sendTo(packet, L3Address::BROADCAST_ADDRESS, destPort);
}

void FsrRouting::processRoutingPacket(Packet *pkt)
{
    // TODO: extract sequence number and neighbor list from pkt,
    // update topologyDB, then call computeRoutes();
    computeRoutes();
    delete pkt;
}

void FsrRouting::computeRoutes()
{
    // TODO: run Dijkstra over topologyDB, fill nextHop map
}

// UDP callback: data arrived from socket
void FsrRouting::socketDataArrived(UdpSocket *socket, Packet *packet)
{
    processRoutingPacket(packet);
}

void FsrRouting::socketErrorArrived(UdpSocket *socket, Indication *indication)
{
    // ignore or log
    delete indication;
}

void FsrRouting::socketClosed(UdpSocket *socket)
{
    // cleanup if needed
}


/* Lifecycle */
void handleStartOperation(inet::LifecycleOperation *operation)
{
    // bind UDP socket
    socket.setOutputGate(gate("socketOut"));
    socket.bind(udpPort);
    socket.setCallback(this);
    socket.setBroadcast(true);

    // schedule scope timers
    for (int i = 0; i < SCOPE_LEVELS; ++i) {
        scopeInterval[i] = par("scope" + std::to_string(i) + "Interval").doubleValue();
        scopeRadius[i]   = par("scope" + std::to_string(i) + "Radius").intValue();
        scopeTimer[i]    = new cMessage("scopeTimer");
        scheduleAt(simTime() + scopeInterval[i], scopeTimer[i]);
    }
}

void handleStopOperation(inet::LifecycleOperation *operation)
{
    socket.close();
    clearState();
}

void handleCrashOperation(inet::LifecycleOperation *operation)
{
    socket.destroy();
    clearState();
}

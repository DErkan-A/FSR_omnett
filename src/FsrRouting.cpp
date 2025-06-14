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

void FsrRouting::handleMessageWhenUp(inet::cMessage *msg)
{
    if (msg->isSelfMessage()) {
        handleSelfMessage(msg);
    }
    else if (auto packet = dynamic_cast<inet::Packet*>(msg)) {
        // UDP data or control packets
        if (packet->getTag<inet::PacketProtocolTag>()->getProtocol() == &inet::Protocol::udp) {
            socket.processMessage(packet);
        }
        else {
            processRoutingPacket(packet);
        }
    }
}

void FsrRouting::handleSelfMessage(inet::cMessage *msg)
{
    // find which scope timer triggered
    for (int i = 0; i < SCOPE_LEVELS; ++i) {
        if (msg == scopeTimer[i]) {
            sendScopeUpdate(i);
            scheduleAt(inet::simTime() + scopeInterval[i], scopeTimer[i]);
            break;
        }
    }
}

void FsrRouting::sendScopeUpdate(int level)
{
    // TODO: Construct and send a link-state packet via UDP
    // Hint: create a Packet, attach control info, set TTL=scopeRadius[level],
    // then use socket.sendTo(packet, L3Address::BROADCAST_ADDRESS, destPort);

    // 1) Build the FsrPacket chunk
        auto fsrChunk = inet::makeShared<FsrMessage>();
        fsrChunk->setSequenceNumber(sequenceNumber++);
        fsrChunk->setScopeLevel(level);

        // origin = this node’s interface address
        const char *ifname = par("interface").stringValue();
        auto ie = interfaceTable->findInterfaceByName(ifname);
        fsrChunk->setOrigin(ie->getNetworkAddress());

        // fill neighbors list
        auto neighbors = getOneHopNeighbors();  // your function returning vector<L3Address>
        int n = neighbors.size();
        fsrChunk->setNeighborsArraySize(n);
        for (int i = 0; i < n; ++i)
            fsrChunk->setNeighbors(i, neighbors[i]);

        // 2) Wrap it in a Packet
        auto packet = new Packet("FSR-Update");
        packet->insertAtBack(fsrChunk);

        // 3) Annotate as UDP and set IP TTL = scopeRadius[level]
        packet->addTag<PacketProtocolTag>()->setProtocol(&Protocol::udp);
        packet->addTagIfAbsent<HopLimitReq>()->setHopLimit(scopeRadius[level]);

        // 4) Broadcast on UDP destPort
        L3Address bcast = Ipv4Address::ALLONES_ADDRESS;
        socket.sendTo(packet, bcast, destPort);
}

void FsrRouting::processRoutingPacket(inet::Packet *packet)
{
    // TODO: extract sequence number and neighbor list from pkt,
    // update topologyDB, then call computeRoutes();

    auto arrivalPacketTime = packet->getArrivalTime();
    unsigned int arrivalPacketTTL  = packet->getTag<HopLimitInd>()->getHopLimit() - 1;
    const auto& fsrPacket = packet->popAtFront<FsrPacket>();
    if (!fsrPacket) {
           delete packet;
           return;
       }

    //First check the origin of the message to compare sequence numbers
    auto seq  = fsrPacket->getSequenceNumber();
    auto origin     = fsrPacket->getOrigin();
    if (seq > topologyDB[origin].seqNum) {
        // extract the other relevant information
       auto scopeLevel = fsrPacket->getScopeLevel();
       int  numNbrs    = fsrPacket->getNeighborsArraySize();

       if(scopeLevel == 1){



       }
       else{
       // 3) build a set of neighbors
       std::set<inet::L3Address> nbrs;
       for (int i = 0; i < numNbrs; ++i)
           nbrs.insert(fsrPacket->getNeighbors(i));

       // 4) update topologyDB
       auto &rec = topologyDB[origin];
       rec.seqNum    = seq;
       rec.neighbors = std::move(nbrs);
       // 5) recompute routes whenever topology changes
       computeRoutes();
       }
       // 6) cleanup
       delete packet;
    }


}

//Function taken from AODV library
inet::L3Address FsrRouting::getSelfIPAddress() const
{
    return routingTable->getRouterIdAsGeneric();
}

void FsrRouting::computeRoutes()
{
    // TODO: run Dijkstra over topologyDB, fill nextHop map
}

// UDP callback: data arrived from socket
void FsrRouting::socketDataArrived(inet::UdpSocket *socket, inet::Packet *packet)
{
    //Like in AODV call the process function
    processRoutingPacket(packet);
}

void FsrRouting::socketErrorArrived(inet::UdpSocket *socket, inet::Indication *indication)
{
    // ignore or log
    delete indication;
}

void FsrRouting::socketClosed(inet::UdpSocket *socket)
{
    // cleanup if needed
}

void FsrRouting::clearState()
{
    // Cancel and delete any scheduled scope‐update timers
    for (int i = 0; i < SCOPE_LEVELS; ++i) {
        if (scopeTimer[i] != nullptr) {
            cancelAndDelete(scopeTimer[i]);
            scopeTimer[i] = nullptr;
        }
    }

    // Reset sequence numbering and topology
    sequenceNumber = 0;
    topologyDB.clear();
    nextHop.clear();
}


/* Lifecycle */
void FsrRouting::handleStartOperation(inet::LifecycleOperation *operation)
{
    // bind UDP socket
    socket.setOutputGate(gate("socketOut"));
    socket.bind(udpPort);
    socket.setCallback(this);
    socket.setBroadcast(true);

    // schedule scope timers
    for (int i = 0; i < SCOPE_LEVELS; ++i) {
        std::string intervalName = "scope" + std::to_string(i) + "Interval";
        std::string radiusName   = "scope" + std::to_string(i) + "Radius";

        scopeInterval[i] = par(intervalName.c_str()).doubleValue();
        scopeRadius[i]   = par(radiusName.c_str()).intValue();
        scopeTimer[i]    = new inet::cMessage("scopeTimer");

        scheduleAt(inet::simTime() + scopeInterval[i], scopeTimer[i]);
    }
}

void FsrRouting::handleStopOperation(inet::LifecycleOperation *operation)
{
    socket.close();
    clearState();
}

void FsrRouting::handleCrashOperation(inet::LifecycleOperation *operation)
{
    socket.destroy();
    clearState();
}

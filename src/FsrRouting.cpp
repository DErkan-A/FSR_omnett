#include "FsrRouting.h"
#include <queue>
#include <limits>

#include <inet/common/Units.h>
#include <inet/linklayer/common/InterfaceTag_m.h>
#include <inet/networklayer/common/L3AddressResolver.h>
#include <inet/networklayer/contract/IL3AddressType.h>
#include <inet/networklayer/common/L3AddressTag_m.h>
#include <inet/transportlayer/common/L4PortTag_m.h>
#include <inet/networklayer/common/HopLimitTag_m.h>
#include <inet/common/ModuleAccess.h>
#include <inet/common/ProtocolTag_m.h>
#include <inet/common/TimeTag_m.h>
#include <inet/transportlayer/contract/udp/UdpControlInfo_m.h>
#include <inet/networklayer/ipv4/Ipv4Header_m.h>

Define_Module(FsrRouting);

FsrRouting::FsrRouting()
{
    for (int i = 0; i < SCOPE_LEVELS; i++)
        scopeTimer[i] = nullptr;
}

FsrRouting::~FsrRouting()
{
    clearState();
}

void FsrRouting::initialize(int stage)
{
    RoutingProtocolBase::initialize(stage);
    if (stage == inet::INITSTAGE_LOCAL) {
        // obtain pointers
        routingTable.reference(this, "routingTableModule", true);
        interfaceTable.reference(this, "interfaceTableModule", true);

        udpPort = par("udpPort");
        maxDelay= par("randomDelay");
    }
}

//Maybe errors here
void FsrRouting::handleMessageWhenUp(inet::cMessage *msg)
{
    if (msg->isSelfMessage()) {
        handleSelfMessage(msg);
    }
    else{
        // UDP data or control packets
        socket.processMessage(msg);
    }
}

void FsrRouting::handleSelfMessage(inet::cMessage *msg)
{
    // find which scope timer triggered
    EV_INFO << "[" << getSelfIPAddress() << "] " << "Handling a self message" <<std::endl;
    for (int i = 0; i < SCOPE_LEVELS; i++) {
        if (msg == scopeTimer[i]) {
            EV_INFO << "[" << getSelfIPAddress() << "] Found the correct event for self message" <<std::endl;
            sendScopeUpdate(i);
            omnetpp::simtime_t delay_amount  = omnetpp::SimTime(uniform(0, maxDelay));
            scheduleAt(inet::simTime() + scopeInterval[i] + delay_amount, scopeTimer[i]);
            break;
        }
    }
}

void FsrRouting::sendScopeUpdate(int level)
{
    // 1) Build the FsrPacket chunk
        auto fsr_packet= inet::makeShared<FsrPacket>();
        fsr_packet->setSequenceNumber(++sequenceNumber);
        fsr_packet->setOrigin(getSelfIPAddress());
        fsr_packet->setScopeLevel(level);

        // origin = this node’s interface address
        const char *ifname = par("interface");
        auto ie = interfaceTable->findInterfaceByName(ifname);
        fsr_packet->setOrigin(ie->getNetworkAddress());

        // fill neighbours list
        auto& neighbours = topologyDB[getSelfIPAddress()].neighbours;
        int n = neighbours.size();
        fsr_packet->setNeighboursArraySize(n);
        int i=0;
        for (auto node_address : neighbours)
            fsr_packet->setNeighbours(i++, node_address);

        fsr_packet->setChunkLength(inet::B(12 + 4 * neighbours.size()));

        //Send packet with ttl equal to scope radius
        EV_INFO << "[" << getSelfIPAddress() << "] Sending a update" <<std::endl;
        sendPacket(fsr_packet, scopeRadius[level]);
}

void FsrRouting::processRoutingPacket(inet::Packet *packet)
{

    bool change_flag = false; // Keeps track of any changes in the tables
    auto arrivalPacketTime = packet->getArrivalTime();
    unsigned int arrivalPacketTTL  = packet->getTag<inet::HopLimitInd>()->getHopLimit() - 1;
    const auto& fsr_packet = packet->popAtFront<FsrPacket>();
    EV_INFO << "[" << getSelfIPAddress() << "] Received a packet, checking for validity..." <<std::endl;
    if (!fsr_packet) {
           delete packet;
           return;
       }

    //First check the origin of the message to compare sequence numbers
    auto seq  = fsr_packet->getSequenceNumber();
    auto origin     = fsr_packet->getOrigin();
    EV_INFO << "[" << getSelfIPAddress() << "] Checking for a sequence number..." <<std::endl;
    if (seq > topologyDB[origin].seqNum) {
        EV_INFO << "[" << getSelfIPAddress() << "] Sequence number matched, calculating topology" <<std::endl;
        // extract the other relevant information
       auto scopeLevel = fsr_packet->getScopeLevel();
       int  numNbrs    = fsr_packet->getNeighboursArraySize();

       // If in the scopeLevel 0 meaning direct neighbour, update our own neighbour list
       if(scopeLevel == 0){
           auto& my_neighbours = topologyDB[getSelfIPAddress()].neighbours;
           auto result = my_neighbours.insert(origin);
           EV_INFO << "[" << getSelfIPAddress() << "] Inserted "<<origin <<" as a neighbour!"<<std::endl;
           // If we added a new neighbour update the change flag
           change_flag = change_flag | result.second;
       }

       // build a set of neighbours to insert into the topology
       std::set<inet::L3Address> update_nbrs;
       for (int i = 0; i < numNbrs; i++){
           EV_INFO << "[" << getSelfIPAddress() << "] Neighbours of "<<origin <<":"<<fsr_packet->getNeighbours(i)<<std::endl;
           update_nbrs.insert(fsr_packet->getNeighbours(i));
       }

       //Update topologyDB
       if (topologyDB[origin].neighbours != update_nbrs) {
               topologyDB[origin].neighbours = std::move(update_nbrs);
               topologyDB[origin].timestamp = arrivalPacketTime;
               topologyDB[origin].seqNum = seq;
               change_flag = true;
               EV_INFO << "[" << getSelfIPAddress() << "] Updated "<<origin <<"'s neighbour list into the topology"<<std::endl;
           }
       //Recompute routes whenever topology changes
       if(change_flag){
           EV_INFO << "[" << getSelfIPAddress() << "] Recomputing the routes"<<std::endl;
           computeRoutes();
       }

        if(arrivalPacketTTL>0){
            sendPacket(fsr_packet,arrivalPacketTTL);
        }

    }

    // Cleanup regardless of anything
    delete packet;

}

void FsrRouting::sendPacket(const inet::Ptr<const FsrPacket>& fsr_packet, int fsr_TTL){

auto packet_to_send = new inet::Packet("fsr_packet",fsr_packet);
packet_to_send->addTag<inet::HopLimitReq>()->setHopLimit(fsr_TTL);
int send_interface_id = CHK(interfaceTable->findInterfaceByName(par("interface")))->getInterfaceId();

packet_to_send->addTag<inet::InterfaceReq>()->setInterfaceId(send_interface_id);

packet_to_send->addTag<inet::L3AddressReq>()->setDestAddress(getSelfIPAddress().getAddressType()->getBroadcastAddress());
packet_to_send->addTag<inet::L4PortReq>()->setDestPort(udpPort);

socket.send(packet_to_send);

}

//Function taken from AODV library
inet::L3Address FsrRouting::getSelfIPAddress() const
{
    return routingTable->getRouterIdAsGeneric();
}

void FsrRouting::computeRoutes()
{
    // 0) Delete the stale entries
    handleStaleEntries();
    // 1) clear the old next‐hop table
    for (int i = routingTable->getNumRoutes() - 1; i >= 0; --i) {
            auto *oldRoute = routingTable->getRoute(i);
            if (oldRoute->getSourceType() == FSR_ROUTE_TYPE)
                routingTable->deleteRoute(oldRoute);
        }

    // 2) our “source” node
    const auto self_address = getSelfIPAddress();

    // 3) prepare Dijkstra structures
    const int INF = std::numeric_limits<int>::max();
    std::map<inet::L3Address,int> dist;
    std::map<inet::L3Address,inet::L3Address> prev;
    std::set<inet::L3Address> visited;

    // initialize all known nodes to “infinite” distance
    for (auto &kv : topologyDB) {
        auto u = kv.first;
        dist[u] = INF;
        // also declare each neighbour so dist[nbr] is INF, not 0
        for (auto &nbr : kv.second.neighbours) {
            if (dist.find(nbr) == dist.end())
                dist[nbr] = INF;
        }
    }
    if (dist.find(self_address) == dist.end())
        dist[self_address] = INF;
    dist[self_address] = 0;

    // min‐heap of (distance, node)
    using Pair = std::pair<int,inet::L3Address>;
    //This will be used to select the minimum distance node
    auto cmp = [](const Pair &a, const Pair &b){ return a.first > b.first; };
    std::priority_queue<Pair, std::vector<Pair>, decltype(cmp)> pq(cmp);
    pq.push({0, self_address});

    // 4) main Dijkstra loop
    while (!pq.empty()) {
        auto [d,u] = pq.top(); pq.pop();
        if (visited.count(u)) continue;
        visited.insert(u);

        EV_INFO << "[" << getSelfIPAddress() << "] Djikstra current node: "<<u<<std::endl;

        // if this node has no links, give up on it
        if (topologyDB[u].neighbours.empty())
            continue;

        // relax all of u’s neighbors
        for (auto &v : topologyDB[u].neighbours) {
            EV_INFO << "[" << getSelfIPAddress() << "] Djikstra current node: "<<u <<" Its neighbour "<<v<<std::endl;
            if (!visited.count(v) && (d + 1 < dist[v])) {
                EV_INFO << "[" << getSelfIPAddress() << "] Djikstra found a route to "<<v<<std::endl;
                dist[v] = d + 1;
                prev[v] = u;
                pq.push({dist[v], v});
            }
        }
    }

    // 5) build nextHop: for each reachable dest ≠ self, walk back to find the first hop
    auto addrType = self_address.getAddressType();
    for (auto &p : dist) {
        auto dest = p.first;
        auto d    = p.second;
        if (dest == self_address || d == INF)
            continue;
        // walk predecessors until we hit self
        inet::L3Address hop = dest;
        while (true) {
            auto it = prev.find(hop);
            if (it == prev.end()) {
                //EV_WARN << "computeRoutes: no predecessor for " << hop<< ", giving up on route to " << dest << endl;
                break;
            }
            if (it->second == self_address) {
                // we found the first hop
                break;
            }
            hop = it->second;
        }
        // create & populate the route object

        EV_INFO << "[" << getSelfIPAddress() << "] Adding a route to "<<dest <<" via "<<hop<<std::endl;
        auto route = routingTable->createRoute();
        route->setNextHop(hop);
        route->setDestination(dest);
        route->setMetric(d);
        route->setPrefixLength(addrType->getMaxPrefixLength());
        if (auto ifEntry = interfaceTable->findInterfaceByName(par("interface")))
            route->setInterface(ifEntry);
        route->setSourceType(FSR_ROUTE_TYPE);
        route->setSource(this);

        // finally add it to the table
        routingTable->addRoute(route);
    }

    /* Print routes */
       inet::L3AddressResolver resolver;
       auto myName = getParentModule()->getFullName();

       for (size_t i = 0 ; i < routingTable->getNumRoutes() ; ++i) {
           auto route = routingTable->getRoute(i);
           if (route->getSourceType() != FSR_ROUTE_TYPE) continue;

           auto dest = resolver.findHostWithAddress(route->getDestinationAsGeneric())->getFullName();
           auto hop  = resolver.findHostWithAddress(route->getNextHopAsGeneric())->getFullName();

           EV_INFO << "[" << myName << "] " << "Route to: \"" << dest << "\" via \"" << hop << "\" metric: " << route->getMetric() << std::endl;
       }
}
// Remove any neighbor whose last‐heard‐from timestamp is older than scopeInterval[0]*5
void FsrRouting::handleStaleEntries()
{
    const auto self_address = getSelfIPAddress();
    auto &myNeighbours = topologyDB[self_address].neighbours;
    // threshold = 5 × the level-0 interval
    omnetpp::simtime_t staleThreshold = scopeInterval[0] * 5;
    omnetpp::simtime_t now = omnetpp::simTime();

    // 1) collect all neighbours that are too old
    std::vector<inet::L3Address> toRemove;
    for (const auto &nbr : myNeighbours) {
        auto it = topologyDB.find(nbr);
        // either no record (sanity check) or record is stale
        if (it == topologyDB.end() || (now - it->second.timestamp) > staleThreshold) {
            toRemove.push_back(nbr);
        }
    }

    // 2) erase them from both our neighbour set and the topologyDB
    for (const auto &nbr : toRemove) {
        myNeighbours.erase(nbr);
        topologyDB.erase(nbr);
        EV_INFO << "I am node "<<self_address<<", removed stale entry for " << nbr << std::endl;
    }
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
    EV_WARN << "Ignoring the UDP error as I cannot bother" << indication->getName() << std::endl;
    delete indication;
}

void FsrRouting::socketClosed(inet::UdpSocket *socket)
{
    // cleanup if needed
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
    for (int i = 0; i < SCOPE_LEVELS; i++) {
        std::string intervalName = "scope" + std::to_string(i) + "Interval";
        std::string radiusName   = "scope" + std::to_string(i) + "Radius";

        scopeInterval[i] = par(intervalName.c_str());
        scopeRadius[i]   = par(radiusName.c_str());
        scopeTimer[i]    = new inet::cMessage("scopeTimer");

        omnetpp::simtime_t delay_amount  = omnetpp::SimTime(uniform(0, maxDelay));

        scheduleAt(inet::simTime() + scopeInterval[i] + delay_amount, scopeTimer[i]);
    }
}

void FsrRouting::clearState()
{
    // Cancel and delete any scheduled scope‐update timers
    for (int i = 0; i < SCOPE_LEVELS; i++) {
        if (scopeTimer[i] != nullptr) {
            cancelAndDelete(scopeTimer[i]);
            scopeTimer[i] = nullptr;
        }
    }

    // Reset sequence numbering and topology
    sequenceNumber = 0;
    topologyDB.clear();
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

#ifndef __FSR_ROUTING_H__
#define __FSR_ROUTING_H__

#include <map>
#include <vector>

#include "inet/common/INETDefs.h"
#include "inet/common/ModuleRefByPar.h"
#include "inet/networklayer/contract/IRoutingTable.h"
#include "inet/networklayer/contract/IInterfaceTable.h"
#include "inet/routing/base/RoutingProtocolBase.h"
#include "inet/transportlayer/contract/udp/UdpSocket.h"
#include "inet/networklayer/ipv4/Ipv4Header_m.h"

#include "FsrMessages_m.h"

class FsrRouting : public inet::RoutingProtocolBase,
                   public inet::UdpSocket::ICallback
{
  protected:
    // References to the routing table and interface table
    inet::ModuleRefByPar<inet::IRoutingTable>   routingTable;
    inet::ModuleRefByPar<inet::IInterfaceTable> interfaceTable;

    // UDP socket for data/hello packets
    inet::UdpSocket socket;
    unsigned int udpPort = 0;

    // Fisheye parameters (3 scope levels)
    static const int SCOPE_LEVELS = 3;
    omnetpp::simtime_t scopeInterval[SCOPE_LEVELS];
    int               scopeRadius  [SCOPE_LEVELS];
    omnetpp::cMessage* scopeTimer   [SCOPE_LEVELS] = {nullptr, nullptr, nullptr};

    // Link‐state sequence number
    unsigned int sequenceNumber = 0;

    // Topology database entry for each origin
    struct LinkStateRecord {
        inet::L3Address                   origin;
        unsigned int                      seqNum;
        std::vector<inet::L3Address>      neighbors;
    };
    std::map<inet::L3Address, LinkStateRecord> topologyDB;

    // Computed next‐hop table: destination → next hop
    std::map<inet::L3Address, inet::L3Address> nextHop;

    // Lifecycle hooks
    virtual int  numInitStages() const override { return inet::NUM_INIT_STAGES; }
    virtual void initialize(int stage) override;
    virtual void handleMessageWhenUp(omnetpp::cMessage *msg) override;
    void handleStartOperation(inet::LifecycleOperation *operation) override;
    void handleStopOperation(inet::LifecycleOperation *operation) override;
    void handleCrashOperation(inet::LifecycleOperation *operation) override;

    // UDP callbacks
    virtual void socketDataArrived(inet::UdpSocket *socket, inet::Packet *packet) override;
    virtual void socketErrorArrived(inet::UdpSocket *socket, inet::Indication *indication) override;
    virtual void socketClosed(inet::UdpSocket *socket) override;

    // Core FSR routines
    virtual void handleSelfMessage(omnetpp::cMessage *msg);
    virtual void sendScopeUpdate(int level);
    virtual void processRoutingPacket(inet::Packet *pkt);
    virtual void computeRoutes();

  public:
    FsrRouting();
    virtual ~FsrRouting();
};

#endif // __FSR_ROUTING_H__

#ifndef __FSR_ROUTING_H__
#define __FSR_ROUTING_H__

#include <map>
#include <vector>
#include <set>

#include "inet/common/INETDefs.h"
#include "inet/common/ModuleRefByPar.h"
#include "inet/networklayer/contract/IRoutingTable.h"
#include "inet/networklayer/contract/IInterfaceTable.h"
#include "inet/routing/base/RoutingProtocolBase.h"
#include "inet/transportlayer/contract/udp/UdpSocket.h"
#include "inet/networklayer/ipv4/Ipv4Header_m.h"
#include <inet/networklayer/common/L3Address.h>

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
    double maxDelay= 1;

    // Link‐state sequence number
    unsigned int sequenceNumber = 0;

    // Topology database entry for each origin
    struct LinkStateRecord {
        omnetpp::simtime_t                timestamp;
        unsigned int                      seqNum;
        int                               scopeLevel;
        std::set<inet::L3Address>      neighbours;
    };
    std::map<inet::L3Address, LinkStateRecord> topologyDB;

    // Lifecycle hooks
    virtual int  numInitStages() const override { return inet::NUM_INIT_STAGES; }
    void initialize(int stage) override;
    void handleMessageWhenUp(omnetpp::cMessage *msg) override;
    void handleStartOperation(inet::LifecycleOperation *operation) override;
    void handleStopOperation(inet::LifecycleOperation *operation) override;
    void handleCrashOperation(inet::LifecycleOperation *operation) override;
    void clearState();


    // UDP callbacks
    void socketDataArrived(inet::UdpSocket *socket, inet::Packet *packet) override;
    void socketErrorArrived(inet::UdpSocket *socket, inet::Indication *indication) override;
    void socketClosed(inet::UdpSocket *socket) override;

    // Core FSR routines
    void handleSelfMessage(omnetpp::cMessage *msg);
    void sendScopeUpdate(int level);
    void processRoutingPacket(inet::Packet *packet);
    void computeRoutes();
    void handleStaleEntries();
    //Helpers
    inet::L3Address getSelfIPAddress() const;
    void sendPacket(const inet::Ptr<const FsrPacket>& fsr_packet, int fsr_TTL);

  public:
    static constexpr inet::IRoute::SourceType FSR_ROUTE_TYPE = inet::IRoute::SourceType::MANUAL;
    FsrRouting();
    virtual ~FsrRouting();
};

#endif // __FSR_ROUTING_H__

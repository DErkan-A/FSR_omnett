package fsr_omnett;

// Simple module for Fisheye State Routing (FSR)
// Parameters correspond to the FsrRouting.h header
simple Fsr
{
    parameters:
        // Module annotations
        @class("FsrRouting");           // C++ implementation class
        @display("i=block/routing");    // icon in Tkenv
        // References to INET tables
        string routingTableModule = default("^.ipv4.routingTable");
        string interfaceTableModule  = default("^.interfaceTable");

        // Fisheye scope update intervals (3 levels)
        double scope0Interval @unit(s) = default(100ms);
        double scope1Interval @unit(s) = default(200ms);
        double scope2Interval @unit(s) = default(400ms);
        double randomDelay @unit(s) = default(50ms);

        // Fisheye scope radii (TTL in hops)
        int scope0Radius = default(1);
        int scope1Radius = default(3);
        int scope2Radius = default(50);

        // UDP socket port
        int udpPort = default(654);
   		string interface = default("wlan0");
        
        gates:
        input socketIn;
        output socketOut;

}
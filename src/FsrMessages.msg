import inet.networklayer.common.L3Address;

// Link-state update packet for Fisheye State Routing
packet FsrMessage
{
    int sequenceNumber;      // incremental sequence number
    int scopeLevel;          // which fisheye scope (0,1,2)
    inet::L3Address origin;        // originator’s address
    inet::L3Address neighbors[];
}
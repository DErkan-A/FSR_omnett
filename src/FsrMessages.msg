import inet.common.packet.chunk.Chunk;
import inet.networklayer.common.L3Address;


class FsrPacket extends inet::FieldsChunk
{
    unsigned int sequenceNumber;      // incremental sequence number
    int scopeLevel;          // which fisheye scope (0,1,2)
    inet::L3Address origin;        // originator’s address
    inet::L3Address neighbors[];   // dynamic array of one-hop neighbors
}
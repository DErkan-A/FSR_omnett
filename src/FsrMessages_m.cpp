//
// Generated file, do not edit! Created by opp_msgtool 6.1 from FsrMessages.msg.
//

// Disable warnings about unused variables, empty switch stmts, etc:
#ifdef _MSC_VER
#  pragma warning(disable:4101)
#  pragma warning(disable:4065)
#endif

#if defined(__clang__)
#  pragma clang diagnostic ignored "-Wshadow"
#  pragma clang diagnostic ignored "-Wconversion"
#  pragma clang diagnostic ignored "-Wunused-parameter"
#  pragma clang diagnostic ignored "-Wc++98-compat"
#  pragma clang diagnostic ignored "-Wunreachable-code-break"
#  pragma clang diagnostic ignored "-Wold-style-cast"
#elif defined(__GNUC__)
#  pragma GCC diagnostic ignored "-Wshadow"
#  pragma GCC diagnostic ignored "-Wconversion"
#  pragma GCC diagnostic ignored "-Wunused-parameter"
#  pragma GCC diagnostic ignored "-Wold-style-cast"
#  pragma GCC diagnostic ignored "-Wsuggest-attribute=noreturn"
#  pragma GCC diagnostic ignored "-Wfloat-conversion"
#endif

#include <iostream>
#include <sstream>
#include <memory>
#include <type_traits>
#include "FsrMessages_m.h"

namespace omnetpp {

// Template pack/unpack rules. They are declared *after* a1l type-specific pack functions for multiple reasons.
// They are in the omnetpp namespace, to allow them to be found by argument-dependent lookup via the cCommBuffer argument

// Packing/unpacking an std::vector
template<typename T, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::vector<T,A>& v)
{
    int n = v.size();
    doParsimPacking(buffer, n);
    for (int i = 0; i < n; i++)
        doParsimPacking(buffer, v[i]);
}

template<typename T, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::vector<T,A>& v)
{
    int n;
    doParsimUnpacking(buffer, n);
    v.resize(n);
    for (int i = 0; i < n; i++)
        doParsimUnpacking(buffer, v[i]);
}

// Packing/unpacking an std::list
template<typename T, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::list<T,A>& l)
{
    doParsimPacking(buffer, (int)l.size());
    for (typename std::list<T,A>::const_iterator it = l.begin(); it != l.end(); ++it)
        doParsimPacking(buffer, (T&)*it);
}

template<typename T, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::list<T,A>& l)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i = 0; i < n; i++) {
        l.push_back(T());
        doParsimUnpacking(buffer, l.back());
    }
}

// Packing/unpacking an std::set
template<typename T, typename Tr, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::set<T,Tr,A>& s)
{
    doParsimPacking(buffer, (int)s.size());
    for (typename std::set<T,Tr,A>::const_iterator it = s.begin(); it != s.end(); ++it)
        doParsimPacking(buffer, *it);
}

template<typename T, typename Tr, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::set<T,Tr,A>& s)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i = 0; i < n; i++) {
        T x;
        doParsimUnpacking(buffer, x);
        s.insert(x);
    }
}

// Packing/unpacking an std::map
template<typename K, typename V, typename Tr, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::map<K,V,Tr,A>& m)
{
    doParsimPacking(buffer, (int)m.size());
    for (typename std::map<K,V,Tr,A>::const_iterator it = m.begin(); it != m.end(); ++it) {
        doParsimPacking(buffer, it->first);
        doParsimPacking(buffer, it->second);
    }
}

template<typename K, typename V, typename Tr, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::map<K,V,Tr,A>& m)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i = 0; i < n; i++) {
        K k; V v;
        doParsimUnpacking(buffer, k);
        doParsimUnpacking(buffer, v);
        m[k] = v;
    }
}

// Default pack/unpack function for arrays
template<typename T>
void doParsimArrayPacking(omnetpp::cCommBuffer *b, const T *t, int n)
{
    for (int i = 0; i < n; i++)
        doParsimPacking(b, t[i]);
}

template<typename T>
void doParsimArrayUnpacking(omnetpp::cCommBuffer *b, T *t, int n)
{
    for (int i = 0; i < n; i++)
        doParsimUnpacking(b, t[i]);
}

// Default rule to prevent compiler from choosing base class' doParsimPacking() function
template<typename T>
void doParsimPacking(omnetpp::cCommBuffer *, const T& t)
{
    throw omnetpp::cRuntimeError("Parsim error: No doParsimPacking() function for type %s", omnetpp::opp_typename(typeid(t)));
}

template<typename T>
void doParsimUnpacking(omnetpp::cCommBuffer *, T& t)
{
    throw omnetpp::cRuntimeError("Parsim error: No doParsimUnpacking() function for type %s", omnetpp::opp_typename(typeid(t)));
}

}  // namespace omnetpp

Register_Class(FsrPacket)

FsrPacket::FsrPacket() : ::inet::FieldsChunk()
{
}

FsrPacket::FsrPacket(const FsrPacket& other) : ::inet::FieldsChunk(other)
{
    copy(other);
}

FsrPacket::~FsrPacket()
{
    delete [] this->neighbours;
}

FsrPacket& FsrPacket::operator=(const FsrPacket& other)
{
    if (this == &other) return *this;
    ::inet::FieldsChunk::operator=(other);
    copy(other);
    return *this;
}

void FsrPacket::copy(const FsrPacket& other)
{
    this->sequenceNumber = other.sequenceNumber;
    this->scopeLevel = other.scopeLevel;
    this->origin = other.origin;
    delete [] this->neighbours;
    this->neighbours = (other.neighbours_arraysize==0) ? nullptr : new inet::L3Address[other.neighbours_arraysize];
    neighbours_arraysize = other.neighbours_arraysize;
    for (size_t i = 0; i < neighbours_arraysize; i++) {
        this->neighbours[i] = other.neighbours[i];
    }
}

void FsrPacket::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::inet::FieldsChunk::parsimPack(b);
    doParsimPacking(b,this->sequenceNumber);
    doParsimPacking(b,this->scopeLevel);
    doParsimPacking(b,this->origin);
    b->pack(neighbours_arraysize);
    doParsimArrayPacking(b,this->neighbours,neighbours_arraysize);
}

void FsrPacket::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::inet::FieldsChunk::parsimUnpack(b);
    doParsimUnpacking(b,this->sequenceNumber);
    doParsimUnpacking(b,this->scopeLevel);
    doParsimUnpacking(b,this->origin);
    delete [] this->neighbours;
    b->unpack(neighbours_arraysize);
    if (neighbours_arraysize == 0) {
        this->neighbours = nullptr;
    } else {
        this->neighbours = new inet::L3Address[neighbours_arraysize];
        doParsimArrayUnpacking(b,this->neighbours,neighbours_arraysize);
    }
}

unsigned int FsrPacket::getSequenceNumber() const
{
    return this->sequenceNumber;
}

void FsrPacket::setSequenceNumber(unsigned int sequenceNumber)
{
    handleChange();
    this->sequenceNumber = sequenceNumber;
}

int FsrPacket::getScopeLevel() const
{
    return this->scopeLevel;
}

void FsrPacket::setScopeLevel(int scopeLevel)
{
    handleChange();
    this->scopeLevel = scopeLevel;
}

const inet::L3Address& FsrPacket::getOrigin() const
{
    return this->origin;
}

void FsrPacket::setOrigin(const inet::L3Address& origin)
{
    handleChange();
    this->origin = origin;
}

size_t FsrPacket::getNeighboursArraySize() const
{
    return neighbours_arraysize;
}

const inet::L3Address& FsrPacket::getNeighbours(size_t k) const
{
    if (k >= neighbours_arraysize) throw omnetpp::cRuntimeError("Array of size %lu indexed by %lu", (unsigned long)neighbours_arraysize, (unsigned long)k);
    return this->neighbours[k];
}

void FsrPacket::setNeighboursArraySize(size_t newSize)
{
    handleChange();
    inet::L3Address *neighbours2 = (newSize==0) ? nullptr : new inet::L3Address[newSize];
    size_t minSize = neighbours_arraysize < newSize ? neighbours_arraysize : newSize;
    for (size_t i = 0; i < minSize; i++)
        neighbours2[i] = this->neighbours[i];
    delete [] this->neighbours;
    this->neighbours = neighbours2;
    neighbours_arraysize = newSize;
}

void FsrPacket::setNeighbours(size_t k, const inet::L3Address& neighbours)
{
    if (k >= neighbours_arraysize) throw omnetpp::cRuntimeError("Array of size %lu indexed by %lu", (unsigned long)neighbours_arraysize, (unsigned long)k);
    handleChange();
    this->neighbours[k] = neighbours;
}

void FsrPacket::insertNeighbours(size_t k, const inet::L3Address& neighbours)
{
    if (k > neighbours_arraysize) throw omnetpp::cRuntimeError("Array of size %lu indexed by %lu", (unsigned long)neighbours_arraysize, (unsigned long)k);
    handleChange();
    size_t newSize = neighbours_arraysize + 1;
    inet::L3Address *neighbours2 = new inet::L3Address[newSize];
    size_t i;
    for (i = 0; i < k; i++)
        neighbours2[i] = this->neighbours[i];
    neighbours2[k] = neighbours;
    for (i = k + 1; i < newSize; i++)
        neighbours2[i] = this->neighbours[i-1];
    delete [] this->neighbours;
    this->neighbours = neighbours2;
    neighbours_arraysize = newSize;
}

void FsrPacket::appendNeighbours(const inet::L3Address& neighbours)
{
    insertNeighbours(neighbours_arraysize, neighbours);
}

void FsrPacket::eraseNeighbours(size_t k)
{
    if (k >= neighbours_arraysize) throw omnetpp::cRuntimeError("Array of size %lu indexed by %lu", (unsigned long)neighbours_arraysize, (unsigned long)k);
    handleChange();
    size_t newSize = neighbours_arraysize - 1;
    inet::L3Address *neighbours2 = (newSize == 0) ? nullptr : new inet::L3Address[newSize];
    size_t i;
    for (i = 0; i < k; i++)
        neighbours2[i] = this->neighbours[i];
    for (i = k; i < newSize; i++)
        neighbours2[i] = this->neighbours[i+1];
    delete [] this->neighbours;
    this->neighbours = neighbours2;
    neighbours_arraysize = newSize;
}

class FsrPacketDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertyNames;
    enum FieldConstants {
        FIELD_sequenceNumber,
        FIELD_scopeLevel,
        FIELD_origin,
        FIELD_neighbours,
    };
  public:
    FsrPacketDescriptor();
    virtual ~FsrPacketDescriptor();

    virtual bool doesSupport(omnetpp::cObject *obj) const override;
    virtual const char **getPropertyNames() const override;
    virtual const char *getProperty(const char *propertyName) const override;
    virtual int getFieldCount() const override;
    virtual const char *getFieldName(int field) const override;
    virtual int findField(const char *fieldName) const override;
    virtual unsigned int getFieldTypeFlags(int field) const override;
    virtual const char *getFieldTypeString(int field) const override;
    virtual const char **getFieldPropertyNames(int field) const override;
    virtual const char *getFieldProperty(int field, const char *propertyName) const override;
    virtual int getFieldArraySize(omnetpp::any_ptr object, int field) const override;
    virtual void setFieldArraySize(omnetpp::any_ptr object, int field, int size) const override;

    virtual const char *getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const override;
    virtual std::string getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const override;
    virtual omnetpp::cValue getFieldValue(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const override;

    virtual const char *getFieldStructName(int field) const override;
    virtual omnetpp::any_ptr getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const override;
};

Register_ClassDescriptor(FsrPacketDescriptor)

FsrPacketDescriptor::FsrPacketDescriptor() : omnetpp::cClassDescriptor(omnetpp::opp_typename(typeid(FsrPacket)), "inet::FieldsChunk")
{
    propertyNames = nullptr;
}

FsrPacketDescriptor::~FsrPacketDescriptor()
{
    delete[] propertyNames;
}

bool FsrPacketDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<FsrPacket *>(obj)!=nullptr;
}

const char **FsrPacketDescriptor::getPropertyNames() const
{
    if (!propertyNames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
        const char **baseNames = base ? base->getPropertyNames() : nullptr;
        propertyNames = mergeLists(baseNames, names);
    }
    return propertyNames;
}

const char *FsrPacketDescriptor::getProperty(const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? base->getProperty(propertyName) : nullptr;
}

int FsrPacketDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? 4+base->getFieldCount() : 4;
}

unsigned int FsrPacketDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeFlags(field);
        field -= base->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,    // FIELD_sequenceNumber
        FD_ISEDITABLE,    // FIELD_scopeLevel
        0,    // FIELD_origin
        FD_ISARRAY | FD_ISRESIZABLE,    // FIELD_neighbours
    };
    return (field >= 0 && field < 4) ? fieldTypeFlags[field] : 0;
}

const char *FsrPacketDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldName(field);
        field -= base->getFieldCount();
    }
    static const char *fieldNames[] = {
        "sequenceNumber",
        "scopeLevel",
        "origin",
        "neighbours",
    };
    return (field >= 0 && field < 4) ? fieldNames[field] : nullptr;
}

int FsrPacketDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    int baseIndex = base ? base->getFieldCount() : 0;
    if (strcmp(fieldName, "sequenceNumber") == 0) return baseIndex + 0;
    if (strcmp(fieldName, "scopeLevel") == 0) return baseIndex + 1;
    if (strcmp(fieldName, "origin") == 0) return baseIndex + 2;
    if (strcmp(fieldName, "neighbours") == 0) return baseIndex + 3;
    return base ? base->findField(fieldName) : -1;
}

const char *FsrPacketDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeString(field);
        field -= base->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "unsigned int",    // FIELD_sequenceNumber
        "int",    // FIELD_scopeLevel
        "inet::L3Address",    // FIELD_origin
        "inet::L3Address",    // FIELD_neighbours
    };
    return (field >= 0 && field < 4) ? fieldTypeStrings[field] : nullptr;
}

const char **FsrPacketDescriptor::getFieldPropertyNames(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldPropertyNames(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

const char *FsrPacketDescriptor::getFieldProperty(int field, const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldProperty(field, propertyName);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

int FsrPacketDescriptor::getFieldArraySize(omnetpp::any_ptr object, int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldArraySize(object, field);
        field -= base->getFieldCount();
    }
    FsrPacket *pp = omnetpp::fromAnyPtr<FsrPacket>(object); (void)pp;
    switch (field) {
        case FIELD_neighbours: return pp->getNeighboursArraySize();
        default: return 0;
    }
}

void FsrPacketDescriptor::setFieldArraySize(omnetpp::any_ptr object, int field, int size) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldArraySize(object, field, size);
            return;
        }
        field -= base->getFieldCount();
    }
    FsrPacket *pp = omnetpp::fromAnyPtr<FsrPacket>(object); (void)pp;
    switch (field) {
        case FIELD_neighbours: pp->setNeighboursArraySize(size); break;
        default: throw omnetpp::cRuntimeError("Cannot set array size of field %d of class 'FsrPacket'", field);
    }
}

const char *FsrPacketDescriptor::getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldDynamicTypeString(object,field,i);
        field -= base->getFieldCount();
    }
    FsrPacket *pp = omnetpp::fromAnyPtr<FsrPacket>(object); (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string FsrPacketDescriptor::getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValueAsString(object,field,i);
        field -= base->getFieldCount();
    }
    FsrPacket *pp = omnetpp::fromAnyPtr<FsrPacket>(object); (void)pp;
    switch (field) {
        case FIELD_sequenceNumber: return ulong2string(pp->getSequenceNumber());
        case FIELD_scopeLevel: return long2string(pp->getScopeLevel());
        case FIELD_origin: return pp->getOrigin().str();
        case FIELD_neighbours: return pp->getNeighbours(i).str();
        default: return "";
    }
}

void FsrPacketDescriptor::setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValueAsString(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    FsrPacket *pp = omnetpp::fromAnyPtr<FsrPacket>(object); (void)pp;
    switch (field) {
        case FIELD_sequenceNumber: pp->setSequenceNumber(string2ulong(value)); break;
        case FIELD_scopeLevel: pp->setScopeLevel(string2long(value)); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'FsrPacket'", field);
    }
}

omnetpp::cValue FsrPacketDescriptor::getFieldValue(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValue(object,field,i);
        field -= base->getFieldCount();
    }
    FsrPacket *pp = omnetpp::fromAnyPtr<FsrPacket>(object); (void)pp;
    switch (field) {
        case FIELD_sequenceNumber: return (omnetpp::intval_t)(pp->getSequenceNumber());
        case FIELD_scopeLevel: return pp->getScopeLevel();
        case FIELD_origin: return omnetpp::toAnyPtr(&pp->getOrigin()); break;
        case FIELD_neighbours: return omnetpp::toAnyPtr(&pp->getNeighbours(i)); break;
        default: throw omnetpp::cRuntimeError("Cannot return field %d of class 'FsrPacket' as cValue -- field index out of range?", field);
    }
}

void FsrPacketDescriptor::setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValue(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    FsrPacket *pp = omnetpp::fromAnyPtr<FsrPacket>(object); (void)pp;
    switch (field) {
        case FIELD_sequenceNumber: pp->setSequenceNumber(omnetpp::checked_int_cast<unsigned int>(value.intValue())); break;
        case FIELD_scopeLevel: pp->setScopeLevel(omnetpp::checked_int_cast<int>(value.intValue())); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'FsrPacket'", field);
    }
}

const char *FsrPacketDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructName(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    };
}

omnetpp::any_ptr FsrPacketDescriptor::getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructValuePointer(object, field, i);
        field -= base->getFieldCount();
    }
    FsrPacket *pp = omnetpp::fromAnyPtr<FsrPacket>(object); (void)pp;
    switch (field) {
        case FIELD_origin: return omnetpp::toAnyPtr(&pp->getOrigin()); break;
        case FIELD_neighbours: return omnetpp::toAnyPtr(&pp->getNeighbours(i)); break;
        default: return omnetpp::any_ptr(nullptr);
    }
}

void FsrPacketDescriptor::setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldStructValuePointer(object, field, i, ptr);
            return;
        }
        field -= base->getFieldCount();
    }
    FsrPacket *pp = omnetpp::fromAnyPtr<FsrPacket>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'FsrPacket'", field);
    }
}

namespace omnetpp {

}  // namespace omnetpp


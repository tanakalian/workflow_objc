#ifndef BINARYNINJA_OBJCMESSAGEHANDLER_H
#define BINARYNINJA_OBJCMESSAGEHANDLER_H

#include <binaryninjaapi.h>

struct MessageSendStub {
    uint64_t address;
    uint64_t selRef;
};

class ObjCMessageHandler {
    bool m_hasObjcStubs;
    std::pair<uint64_t, uint64_t> m_objcStubsRange;

    std::set<uint64_t> m_msgSendFunctions;
    std::unordered_map<uint64_t, std::optional<MessageSendStub>> m_msgSendStubs;

    std::set<uint64_t> FindMsgSendFunctions(BinaryNinja::Ref<BinaryNinja::BinaryView> data);

public:
    ObjCMessageHandler(BinaryNinja::Ref<BinaryNinja::BinaryView> data);

    bool HasMsgSendStubs() const { return m_hasObjcStubs; };

    std::set<uint64_t> GetMessageSendFunctions() const { return m_msgSendFunctions; };
    bool IsPotentialMessageStub(uint64_t addr) const { return (m_objcStubsRange.first <= addr && addr < m_objcStubsRange.second); };
};

#endif // BINARYNINJA_OBJCMESSAGEHANDLER_H

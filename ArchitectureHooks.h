#pragma once

#include <binaryninjaapi.h>

constexpr uint32_t CFSTRIntrinsicIndex = UINT32_MAX - 64;

class CFStringArchitectureHook : public BinaryNinja::ArchitectureHook
{
    virtual std::string GetIntrinsicName(uint32_t intrinsic) override;
    virtual std::vector<uint32_t> GetAllIntrinsics() override;
    virtual std::vector<BinaryNinja::NameAndType> GetIntrinsicInputs(uint32_t intrinsic) override;
    virtual std::vector<BinaryNinja::Confidence<BinaryNinja::Ref<BinaryNinja::Type>>> GetIntrinsicOutputs(uint32_t intrinsic) override;

public:
    CFStringArchitectureHook(BinaryNinja::Ref<BinaryNinja::Architecture> base)
        : BinaryNinja::ArchitectureHook(base) { };
};

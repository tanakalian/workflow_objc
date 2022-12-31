#include "ArchitectureHooks.h"

using namespace BinaryNinja;

std::string CFStringArchitectureHook::GetIntrinsicName(uint32_t intrinsic)
{
    if (intrinsic == CFSTRIntrinsicIndex)
        return "CFSTR";

    return ArchitectureHook::GetIntrinsicName(intrinsic);
}

std::vector<uint32_t> CFStringArchitectureHook::GetAllIntrinsics()
{
    auto parent = ArchitectureHook::GetAllIntrinsics();
    parent.push_back(CFSTRIntrinsicIndex);
    return parent;
}

std::vector<NameAndType> CFStringArchitectureHook::GetIntrinsicInputs(uint32_t intrinsic)
{
    if (intrinsic != CFSTRIntrinsicIndex)
        return ArchitectureHook::GetIntrinsicInputs(intrinsic);

    return { NameAndType(Type::PointerType(ArchitectureHook::GetAddressSize(), Type::IntegerType(1, false))) };
}

std::vector<Confidence<BinaryNinja::Ref<Type>>> CFStringArchitectureHook::GetIntrinsicOutputs(uint32_t intrinsic)
{
    if (intrinsic != CFSTRIntrinsicIndex)
        return ArchitectureHook::GetIntrinsicOutputs(intrinsic);

    return { Type::PointerType(ArchitectureHook::GetAddressSize(), Type::IntegerType(1, false)) };
}

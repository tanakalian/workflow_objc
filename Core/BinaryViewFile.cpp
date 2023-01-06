/*
 * Copyright (c) 2022-2023 Jon Palmisciano. All rights reserved.
 *
 * Use of this source code is governed by the BSD 3-Clause license; the full
 * terms of the license can be found in the LICENSE.txt file.
 */

#include "BinaryViewFile.h"

namespace ObjectiveNinja {

BinaryViewFile::BinaryViewFile(BinaryViewRef bv)
    : m_bv(bv)
    , m_reader(BinaryNinja::BinaryReader(bv))
{
}

void BinaryViewFile::seek(uint64_t address)
{
    m_reader.Seek(address);
}

uint8_t BinaryViewFile::readByte()
{
    return m_reader.Read8();
}

uint32_t BinaryViewFile::readInt()
{
    return m_reader.Read32();
}

uint64_t BinaryViewFile::readLong()
{
    return m_reader.Read64();
}

uint64_t BinaryViewFile::imageBase() const
{
    return m_bv->GetStart();
}

uint64_t BinaryViewFile::sectionStart(const std::string& name) const
{
    auto section = m_bv->GetSectionByName(name);
    if (!section)
        return 0;

    return section->GetStart();
}

uint64_t BinaryViewFile::sectionEnd(const std::string& name) const
{
    auto section = m_bv->GetSectionByName(name);
    if (!section)
        return 0;

    return section->GetStart() + section->GetLength();
}

bool BinaryViewFile::addressIsMapped(uint64_t address, bool includeExtern) const
{
    if (!includeExtern)
    {
        uint64_t externStart = sectionStart(".extern");
        uint64_t externEnd = sectionEnd(".extern");
        if (externStart) {
            if (externStart < address && externEnd > address) {
                return false;
            }
        }
    }

    return m_bv->IsValidOffset(address);
}


bool BinaryViewFile::hasImportedSymbolAtLocation(uint64_t address) const
{
    BinaryNinja::Ref<BinaryNinja::Symbol> sym = m_bv->GetSymbolByAddress(address);

    if (sym)
        return (sym->GetType() == ImportedDataSymbol);

    return false;
}


std::string BinaryViewFile::symbolNameAtLocation(uint64_t address) const
{
    BinaryNinja::Ref<BinaryNinja::Symbol> sym = m_bv->GetSymbolByAddress(address);

    if (sym)
        return sym->GetFullName();

    return "";
}

}

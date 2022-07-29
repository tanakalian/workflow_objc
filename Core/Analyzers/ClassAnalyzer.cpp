/*
 * Copyright (c) 2022 Jon Palmisciano. All rights reserved.
 *
 * Use of this source code is governed by the BSD 3-Clause license; the full
 * terms of the license can be found in the LICENSE.txt file.
 */

#include "ClassAnalyzer.h"

using namespace ObjectiveNinja;

ClassAnalyzer::ClassAnalyzer(SharedAnalysisInfo info,
    SharedAbstractFile file)
    : Analyzer(std::move(info), std::move(file))
{
}

MethodListInfo ClassAnalyzer::analyzeMethodList(uint64_t address)
{
    MethodListInfo mli;
    mli.address = address;
    mli.flags = m_file->readInt(mli.address);

    auto methodCount = m_file->readInt(mli.address + 0x4);
    auto methodSize = mli.hasRelativeOffsets() ? 12 : (3 * m_file->pointerSize());

    for (unsigned i = 0; i < methodCount; ++i) {
        MethodInfo mi;
        mi.address = mli.address + 8 + (i * methodSize);

        m_file->seek(mi.address);

        if (mli.hasRelativeOffsets()) {
            mi.nameAddress = mi.address + static_cast<int32_t>(m_file->readInt());
            mi.typeAddress = mi.address + 4 + static_cast<int32_t>(m_file->readInt());
            mi.implAddress = mi.address + 8 + static_cast<int32_t>(m_file->readInt());
        } else {
            mi.nameAddress = arp(m_file->readPointer());
            mi.typeAddress = arp(m_file->readPointer());
            mi.implAddress = arp(m_file->readPointer());
			// mask out armv7+thumb2 branch and exchange swap bit
			mi.implAddress &= ~0b1;
        }

        if (!mli.hasRelativeOffsets() || mli.hasDirectSelectors()) {
            mi.selector = m_file->readStringAt(mi.nameAddress);
        } else {
            auto selectorNamePointer = arp(m_file->readLong(mi.nameAddress));
            mi.selector = m_file->readStringAt(selectorNamePointer);
        }

        mi.type = m_file->readStringAt(mi.typeAddress);

        m_info->methodImpls[mi.nameAddress] = mi.implAddress;

        mli.methods.emplace_back(mi);
    }

    return mli;
}

IvarListInfo ClassAnalyzer::analyzeIvarList(uint64_t address)
{
	IvarListInfo ili;
	ili.address = address;
	auto ivarCount = m_file->readInt(ili.address + 4);

	ili.ivars.reserve(ivarCount);

	auto ivarSize = m_file->pointerSize() * 3 + 8;

	for (unsigned i = 0; i < ivarCount; ++i)
	{
		IvarInfo ii;
		ii.address = ili.address + 8 + (i * ivarSize);

		m_file->seek(ii.address);

		ii.offsetAddress = arp(m_file->readPointer());
		ii.nameAddress = arp(m_file->readPointer());
		ii.typesAddress = arp(m_file->readPointer());
		m_file->readInt();
		ii.size = m_file->readInt();

		ii.offset = m_file->readInt(ii.offsetAddress);
		ii.name = m_file->readStringAt(ii.nameAddress);
		ii.types = m_file->readStringAt(ii.typesAddress);

		ili.ivars.push_back(ii);
	}

	return ili;
}

void ClassAnalyzer::run()
{
    const auto sectionStart = m_file->sectionStart("__objc_classlist");
    const auto sectionEnd = m_file->sectionEnd("__objc_classlist");
    if (sectionStart == 0 || sectionEnd == 0)
        return;

    for (auto address = sectionStart; address < sectionEnd; address += 8) {
        ClassInfo ci;
        ci.listPointer = address;
        ci.address = arp(m_file->readPointer(address));

		int pointerSize = m_file->pointerSize();

		// dataAddress is class_ro
        ci.dataAddress = arp(m_file->readPointer(ci.address + (4 * pointerSize)));

        // Sometimes the lower two bits of the data address are used as flags
        // for Swift/Objective-C classes. They should be ignored, unless you
        // want incorrect analysis...
        ci.dataAddress &= ~ABI::FastPointerDataMask;

		// kat: core desperately needs a 'load pre-defined struct from addr' ;_;
		int RO_PTR_START = pointerSize == 8 ? 16 : 12;
		int RO_NAME_OFF_FROM_PTR_START = RO_PTR_START + pointerSize * 1;
		int RO_METH_OFF_FROM_PTR_START = RO_PTR_START + pointerSize * 2;
		int RO_IVAR_OFF_FROM_PTR_START = RO_PTR_START + pointerSize * 4;

        ci.nameAddress = arp(m_file->readPointer(ci.dataAddress + RO_NAME_OFF_FROM_PTR_START));
        ci.name = m_file->readStringAt(ci.nameAddress);

        ci.methodListAddress = arp(m_file->readPointer(ci.dataAddress + RO_METH_OFF_FROM_PTR_START));
        if (ci.methodListAddress)
            ci.methodList = analyzeMethodList(ci.methodListAddress);

		ci.ivarListAddress = arp(m_file->readPointer(ci.dataAddress + RO_IVAR_OFF_FROM_PTR_START));
		if (ci.ivarListAddress)
			ci.ivarList = analyzeIvarList(ci.ivarListAddress);

        m_info->classes.emplace_back(ci);
    }
}

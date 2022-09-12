#include "ClassRefAnalyzer.h"

using namespace ObjectiveNinja;

ClassRefAnalyzer::ClassRefAnalyzer(SharedAnalysisInfo info, SharedAbstractFile file)
    : Analyzer(std::move(info), std::move(file))
{
}

void ClassRefAnalyzer::run()
{
    const auto sectionStart = m_file->sectionStart("__objc_classrefs");
    const auto sectionEnd = m_file->sectionEnd("__objc_classrefs");

    // TODO: Dynamic Address size for armv7
    if (sectionStart != 0 && sectionEnd != 0) {
        for (auto address = sectionStart; address < sectionEnd; address += 0x8) {
            m_info->classRefs.push_back({ address, m_file->readLong(address) });
        }
    }

    const auto superRefSectionStart = m_file->sectionStart("__objc_superrefs");
    const auto superRefSectionEnd = m_file->sectionEnd("__objc_superrefs");

    if (superRefSectionStart != 0 && superRefSectionEnd != 0) {
        for (auto address = superRefSectionStart; address < superRefSectionEnd; address += 0x8) {
            m_info->superRefs.push_back({ address, m_file->readLong(address) });
        }
    }
}
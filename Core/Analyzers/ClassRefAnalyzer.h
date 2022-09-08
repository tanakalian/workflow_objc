#pragma once

#include "../Analyzer.h"

namespace ObjectiveNinja {

/**
 * Analyzer for extracting Objective-C class information.
 */
class ClassRefAnalyzer : public Analyzer {

public:
    ClassRefAnalyzer(SharedAnalysisInfo, SharedAbstractFile);

    void run() override;
};
}
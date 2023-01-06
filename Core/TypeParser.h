/*
 * Copyright (c) 2022-2023 Jon Palmisciano. All rights reserved.
 *
 * Use of this source code is governed by the BSD 3-Clause license; the full
 * terms of the license can be found in the LICENSE.txt file.
 */

#pragma once

#include <binaryninjaapi.h>
#include <string>
#include <vector>

namespace ObjectiveNinja {

struct QualifiedNameOrType {
    BinaryNinja::Ref<BinaryNinja::Type> type = nullptr;
    BinaryNinja::QualifiedName name;
    size_t ptrCount = 0;
};

/**
 * Parser for Objective-C type strings.
 */
class TypeParser {
public:
    /**
     * Parse an encoded type string.
     */
    static std::vector<QualifiedNameOrType> parseEncodedType(const std::string&);
};

}

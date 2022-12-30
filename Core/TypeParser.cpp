/*
 * Copyright (c) 2022 Jon Palmisciano. All rights reserved.
 *
 * Use of this source code is governed by the BSD 3-Clause license; the full
 * terms of the license can be found in the LICENSE.txt file.
 */

#include "TypeParser.h"

#include <map>

using namespace BinaryNinja;

namespace ObjectiveNinja {

std::vector<QualifiedNameOrType> TypeParser::parseEncodedType(const std::string& encodedType)
{
    std::vector<QualifiedNameOrType> result;
    int pointerDepth = 0;

    bool readingNamedType = false;
    std::string namedType;
    size_t readingStructDepth = 0;
    std::string structType;
    char last;

    for (char c : encodedType) {

        if (readingNamedType && c != '"') {
            namedType.push_back(c);
            last = c;
            continue;
        } else if (readingStructDepth > 0 && c != '{' && c != '}') {
            structType.push_back(c);
            last = c;
            continue;
        }

        if (std::isdigit(c))
            continue;

        QualifiedNameOrType nameOrType;
        std::string qualifiedName;

        switch (c) {
        case '^':
            pointerDepth++;
            last = c;
            continue;

        case '"':
            if (!readingNamedType) {
                readingNamedType = true;
                if (last == '@')
                    result.pop_back(); // We added an 'id' in the last cycle, remove it
                last = c;
                continue;
            } else {
                readingNamedType = false;
                nameOrType.name = QualifiedName(namedType);
                nameOrType.ptrCount = 1;
                break;
            }
        case '{':
            readingStructDepth++;
            last = c;
            continue;
        case '}':
            readingStructDepth--;

            if (readingStructDepth == 0) {
                // TODO: Emit real struct types
                nameOrType.type = Type::PointerType(8, Type::VoidType());
                break;
            }
            last = c;
            continue;
        case 'v':
            nameOrType.type = Type::VoidType();
            break;
        case 'c':
            nameOrType.type = Type::IntegerType(1, true);
            break;
        case 'A':
        case 'C':
            nameOrType.type = Type::IntegerType(1, false);
            break;
        case 's':
            nameOrType.type = Type::IntegerType(2, true);
            break;
        case 'S':
            nameOrType.type = Type::IntegerType(1, false);
            break;
        case 'i':
            nameOrType.type = Type::IntegerType(4, true);
            break;
        case 'I':
            nameOrType.type = Type::IntegerType(4, false);
            break;
        case 'l':
            nameOrType.type = Type::IntegerType(8, true);
            break;
        case 'L':
            nameOrType.type = Type::IntegerType(8, true);
            break;
        case 'f':
            nameOrType.type = Type::IntegerType(4, true);
            break;
        case 'b':
        case 'B':
            nameOrType.type = Type::BoolType();
            break;
        case 'q':
            qualifiedName = "NSInteger";
            break;
        case 'Q':
            qualifiedName = "NSUInteger";
            break;
        case 'd':
            qualifiedName = "CGFloat";
            break;
        case '*':
            nameOrType.type = Type::PointerType(8, Type::IntegerType(1, true));
            break;
        case '@':
            qualifiedName = "id";
            // There can be a type after this, like @"NSString", that overrides this
            // The handler for " will catch it and drop this "id" entry.
            break;
        case ':':
            qualifiedName = "SEL";
            break;
        case '#':
            qualifiedName = "objc_class_t";
            break;
        case '?':
        case 'T':
            nameOrType.type = Type::PointerType(8, Type::VoidType());
            break;
        default:
            // BNLogWarn("Unknown type specifier %c", c);
            last = c;
            continue;
        }

        while (pointerDepth) {
            if (nameOrType.type)
                nameOrType.type = Type::PointerType(8, nameOrType.type);
            else
                nameOrType.ptrCount++;

            pointerDepth--;
        }

        if (!qualifiedName.empty())
            nameOrType.name = QualifiedName(qualifiedName);

        if (nameOrType.type == nullptr && nameOrType.name.IsEmpty()) {
            // BNLogError("Parsing Typestring item %c failed", c);
            nameOrType.type = Type::VoidType();
        }

        // BNLogWarn("Pushing back a type %s on %c", nameOrType.type ? nameOrType.type->GetString().c_str() : nameOrType.name.GetString().c_str(), c);

        result.push_back(nameOrType);
        last = c;
    }

    return result;
}

}

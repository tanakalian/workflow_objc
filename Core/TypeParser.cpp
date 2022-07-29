/*
 * Copyright (c) 2022 Jon Palmisciano. All rights reserved.
 *
 * Use of this source code is governed by the BSD 3-Clause license; the full
 * terms of the license can be found in the LICENSE.txt file.
 */

#include "TypeParser.h"

#include <map>

namespace ObjectiveNinja {

static const std::map<char, std::pair<std::string, std::optional<BinaryNinja::Ref<BinaryNinja::Type>>>> TypeEncodingMap = {
    { 'v', {"void", BinaryNinja::Type::VoidType()}},
    { 'c', {"char", BinaryNinja::Type::IntegerType(1, true)}},
    { 's', {"short", BinaryNinja::Type::IntegerType(2, true)}},
    { 'i', {"int", BinaryNinja::Type::IntegerType(4, true)}},
    { 'l', {"long", BinaryNinja::Type::IntegerType(8, true)}},
    { 'C', {"unsigned char", BinaryNinja::Type::IntegerType(1, false)}},
    { 'S', {"unsigned short", BinaryNinja::Type::IntegerType(2, false)}},
    { 'I', {"unsigned int", BinaryNinja::Type::IntegerType(4, false)}},
    { 'L', {"unsigned long", BinaryNinja::Type::IntegerType(8, false)}},
    { 'f', {"float", BinaryNinja::Type::FloatType(4)}},
    { 'A', {"uint8_t", BinaryNinja::Type::IntegerType(1, false)}},
    { 'b', {"BOOL", BinaryNinja::Type::BoolType()}},
    { 'B', {"BOOL", BinaryNinja::Type::BoolType()} },

    { 'q', {"NSInteger", std::nullopt}},
    { 'Q', {"NSUInteger", std::nullopt}},
    { 'd', {"CGFloat", std::nullopt}},
    { '*', {"char *", BinaryNinja::Type::PointerType(8, BinaryNinja::Type::IntegerType(1, false))}},

    { '@', {"id", std::nullopt} },
    { ':', {"SEL", std::nullopt} },
    { '#', {"objc_class_t", std::nullopt} },

    { '?', {"void*", BinaryNinja::Type::PointerType(8, BinaryNinja::Type::IntegerType(1, false))} },
    { 'T', {"void*", BinaryNinja::Type::PointerType(8, BinaryNinja::Type::IntegerType(1, false))} },
};

std::vector<ParsedType> TypeParser::parseEncodedType(BinaryNinja::Ref<BinaryNinja::Architecture> arch, const std::string& encodedType)
{
    std::vector<ParsedType> result;

    for (size_t i = 0; i < encodedType.size(); ++i) {

		ParsedType type;

        char c = encodedType[i];

        // Argument frame size and offset specifiers aren't relevant here; they
        // should just be skipped.
        if (std::isdigit(c))
            continue;

        if (TypeEncodingMap.count(c)) {
			type.encodedKind = PredefinedType;
			type.name = TypeEncodingMap.at(c).first;
			type.dependency = std::nullopt;
			type.type = TypeEncodingMap.at(c).second;
			if (type.type.has_value() && type.type.value()->GetClass() == PointerTypeClass)
				type.type = BinaryNinja::Type::PointerType(arch, type.type.value()->GetChildType());
            result.push_back(type);
            continue;
        }

        // (Partially) handle quoted type names.
        if (c == '"') {
			std::string typeName;
            while (encodedType[i] != '"')
			{
				typeName.push_back(encodedType[i]);
				i++;
			}

			// TODO: Emit real type names.
			type.encodedKind = NamedType;
			type.name = typeName;
			type.dependency = typeName;
			type.type = BinaryNinja::Type::PointerType(arch, BinaryNinja::Type::VoidType());
            result.push_back(type);
            continue;
        }

        // (Partially) handle struct types.
        if (c == '{') {
            auto depth = 1;

            while (depth != 0) {
                char d = encodedType[++i];

                if (d == '{')
                    ++depth;
                else if (d == '}')
                    --depth;
            }

            // TODO: Emit real struct types.
			type.encodedKind = StructType;
			type.name = "void*";
			type.dependency = std::nullopt;
			type.type = BinaryNinja::Type::PointerType(arch, BinaryNinja::Type::VoidType());
            result.push_back(type);
            continue;
        }

        break;
    }

    return result;
}

}

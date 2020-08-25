#pragma once

//**************************************************************************
// Copyright (c) 2020-present, Altrop Software Inc. and Contributors.
// SPDX-License-Identifier: BSL-1.0
//**************************************************************************

/**
 * @file TranslationMap.h
 * @library alt_util
 * @brief definition of a string to string translation map
 * Strings are pointers that points to the position in a string pool
 * where the space is not freed for new string. Therefore, the map is suitable
 * for the usage where erase is not required.
 */

#include "Allocator.h"
#include "util/string/StrBuffer.h"
#include "util/string/StrPool.h"
#include <unordered_map>

namespace alt
{

/**
 * \brief implements a string to string translation map.
 */
using TranslationMapBase =
        std::unordered_map<StrRef,
                StrRef,
                std::hash<StrRef>,
                std::equal_to<StrRef>,
                StdFixedPoolAllocator< std::pair<const StrRef, StrRef> >
                >;

class ALT_UTIL_PUBLIC TranslationMap : protected TranslationMapBase
{
  protected:
    StrPool                           string_pool_;

  public:

    void add (const char* source, const char* translated)
    {
        const auto [iter, inserted] =
            TranslationMapBase::emplace(StrRef(source), StrRef(translated));
        if (inserted)
        {
            *const_cast<StrRef*>(&iter->first) = StrRef(string_pool_.insert(source));
            iter->second = StrRef(string_pool_.insert(translated));
        }
    }

    template <typename StrT>
    void add (const StrT& source, const StrT& translated)
    {
        add(source.c_str(), translated.c_str());
    }

    const char* translate(const char* source)
    {
        auto iter = TranslationMapBase::find(StrRef(source));
        return iter==end() ? nullptr : iter->second.c_str();
    }

    template <typename StrT>
    const char* translate(const StrT& source)
    {
        return translate(source.c_str());
    }

    void clear()
    {
        TranslationMapBase::clear();
        string_pool_.clear();
    }
};

}


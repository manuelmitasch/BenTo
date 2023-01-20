// ArduinoJson - arduinojson.org
// Copyright Benoit Blanchon 2014-2020
// MIT License

#pragma once

#include "../Operators/VariantCasts.hpp"
#include "../Operators/VariantComparisons.hpp"
#include "../Operators/VariantOr.hpp"
#include "../Operators/VariantShortcuts.hpp"

namespace ARDUINOJSON_NAMESPACE {

template <typename TImpl>
class VariantOperators : public VariantCasts<TImpl>,
                         public VariantComparisons<TImpl>,
                         public VariantOr<TImpl>,
                         public VariantShortcuts<TImpl> {};
}  // namespace ARDUINOJSON_NAMESPACE

/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 *
 * Copyright (c) 2012-2018 Inviwo Foundation
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *********************************************************************************/

#ifndef IVW_ORDINALPROPERTY_H
#define IVW_ORDINALPROPERTY_H

#include <inviwo/core/common/inviwocoredefine.h>
#include <inviwo/core/common/inviwo.h>
#include <inviwo/core/properties/templateproperty.h>
#include <inviwo/core/util/glm.h>
#include <string>
#include <sstream>

namespace inviwo {

/**
 * \ingroup properties
 * A property representing a Ordinal value, for example int, floats.
 */
template <typename T>
class OrdinalProperty : public TemplateProperty<T> {
public:
    OrdinalProperty(const std::string& identifier, const std::string& displayName,
                    const T& value = Defaultvalues<T>::getVal(),
                    const T& minValue = Defaultvalues<T>::getMin(),
                    const T& maxValue = Defaultvalues<T>::getMax(),
                    const T& increment = Defaultvalues<T>::getInc(),
                    InvalidationLevel invalidationLevel = InvalidationLevel::InvalidOutput,
                    PropertySemantics semantics = PropertySemantics::Default);

    OrdinalProperty(const OrdinalProperty<T>& rhs) = default;
    OrdinalProperty<T>& operator=(const OrdinalProperty<T>& that) = default;
    OrdinalProperty<T>& operator=(const T& value);
    virtual OrdinalProperty<T>* clone() const override;
    virtual ~OrdinalProperty() = default;

    virtual std::string getClassIdentifier() const override;

    T getMinValue() const;
    T getMaxValue() const;
    T getIncrement() const;

    virtual void set(const T& value) override;
    virtual void set(const Property* src) override;

    void setMinValue(const T& value);
    void setMaxValue(const T& value);
    void setIncrement(const T& value);

    /**
     * \brief set all parameters of the ordinal property at the same time with only a
     * single validation.
     */
    void set(const T& value, const T& minVal, const T& maxVal, const T& increment);

    virtual void setCurrentStateAsDefault() override;
    virtual void resetToDefaultState() override;

    virtual void serialize(Serializer& s) const override;
    virtual void deserialize(Deserializer& d) override;

    static uvec2 getDim() { return Defaultvalues<T>::getDim(); }

    virtual Document getDescription() const override;

protected:
    /**
     * \brief validate the given value against the set min/max range
     *
     * @param v   value to be validated
     * @return returns the pair { modified, valid value } where modified indicates
     *            whether the given value was adjusted. The new value is stored as
     *            second parameter. In case there was not modification, valid value
     *            is equal to TemplateProperty<T>::value_.
     */
    std::pair<bool, T> validateValues(const T& v);

private:
    using TemplateProperty<T>::value_;
    ValueWrapper<T> minValue_;
    ValueWrapper<T> maxValue_;
    ValueWrapper<T> increment_;
};

// Scalar properties
using FloatProperty = OrdinalProperty<float>;
using IntProperty = OrdinalProperty<int>;
using IntSizeTProperty = OrdinalProperty<size_t>;
using Int64Property = OrdinalProperty<glm::i64>;
using DoubleProperty = OrdinalProperty<double>;

// Vector properties
using FloatVec2Property = OrdinalProperty<vec2>;
using FloatVec3Property = OrdinalProperty<vec3>;
using FloatVec4Property = OrdinalProperty<vec4>;

using DoubleVec2Property = OrdinalProperty<dvec2>;
using DoubleVec3Property = OrdinalProperty<dvec3>;
using DoubleVec4Property = OrdinalProperty<dvec4>;

using IntVec2Property = OrdinalProperty<ivec2>;
using IntVec3Property = OrdinalProperty<ivec3>;
using IntVec4Property = OrdinalProperty<ivec4>;

using IntSize2Property = OrdinalProperty<size2_t>;
using IntSize3Property = OrdinalProperty<size3_t>;
using IntSize4Property = OrdinalProperty<size4_t>;

// Matrix properties
using FloatMat2Property = OrdinalProperty<mat2>;
using FloatMat3Property = OrdinalProperty<mat3>;
using FloatMat4Property = OrdinalProperty<mat4>;

using DoubleMat2Property = OrdinalProperty<dmat2>;
using DoubleMat3Property = OrdinalProperty<dmat3>;
using DoubleMat4Property = OrdinalProperty<dmat4>;

using DoubleQuaternionProperty = OrdinalProperty<glm::dquat>;
using FloatQuaternionProperty = OrdinalProperty<glm::fquat>;

template <typename T>
struct PropertyTraits<OrdinalProperty<T>> {
    static std::string classIdentifier() {
        return "org.inviwo." + Defaultvalues<T>::getName() + "Property";
    }
};

template <typename T>
OrdinalProperty<T>::OrdinalProperty(const std::string& identifier, const std::string& displayName,
                                    const T& value, const T& minValue, const T& maxValue,
                                    const T& increment, InvalidationLevel invalidationLevel,
                                    PropertySemantics semantics)
    : TemplateProperty<T>(identifier, displayName, value, invalidationLevel, semantics)
    , minValue_("minvalue", minValue)
    , maxValue_("maxvalue", maxValue)
    , increment_("increment", increment) {

    // Invariant minValue_ < value_ < maxValue_
    // Assume minValue is correct.
    value_.value = glm::max(value_.value, minValue_.value);
    maxValue_.value = glm::max(maxValue_.value, value_.value);
}

template <typename T>
OrdinalProperty<T>& OrdinalProperty<T>::operator=(const T& value) {
    TemplateProperty<T>::operator=(value);
    return *this;
}

template <typename T>
OrdinalProperty<T>* OrdinalProperty<T>::clone() const {
    return new OrdinalProperty<T>(*this);
}

template <typename T>
std::string OrdinalProperty<T>::getClassIdentifier() const {
    return PropertyTraits<OrdinalProperty<T>>::classIdentifier();
}

template <typename T>
void OrdinalProperty<T>::set(const T& value) {
    if (value_.value == value) return;

    auto retVal = validateValues(value);
    if (retVal.first) {
        value_.value = retVal.second;
        OrdinalProperty<T>::propertyModified();
    }
}

template <typename T>
void OrdinalProperty<T>::set(const Property* srcProperty) {
    if (auto prop = dynamic_cast<const OrdinalProperty<T>*>(srcProperty)) {
        this->minValue_.value = prop->minValue_.value;
        this->maxValue_.value = prop->maxValue_.value;
        this->increment_.value = prop->increment_.value;
        TemplateProperty<T>::set(prop);
    }
}

template <typename T>
T OrdinalProperty<T>::getMinValue() const {
    return minValue_;
}

template <typename T>
T OrdinalProperty<T>::getMaxValue() const {
    return maxValue_;
}

template <typename T>
T OrdinalProperty<T>::getIncrement() const {
    return increment_;
}

template <typename T>
void OrdinalProperty<T>::setMinValue(const T& value) {
    if (value == minValue_) return;
    minValue_ = value;

    // Make sure min < max
    maxValue_.value = glm::max(maxValue_.value, minValue_.value);

    value_.value = validateValues(value_.value).second;
    OrdinalProperty<T>::propertyModified();
}

template <typename T>
void OrdinalProperty<T>::setMaxValue(const T& value) {
    if (value == maxValue_) return;
    maxValue_ = value;

    // Make sure min < max
    minValue_.value = glm::min(minValue_.value, maxValue_.value);

    value_.value = validateValues(value_.value).second;
    OrdinalProperty<T>::propertyModified();
}

template <typename T>
void OrdinalProperty<T>::setIncrement(const T& value) {
    if (value == increment_) return;
    increment_ = value;

    value_.value = validateValues(value_.value).second;
    OrdinalProperty<T>::propertyModified();
}

template <typename T>
void OrdinalProperty<T>::set(const T& value, const T& minVal, const T& maxVal, const T& increment) {
    bool modified = false;

    if ((minVal != minValue_.value) || (maxVal != maxValue_.value)) {
        if (glm::any(glm::greaterThan(minVal, maxVal))) {
            LogWarn("Invalid range given for \"" << this->getDisplayName() << "\" ("
                                                 << Defaultvalues<T>::getName()
                                                 << "Property). Using min range as reference.");
        }

        minValue_.value = minVal;
        maxValue_.value = glm::max(minVal, maxVal);
        modified = true;
    }
    if (increment != increment_) {
        increment_ = increment;
        modified = true;
    }
    auto retVal = validateValues(value);
    if (retVal.first) {
        value_.value = retVal.second;
        modified = true;
    }

    if (modified) {
        OrdinalProperty<T>::propertyModified();
    }
}

template <typename T>
void OrdinalProperty<T>::resetToDefaultState() {
    minValue_.reset();
    maxValue_.reset();
    increment_.reset();
    TemplateProperty<T>::resetToDefaultState();
}

template <typename T>
void OrdinalProperty<T>::setCurrentStateAsDefault() {
    TemplateProperty<T>::setCurrentStateAsDefault();
    minValue_.setAsDefault();
    maxValue_.setAsDefault();
    increment_.setAsDefault();
}

template <typename T>
void OrdinalProperty<T>::serialize(Serializer& s) const {
    Property::serialize(s);

    minValue_.serialize(s, this->serializationMode_);
    maxValue_.serialize(s, this->serializationMode_);
    increment_.serialize(s, this->serializationMode_);
    value_.serialize(s, this->serializationMode_);
}

template <typename T>
void OrdinalProperty<T>::deserialize(Deserializer& d) {
    Property::deserialize(d);

    bool modified = false;
    modified |= minValue_.deserialize(d, this->serializationMode_);
    modified |= maxValue_.deserialize(d, this->serializationMode_);
    modified |= increment_.deserialize(d, this->serializationMode_);
    modified |= value_.deserialize(d, this->serializationMode_);
    if (modified) OrdinalProperty<T>::propertyModified();
}

template <typename T>
std::pair<bool, T> OrdinalProperty<T>::validateValues(const T& v) {
    T newValue(glm::clamp(v, minValue_.value, maxValue_.value));
    return {(newValue != value_.value), newValue};
}

template <typename T>
Document OrdinalProperty<T>::getDescription() const {
    using P = Document::PathComponent;
    using H = utildoc::TableBuilder::Header;

    Document doc = TemplateProperty<T>::getDescription();
    auto b = doc.get({P("html"), P("body")});

    utildoc::TableBuilder tb(b, P::end());
    tb(H("#"), H("Value"), H("Min"), H("Max"), H("Inc"));
    size_t size = getDim().x * getDim().y;
    for (size_t i = 0; i < size; i++) {
        tb(H(i), util::glmcomp(value_.value, i), util::glmcomp(minValue_.value, i),
           util::glmcomp(maxValue_.value, i), util::glmcomp(increment_.value, i));
    }

    return doc;
}

}  // namespace inviwo

#endif  // IVW_ORDINALPROPERTY_H

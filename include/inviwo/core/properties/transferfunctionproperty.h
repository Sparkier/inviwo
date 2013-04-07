#ifndef IVW_TRANSFERFUNCTIONPROPERTY_H
#define IVW_TRANSFERFUNCTIONPROPERTY_H

#include <inviwo/core/properties/templateproperty.h>
#include <inviwo/core/datastructures/transferfunction.h>

namespace inviwo {

    class IVW_CORE_API TransferFunctionProperty : public TemplateProperty<TransferFunction> {

    public:
        TransferFunctionProperty(std::string identifier, std::string displayName, TransferFunction value, PropertySemantics::Type semantics = PropertySemantics::Default);
        virtual void serialize(IvwSerializer& s) const;
        virtual void deserialize(IvwDeserializer& d);
    private:
        TransferFunction value;
    };
} // namespace

#endif // IVW_TRANSFERFUNCTIONPROPERTY_H
/**********************************************************************************************
 Copyright (C) 2015 Ivo Kronenberg

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.

 **********************************************************************************************/


#include "gis/fit/defs/CFitProfileLockup.h"
#include "gis/fit/decoder/CFitFieldBuilder.h"
#include "gis/fit/decoder/CFitByteDataTransformer.h"


void CFitFieldBuilder::evaluateSubfieldsAndExpandComponents(CFitMessage& mesg) {
    for (CFitField *field : mesg.getFields()) {
        CFitFieldBuilder::evaluateFieldProfile(mesg, *field);
    }
    for (CFitField *field : mesg.getFields()) {
        CFitFieldBuilder::expandComponents(mesg, *field);
    }
}

CFitField *CFitFieldBuilder::buildField(const CFitFieldDefinition &def, uint8_t *fieldData, const CFitMessage& message) {
    CFitByteDataTransformer::swapFieldData(def, fieldData);
    const CFitBaseType& baseType = def.getBaseType();
    const CFitFieldProfile* fieldProfile = CFitProfileLockup::getFieldForProfile(message.getGlobalMesgNr(), def.getDefNr());
    if (baseType.isSignedInt()) {
        return buildSIntField(def, fieldProfile, fieldData);
    }
    else if (baseType.isUnsignedInt()) {
        return buildUIntField(def, fieldProfile, fieldData);
    }
    else if (baseType.isFloat()) {
        return buildFloatField(def, fieldProfile, fieldData);
    }
    else if (baseType.isString()) {
        return buildStringField(def, fieldProfile, fieldData);
    }
    else if (baseType.isByte()) {
        return buildByteField(def, fieldProfile, fieldData);
    }
    else {
        // should not be possible
        throw QString("FIT unknown base type");
    }
}


CFitField *CFitFieldBuilder::buildSIntField(const CFitFieldDefinition &def, const CFitFieldProfile* fieldProfile, uint8_t *fieldData) {
    const CFitBaseType &baseType = def.getBaseType();
    uint8_t *invalidBytes = baseType.invalidValueBytes();
    bool valid = false;
    if (def.getSize() == baseType.size()) {
        for (uint8_t i = 0; i < def.getSize(); i++) {
            if (fieldData[i] != invalidBytes[i]) {
                valid = true;
                break;
            }
        }
    }
    int ival = CFitByteDataTransformer::getSIntValue(baseType, fieldData);
    return new CFitIntField<int32_t>(def, fieldProfile, ival, valid);

}

CFitField *CFitFieldBuilder::buildUIntField(const CFitFieldDefinition &def, const CFitFieldProfile* fieldProfile, uint8_t *fieldData) {
    const CFitBaseType& baseType = def.getBaseType();
    uint8_t *invalidBytes = baseType.invalidValueBytes();
    bool valid = false;
    if (def.getSize() == baseType.size()) {
        for (uint8_t i = 0; i < def.getSize(); i++) {
            if (fieldData[i] != invalidBytes[i]) {
                valid = true;
                break;
            }
        }
    }
    unsigned int uval = CFitByteDataTransformer::getUIntValue(baseType, fieldData);
    return new CFitIntField<uint32_t>(def, fieldProfile, uval, valid);
}


CFitField *CFitFieldBuilder::buildFloatField(const CFitFieldDefinition &def, const CFitFieldProfile* fieldProfile, uint8_t *fieldData) {
    const CFitBaseType& baseType = def.getBaseType();
    uint8_t *invalidBytes = baseType.invalidValueBytes();
    bool valid = false;
    if (def.getSize() == baseType.size()) {
        for (uint8_t i = 0; i < def.getSize(); i++) {
            if (fieldData[i] != invalidBytes[i]) {
                valid = true;
                break;
            }
        }
    }
    double dval = CFitByteDataTransformer::getFloatValue(baseType, fieldData);
    return new CFitFloatField(def, fieldProfile, dval, valid);
}


CFitField *CFitFieldBuilder::buildStringField(const CFitFieldDefinition &def, const CFitFieldProfile* fieldProfile, uint8_t *fieldData) {
    uint8_t *invalidBytes = def.getBaseType().invalidValueBytes();
    QString str = CFitByteDataTransformer::getString(fieldData, def.getSize());
    // all byts set to invalid value
    uint8_t invalidCount = 0;
    for (uint8_t i = 0; i < def.getSize(); i++) {
        if (fieldData[i] == invalidBytes[0])
            invalidCount++;
    }
    bool valid = (invalidCount < def.getSize());
    return new CFitStringField(def, fieldProfile, str, valid);
}


CFitField *CFitFieldBuilder::buildByteField(const CFitFieldDefinition &def, const CFitFieldProfile* fieldProfile, uint8_t *fieldData) {
    uint8_t *invalidBytes = def.getBaseType().invalidValueBytes();
    // all byts set to invalid value
    uint8_t invalidCount = 0;
    for (uint8_t i = 0; i < def.getSize(); i++) {
        if (fieldData[i] == invalidBytes[0])
            invalidCount++;
    }
    bool valid = (invalidCount < def.getSize());
    return new CFitByteField(def, fieldProfile, CFitByteDataTransformer::getBytes(fieldData, def.getSize()), valid);
}


void CFitFieldBuilder::evaluateFieldProfile(CFitMessage& mesg, CFitField& field) {
    const CFitFieldProfile& fieldProfile = field.profile();
    // case subfield
    if (fieldProfile.hasSubfields()) {
        for (const CFitSubfieldProfile* subfieldProfile : fieldProfile.getSubfields()) {
            // the referenced field is for all subfields the same
            for (CFitField *referencedField : mesg.getFields()) {
                if (referencedField->getFieldDefNr() == subfieldProfile->getReferencedFieldDefNr() &&
                    referencedField->getUIntValue() == subfieldProfile->getReferencedFieldValue()) {
                    // the value of the referenced field matches with the field profile reference-value
                    field.setProfile(subfieldProfile);
                }
            }
        }
    }
}

void CFitFieldBuilder::expandComponents(CFitMessage& mesg, const CFitField& field) {
    // TODO accumulated fields are not implemented (no need so far)
    const CFitFieldProfile& fieldProfile = field.profile();
    // case component
    if (fieldProfile.hasComponents()) {
        int offset = 0;
        for (const CFitComponentfieldProfile* compProfile : fieldProfile.getComponents()) {
            if (field.getBaseType().isSignedInt()) {
                int32_t value = (field.getSIntValue() >> offset) & compProfile->getBitmask();
                mesg.addField(new CFitIntField<int32_t>(field, compProfile, value, true));

            }
            else {
                uint32_t value = (field.getUIntValue() >> offset) & compProfile->getBitmask();
                mesg.addField(new CFitIntField<uint32_t>(field, compProfile, value, true));
            }
            offset += compProfile->getBits();
        }
    }
}

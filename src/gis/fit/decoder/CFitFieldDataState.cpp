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

#include "gis/fit/decoder/CFitFieldBuilder.h"
#include "gis/fit/decoder/CFitFieldDataState.h"
#include "gis/fit/defs/fit_const.h"
#include "gis/fit/defs/fit_fields.h"
#include "gis/fit/defs/fit_enums.h"
#include "gis/fit/defs/CFitBaseType.h"

void CFitFieldDataState::reset()
{
    fieldDataIndex = 0;
    fieldIndex = 0;
    devFieldIndex = 0;
}


decode_state_e CFitFieldDataState::process(quint8 &dataByte)
{
    CFitMessage& mesg = *latestMessage();
    CFitDefinitionMessage* defMesg = definition(mesg.getLocalMesgNr());

    // add the read byte to the data array
    fieldData[fieldDataIndex++] = dataByte;

    handleFitField();
    bool allFieldRead =  fieldIndex >= defMesg->getNrOfFields();
    if(allFieldRead)
    {
        handleDevField();
    }
    bool allDevFielRead = devFieldIndex >= defMesg->getNrOfDevFields();

    if (allFieldRead && allDevFielRead)
    {
        // Now that the entire message is decoded we may evaluate subfields and expand components
        CFitFieldBuilder::evaluateSubfieldsAndExpandComponents(mesg);

        devProfile(mesg);

        reset();
        FITDEBUG(2, qDebug() << mesg.messageInfo())
        // after all fields read, go to next record header
        return eDecoderStateRecord;
    }

    // there are more fields to read for the current message
    return eDecoderStateFieldData;
}


bool CFitFieldDataState::handleFitField()
{
    CFitMessage& mesg = *latestMessage();
    CFitDefinitionMessage* defMesg = definition(mesg.getLocalMesgNr());

    if (fieldIndex < defMesg->getNrOfFields())
    {
        const CFitFieldDefinition& fieldDef = defMesg->getFieldByIndex(fieldIndex);
        if (fieldDataIndex >= fieldDef.getSize())
        {
            // all bytes are read for current field

            // new field with data
            CFitField f = CFitFieldBuilder::buildField(fieldDef, fieldData, mesg);
            mesg.addField(f);

            // The special case time record.
            // timestamp has always the same value for all enums. it does not matter against which we're comparing.
            if (fieldDef.getDefNr() == eRecordTimestamp)
            {
                setTimestamp(f.getValue().toUInt());
            }

            // new field follows, reset
            fieldDataIndex = 0;
            fieldIndex++;
        }
    }
    return fieldIndex >= defMesg->getNrOfFields();
}

bool CFitFieldDataState::handleDevField()
{
    CFitMessage& mesg = *latestMessage();
    CFitDefinitionMessage* defMesg = definition(mesg.getLocalMesgNr());

    if (devFieldIndex < defMesg->getNrOfDevFields())
    {
        const CFitFieldDefinition& fieldDef = defMesg->getDevFieldByIndex(devFieldIndex);
        if (fieldDataIndex >= fieldDef.getSize())
        {
            // handling developer data for mapping the field data to its definitions:
            // part 2, reading field data and attach dynamic profile
            CFitFieldProfile* fieldProfile = devFieldProfile(fieldDef.getDefNr());
            if (fieldProfile->getBaseType().nr() == eBaseTypeNrInvalid)
            {
                // test if profile exists
                throw tr("Missing field definition for development field.");
            }

            CFitField f = CFitFieldBuilder::buildField(*fieldProfile, fieldDef, fieldData, mesg);
            mesg.addField(f);

            // new field follows, reset
            fieldDataIndex = 0;
            devFieldIndex++;
        }
    }
    return devFieldIndex >= defMesg->getNrOfDevFields();
}

void CFitFieldDataState::devProfile(CFitMessage& mesg)
{
    // handling developer data for mapping the field data to its definitions:
    // part 1, dynamic profiles creation
    if (mesg.getGlobalMesgNr() == eMesgNumDeveloperDataId)
    {
        clearDevFieldProfiles();
    }
    if (mesg.getGlobalMesgNr() == eMesgNumFieldDescription)
    {
        CFitFieldProfile devFieldProfile = buildDevFieldProfile(mesg);
        FITDEBUG(2, qDebug() << devFieldProfile.fieldProfileInfo());
        addDevFieldProfile(devFieldProfile);
    }
}

CFitFieldProfile CFitFieldDataState::buildDevFieldProfile(CFitMessage& mesg)
{
    QString fieldName;
    quint8 fieldDefNr = 0;
    quint8 devDataIdx = 0;
    quint8 baseType = eBaseTypeNrInvalid;
    quint8 scale = 0;
    quint8 array = 0;
    QString components;
    qint8 offset = 0;
    QString units;
    QString bits;
    QString accumulate;
    quint8 baseUnitId = 0;
    quint8 natvieMesgNum = 0;
    quint8 nativeFieldNum = 0;

    for (const CFitField field : mesg.getFields()) {
        switch (field.getFieldDefNr()) {
            case eFieldDescriptionDeveloperDataIndex:
                devDataIdx = field.getValue().toUInt();
                break;
            case eFieldDescriptionFieldDefinitionNumber:
                fieldDefNr = field.getValue().toUInt();
                break;
            case eFieldDescriptionFitBaseTypeId:
                baseType = field.getValue().toUInt(); // enum
                break;
            case eFieldDescriptionFieldName:
                fieldName = field.getValue().toString();
                break;
            case eFieldDescriptionArray:
                array = field.getValue().toUInt();
                break;
            case eFieldDescriptionComponents:
                components = field.getValue().toString();
                break;
            case eFieldDescriptionScale:
                scale = field.getValue().toUInt();
                break;
            case eFieldDescriptionOffset:
                offset = field.getValue().toInt();
                break;
            case eFieldDescriptionUnits:
                units = field.getValue().toString();
                break;
            case eFieldDescriptionBits:
                bits = field.getValue().toString();
                break;
            case eFieldDescriptionAccumulate:
                accumulate = field.getValue().toString();
                break;
            case eFieldDescriptionFitBaseUnitId:
                baseUnitId = field.getValue().toUInt(); // enum
                break;
            case eFieldDescriptionNativeMesgNum:
                natvieMesgNum = field.getValue().toUInt(); // enum
                break;
            case eFieldDescriptionNativeFieldNum:
                nativeFieldNum = field.getValue().toUInt();
                break;
            default:
                throw tr("FIT decoding error: invalid field def nr %1 while creating dev field profile.")
                        .arg(field.getFieldDefNr());
                break;
        }
    }

    CFitFieldProfile devFieldProfile = CFitFieldProfile(nullptr, fieldName, *CFitBaseTypeMap::get(baseType), fieldDefNr,
                                                        (qreal) scale, (quint16) offset, units, eFieldTypeDevelopment);
    // TODO complete field profile

    // TODO a development field definition is is linked to an developer data ID. It is needed if more than one
    // TODO developer data ID is inside a fit file.
    return devFieldProfile;
}
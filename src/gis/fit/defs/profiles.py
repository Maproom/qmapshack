#!/usr/bin/env python
# vim:fileencoding=utf-8

from __future__ import print_function
import csv


type_csv_filename = './Profile_Types.csv'
messages_csv_filename = './Profile_Messages.csv'

enum_filename = 'gen-fit_enums.h'
enum_profiles_filename = 'gen-fit_fields.h'
profile_code_filename = 'gen-CFitProfileLockup.cpp'
profile_init_filename = 'gen-CFitProfileLockup-call.cpp'


def titlecase(s):
    s = s.replace('_', ' ')
    s = s.title()
    s = s.replace(' ', '')
    return s


def zero_if_null(s, i = None):
    return default_if_null('0', s, i)


def empty_if_none(s, i = None):
    return default_if_null('', s, i)


def default_if_null(default, s, i = None):
    if i is not None:
        if len(s) > i:
            s = s[i]
        else:
            s = ''

    if not s:
        return default
    return s


def write_enum_type(enum_file, enum_list, type_name):
    if len(enum_list) > 0 and type_name:
        count = 0
        print("typedef enum {", file=enum_file)
        for e in enum_list:
            if count + 1 < len(enum_list):
                print('    ' + e + ',', file=enum_file)
            else:
                print('    ' + e, file=enum_file)
            count = count + 1
        print('} ' + type_name + '_e;', file=enum_file)
        print(' ', file=enum_file)

def write_enum_file():
    # field enums
    count = 0
    last_type_name = ''
    enum_list = []

    with open(type_csv_filename, 'r') as csvfile:
        typereader = csv.reader(csvfile, delimiter=';', quotechar='"')
        with open(enum_filename, 'w') as enum_file:
            for row in typereader:
                type_name = row[0]
                base_type = row[1]
                value_name = row[2]
                value = row[3]
                comment = row[4]

                if count > 0:
                    if type_name:
                        write_enum_type(enum_file, enum_list, last_type_name)
                        enum_list = []
                        last_type_name = type_name

                    if value_name and value:
                        enum_list.append('e' + titlecase(last_type_name) + titlecase(value_name) + ' = ' + value)

                count = count + 1
            write_enum_type(enum_file, enum_list, last_type_name)


def write_message_enums():
    count = 0
    last_type_name = ''
    enum_list = []

    with open(messages_csv_filename, 'r') as csvfile:
        typereader = csv.reader(csvfile, delimiter=';', quotechar='"')
        with open(enum_profiles_filename, 'w') as enum_file:
            for row in typereader:
                message_name = row[0]
                field_def_nr = row[1]
                field_name = row[2]

                if count > 0:
                    if message_name:
                        # '} message_' + + '_e;'
                        write_enum_type(enum_file, enum_list, 'message_' + last_type_name)
                        enum_list = []
                        last_type_name = message_name

                    if field_name and field_def_nr:
                        enum_list.append('e' + titlecase(last_type_name) + titlecase(field_name) + ' = ' + field_def_nr)

                count = count + 1
            write_enum_type(enum_file, enum_list, 'message_' + last_type_name)



def write_profile_type(profile_file, field_list, msg_name):
    if len(field_list) > 0 and msg_name:
        print('void init' + titlecase(msg_name) + '(QMap<quint16, CFitProfile*>& profiles)', file=profile_file)
        print('{', file=profile_file)
        print('    CFitProfile* f = new CFitProfile("' + msg_name + '", eMesgNum' + titlecase(msg_name) + ');', file=profile_file)
        for f in field_list:
            print('    f->' + f + ';', file=profile_file)
        print('    profiles.insert(eMesgNum' + titlecase(msg_name) + ', f);', file=profile_file)
        print('}', file=profile_file)
        print(' ', file=profile_file)


def write_profile_init(profile_file, msg_name):
    if msg_name:
        print('    init' + titlecase(msg_name) + '(allProfiles);', file=profile_file)


base_type_map = {
    'uint8' : 'fitUint8Type', 'uint16': 'fitUint16Type', 'uint32':'fitUint32Type', 'uint64':'fitUint64Type',
    'sint8':'fitSint8Type', 'sint16':'fitSint16Type', 'sint32': 'fitSint32Type', 'sint64':'fitSint64Type',
    'uint8z':'fitUint8zType', 'uint16z':'fitUint16zType', 'uint32z':'fitUint32zType', 'uint64z':'fitUint64zType',
    'float32':'fitFloat32Type', 'float64':'fitFloat64Type',
    'string':'fitStringType', 'byte':'fitByteType', 'bool':'fitEnumType', '':'fitEnumType'}


def base_type(type):
    if type in base_type_map:
        return base_type_map[type]
    return 'fitEnumType'


def write_profiles():
    count = 0
    subcount = 0
    last_msg_name = ''
    field_names = {}
    add_list = []

    profile_file = open(profile_code_filename, 'w')
    profile_init_file = open(profile_init_filename, 'w')

    with open(messages_csv_filename, 'r') as csvfile:
        typereader = csv.reader(csvfile, delimiter=';', quotechar='"')
        for row in typereader:
            message_name = row[0]
            field_def_nr = row[1]
            field_name = row[2]
            field_type = row[3]
            array = row[4]
            components = row[5]
            scale = row[6]
            offset = row[7]
            units = row[8]
            bits = row[9]
            accumulate = row[10]
            ref_field_name = row[11]
            ref_field_value = row[12]

            if count > 0:
                if message_name:
                    # new message
                    write_profile_type(profile_file, add_list, last_msg_name)
                    write_profile_init(profile_init_file, last_msg_name)
                    add_list = []
                    last_msg_name = message_name

                elif field_name:
                    type_name = base_type(field_type)
                    field_names[field_name] = field_type

                    if field_def_nr and not components:
                        # normal type
                        scale = zero_if_null(scale)
                        offset = zero_if_null(offset)

                        last_field_def_nr = 'e' + titlecase(last_msg_name) + titlecase(field_name)
                        add_list.append('addField("' + field_name + '", ' + type_name + ', ' + last_field_def_nr + ', '
                                        + scale + ', ' + offset + ', "' + units + '")')
                        subcount = 0
                        # end normal type
                    else:
                        # sub type or component type

                        # ref field name is for all elements the same, so take the first one
                        ref_field_name = empty_if_none(ref_field_name.split(','), 0)
                        ref_field_value_list = ref_field_value.split(',')
                        components_list = components.split(',')
                        scale_list = scale.split(',')
                        offset_list = offset.split(',')
                        units_list = units.split(',')
                        bits_list = bits.split(',')

                        if not field_def_nr:
                            # sub type
                            enum_field = 'e' + titlecase(last_msg_name) + titlecase(ref_field_name)

                            # case it has components (scale, offset and units are undefined)
                            scale = '0'
                            offset = '0'
                            units = ''
                            i = 0
                            for a in ref_field_value_list:
                                if not components:
                                    scale = zero_if_null(scale_list, i)
                                    offset = zero_if_null(offset_list, i)
                                    units = empty_if_none(units_list, i)

                                if ref_field_name not in field_names or not field_names[ref_field_name]:
                                    enum_field_value = 'e' + titlecase(ref_field_name) + titlecase(ref_field_value_list[i])
                                else:
                                    enum_field_value = 'e' +titlecase(field_names[ref_field_name]) + titlecase(ref_field_value_list[i])

                                add_list.append('addSubfield("' + field_name + '", ' + type_name + ', ' + last_field_def_nr + ', '
                                                + scale + ', ' + offset + ', "' + units + '", ' + enum_field + ', ' + enum_field_value+')')
                                i = i + 1
                                subcount = subcount + 1
                            #  end sub type
                        if components:
                            # component type
                            if field_def_nr:
                                # a component needs always a parent field. add one
                                last_field_def_nr = 'e' + titlecase(last_msg_name) + titlecase(field_name)
                                add_list.append('addField("' + field_name + '", ' + type_name + ', ' + last_field_def_nr + ', 0, 0, "")')
                                subaccess_field = ''
                            else:
                                # parent field for component is sub field
                                subaccess_field = str(subcount - 1) + ', '

                            i = 0
                            for a in components_list:
                                scale = zero_if_null(scale_list, i)
                                if not scale:
                                    scale = zero_if_null(scale_list, 0)
                                offset = zero_if_null(offset_list, i)
                                units = empty_if_none(units_list, i)
                                if not units:
                                    units = empty_if_none(units_list, 0)
                                bits = zero_if_null(bits_list, i)
                                if not bits:
                                    bits = zero_if_null(bits_list, 0)
                                enum_component = 'e' + titlecase(last_msg_name) + titlecase(components_list[i])
                                add_list.append('addComponent('+ subaccess_field + '"' + field_name + '", ' + type_name + ', ' +
                                                last_field_def_nr + ', ' + scale + ', ' + offset + ', "' + units + '", ' +
                                                enum_component + ', ' + bits + ')')
                                i = i + 1
                            # end component type
            count = count + 1
        write_profile_type(profile_file, add_list, last_msg_name)
        write_profile_init(profile_init_file, last_msg_name)


write_enum_file()
write_message_enums()
write_profiles()
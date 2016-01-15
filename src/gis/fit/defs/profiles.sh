#!/usr/local/bin/bash
# /bin/bash

tmp_array=()

function splitField()
{
	local  in=$1
	in=${in#"\""}
	in=${in%"\""}
	in=${in//,/ }
	OLDIFS=$IFS
	IFS=" "
	tmp_array=($in)
	IFS=$OLDIFS
}

function zeroIfNull()
{
    local  __resultvar=$1
    local  result=$2
    if [[ "$result" == "" ]]; then
        result="0"
    fi
    
    eval $__resultvar="'$result'"
}


function titleCase()
{
    local __resultvar=$1
    local string=$2
    
    string=${string//_/ }
    titleCasev=$(echo "$string" | perl -ane ' foreach $wrd ( @F ) { print ucfirst($wrd)." "; } print "\n" ; ')
    titleCasev=${titleCasev// /}

    eval $__resultvar="'$titleCasev'"
}


function createFieldEnums()
{
	COUNT=0
	lastTypeName=""
	lastTypeNameCamel=""
	lastValue=""
	ENUM_FILE=gen-fit_enums.h
	rm $ENUM_FILE

	while read typeName baseType valueName value comment
	do
		if [[ $COUNT -gt 0 ]]; then
			if [[ "$typeName" != "" ]]; then
				lastTypeName=$typeName
				titleCase lastTypeNameCamel $typeName
				lastValue=""
				
				echo "typedef enum {" >> $ENUM_FILE
			fi
			
			if [[ "$lastValue" != "" && "$valueName" != "" ]]; then
					echo " $lastValue," >> $ENUM_FILE
			fi
			if [[ "$lastTypeName" != "" && "$valueName" == "" && "$typeName" == "" ]]; then
				echo " $lastValue" >> $ENUM_FILE
			echo "} ${lastTypeName}_e;"  >> $ENUM_FILE
				echo "" >> $ENUM_FILE
			fi
			
			if [[ "$valueName" != "" ]]; then 
				titleCase valueName $valueName
				lastValue="e$lastTypeNameCamel$valueName = $value"
			fi
		fi
		COUNT=$[$COUNT +1]
	done < $IN_TYPES
	echo " $lastValue" >> $ENUM_FILE
	echo "} ${lastTypeName}_e;"  >> $ENUM_FILE
}


function baseType()
{
    local __resultvar=$1
    local ctype=$2
    
    if [[ "$ctype" == "uint8" ]]; then
        conv="Uint8Type"
    elif [[ "$ctype" == "uint16" ]]; then
        conv="Uint16Type"
    elif [[ "$ctype" == "uint32" ]]; then
        conv="Uint32Type"
    elif [[ "$ctype" == "sint8" ]]; then
        conv="Sint8Type"
    elif [[ "$ctype" == "sint16" ]]; then
        conv="Sint16Type"
    elif [[ "$ctype" == "sint32" ]]; then
        conv="Sint32Type"
    elif [[ "$ctype" == "string" ]]; then
        conv="StringType"
    elif [[ "$ctype" == "uint8z" ]]; then
        conv="Uint8zType"
    elif [[ "$ctype" == "uint16z" ]]; then
        conv="Uint16zType"
    elif [[ "$ctype" == "uint32z" ]]; then    
        conv="Uint32zType"
    elif [[ "$ctype" == "float32" ]]; then
        conv="Float32Type"
    elif [[ "$ctype" == "float64" ]]; then
        conv="Float64Type"
    elif [[ "$ctype" == "byte" ]]; then
        conv="ByteType"
    elif [[ "$ctype" == "bool" ]]; then
        conv="EnumType"
    else
        conv="EnumType"
    fi

    eval $__resultvar="'$conv'"
}

function createMessageEnums()
{
	ENUM_PROFILES_FILE=gen-fit_fields.h
	rm $ENUM_PROFILES_FILE
	COUNT=0
	lastName=""
	valueName=""
	lastTypeName=""
	lastTypeNameCamel=""
	while read messageName fieldDefNr fieldName fieldType array components scale offset units bits accumulate refFieldName refFieldValeu comment example
	do
		if [[ $COUNT -gt 1 ]]; then
			if [[ "$messageName" != "" ]]; then
				lastTypeName=$messageName
				titleCase lastTypeNameCamel $lastTypeName
				lastValue=""
				
				echo "typedef enum {" >> $ENUM_PROFILES_FILE
			fi
			
			if [[ "$lastValue" != "" && "$fieldDefNr" != "" ]]; then
				echo " $lastValue," >> $ENUM_PROFILES_FILE
				lastValue=""    
			fi
			if [[ "$lastTypeName" != "" && "$fieldName" == "" && "$messageName" == "" && "$lastValue" != "" ]]; then
				echo " $lastValue" >> $ENUM_PROFILES_FILE
				echo "} message_${lastTypeName}_e;"  >> $ENUM_PROFILES_FILE
				echo "" >> $ENUM_PROFILES_FILE
				lastValue=""
			fi
			
			if [[ "$fieldDefNr" != "" ]]; then 
				titleCase fieldName $fieldName
				lastValue="e$lastTypeNameCamel$fieldName = $fieldDefNr"
			fi
		fi
		COUNT=$[$COUNT +1]
	done < $IN_MESSAGES
	echo " $lastValue" >> $ENUM_PROFILES_FILE
	echo "} message_${lastTypeName}_e;"  >> $ENUM_PROFILES_FILE
}


function createProfiles()
{
	CODE_FILE=gen-CFitProfileLockup.cpp
	CALL_FILE=gen-CFitProfileLockup-call.cpp
	rm $CODE_FILE
	rm $CALL_FILE
	
	COUNT=0
	subcount=0
	lastName=""
	lastFieldDefNr=0
	typeName=""
	enumName=""
	enumField=""
	enumFieldValue=""
	enumComponent=""
	declare -A fieldNames
	
	# second run create the source code
	while read messageName fieldDefNr fieldName fieldType array components scale offset units bits accumulate refFieldName refFieldValue comment example
	do
		if [ $COUNT -gt 1 ]; then
			if [[ "$messageName" != "" ]]; then
				# new message
				if [ $COUNT -gt 3 ]; then
					echo "profiles.insert($enumName, f);" >> $CODE_FILE
					echo "}" >> $CODE_FILE
					echo " " >> $CODE_FILE
				fi
				titleCase enumName $messageName
				echo "init$enumName(profiles);" >> $CALL_FILE
				echo "void init$enumName(QMap<quint16, CFitProfile*>& profiles)" >> $CODE_FILE
				echo "{" >> $CODE_FILE
				enumName=eMesgNum$enumName
				echo "CFitProfile* f = new CFitProfile(\"$messageName\", $enumName);" >> $CODE_FILE
				
				lastName=$messageName
				
			elif [[ "$fieldName" != "" ]]; then
				# new fields
				baseType typeName $fieldType
				fieldNames["$fieldName"]=$fieldType
				
				if [[ "$fieldDefNr" != "" &&  "$components" == "" ]]; then
					# normal type
					zeroIfNull scale $scale
					zeroIfNull offset $offset
					titleCase lastFieldDefNr "${lastName}_${fieldName}"
					lastFieldDefNr="e$lastFieldDefNr"
					echo "f->addField(\"$fieldName\", $typeName, $lastFieldDefNr, $scale, $offset, \"$units\");" >> $CODE_FILE
					subcount=0
					# end normal type
				else
					# sub type or component type
					splitField $refFieldName
					# ref field name is for all elements the same, so take the first one
					refFieldName=$tmp_array
					splitField $refFieldValue
					refFieldValueArray=( "${tmp_array[@]}" )
					
					splitField $components
					componentsArray=( "${tmp_array[@]}" )
					splitField $scale
					scaleArray=( "${tmp_array[@]}" )
					splitField $offset
					offsetArray=( "${tmp_array[@]}" )
					splitField $units
					unitsArray=( "${tmp_array[@]}" )
					splitField $bits
					bitsArray=( "${tmp_array[@]}" )
					
					if [[ "$fieldDefNr" == "" ]]; then
						# sub type
						titleCase enumField "${lastName}_${refFieldName}"
						enumField=e$enumField
						for i in "${!refFieldValueArray[@]}"; do
							if [[ "$components" != "" ]]; then
								# case it has components (scale, offset and units are undefined)
								scale="0"
								offset="0"
								units=""
							else
								zeroIfNull scale ${scaleArray[i]}
								zeroIfNull offset ${offsetArray[i]}
								units=${unitsArray[i]}
							fi
							
							titleCase enumFieldValue "${fieldNames[$refFieldName]}_${refFieldValueArray[i]}"
							if [[ ${fieldNames[$refFieldName]} == "" ]]; then
								titleCase enumFieldValue "${refFieldName}_${refFieldValueArray[i]}"
							fi
							enumFieldValue="e$enumFieldValue"
							echo "f->addSubfield(\"$fieldName\", $typeName, $lastFieldDefNr, $scale, $offset, \"$units\", $enumField, $enumFieldValue);" >> $CODE_FILE 
							subcount=$[$subcount +1]
						done
						#  end sub type
					fi
					if [[ "$components" != "" ]]; then
						# component type
						
						if [[ "$fieldDefNr" != "" ]]; then
							# a component needs always a parent field. add one
							titleCase lastFieldDefNr "${lastName}_${fieldName}"
							lastFieldDefNr="e$lastFieldDefNr"
							echo "f->addField(\"$fieldName\", $typeName, $lastFieldDefNr, 0, 0, \"\");" >> $CODE_FILE
							subaccessField=""
						else
							# parent field for component is sub field
							i=$[$subcount -1]
							subaccessField="$i,"
						fi
						for i in "${!componentsArray[@]}"; do
							zeroIfNull scale ${scaleArray[i]}
							zeroIfNull offset ${offsetArray[i]}
							units=${unitsArray[i]}
							bits=${bitsArray[i]}
							compName=${componentsArray[i]}
							titleCase enumComponent "${lastName}_${componentsArray[i]}"
							enumComponent="e$enumComponent"
							echo "f->addComponent($subaccessField \"$fieldName\", $typeName, $lastFieldDefNr, $scale, $offset, \"$units\", $enumComponent, $bits);" >> $CODE_FILE
						done
						# end component type
					fi
				fi
			fi
			#else
			# empty line
		fi
		COUNT=$[$COUNT +1]
	done < $IN_MESSAGES
	echo "}" >> $CODE_FILE
}


# line ending unix 
IN_TYPES=./Profile_Types.csv
IN_MESSAGES=./Profile_Messages.csv

OLDIFS=$IFS
IFS=";"

[ ! -f $IN_TYPES ] && { echo "$IN_TYPES file not found"; exit 99; }
createFieldEnums

[ ! -f $IN_MESSAGES ] && { echo "$IN_MESSAGES file not found"; exit 99; }
createMessageEnums
createProfiles

IFS=$OLDIFS

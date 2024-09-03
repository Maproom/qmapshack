/**********************************************************************************************
    Copyright (C) 2014-2016 Oliver Eichler <oliver.eichler@gmx.de>

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

#include "helpers/CValue.h"

#include "helpers/CSettings.h"

QSet<CValue*> CValue::allValues;

CValue::CValue(const QString& cfgTag, const QVariant& initDefault, fMarkChanged markChanged, fValueOnChange onChange)
    : cfgTag(cfgTag),
      initDefault(initDefault),
      valUser(initDefault),
      funcOnChange(onChange),
      funcMarkChanged(markChanged) {
  if (onChange != nullptr) {
    onChange(val());
  }

  allValues << this;
}

CValue::~CValue() { allValues.remove(this); }

void CValue::setMode(mode_e m) {
  bool markAsChanged = mode != m;

  mode = m;

  if (funcOnChange != nullptr) {
    funcOnChange(val());
  }

  if (markAsChanged) {
    funcMarkChanged();
  }
}

QVariant CValue::val() const {
  if (mode == eModeUser) {
    return valUser;
  }

  SETTINGS;
  return cfg.value(cfgTag, initDefault);
}

const QVariant& CValue::operator=(const QVariant& v) {
  bool markAsChanged = false;

  if (mode == eModeUser) {
    markAsChanged = valUser != v;
    valUser = v;
  } else {
    SETTINGS;
    cfg.setValue(cfgTag, v);

    for (CValue* value : std::as_const(allValues)) {
      if (value != this) {
        value->updateSys(cfgTag, v);
      }
    }
  }

  if (funcOnChange != nullptr) {
    funcOnChange(v);
  }

  if (markAsChanged) {
    funcMarkChanged();
  }
  return v;
}

void CValue::updateSys(const QString& tag, const QVariant& val) {
  if ((mode == eModeSys) && (tag == cfgTag) && (funcOnChange != nullptr)) {
    funcOnChange(val);
  }
}

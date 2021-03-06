//
// Copyright (C) 2016 Red Hat, Inc.
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
// Authors: Daniel Kopecek <dkopecek@redhat.com>
//
#include "UEvent.hpp"
#include "UEventParser.hpp"
#include "Logger.hpp"

namespace usbguard
{
  UEvent::UEvent()
  {
  }

  UEvent::UEvent(UEvent&& rhs)
    : _attributes(std::move(rhs._attributes))
  {
  }

  UEvent& UEvent::operator=(UEvent&& rhs)
  {
    _attributes = std::move(rhs._attributes);
    return *this;
  }

  UEvent UEvent::fromString(const String& uevent_string, bool attributes_only, bool trace)
  {
    UEvent uevent;
    parseUEventFromString(uevent_string, uevent, attributes_only, trace);
    return std::move(uevent);
  }

  void UEvent::clear()
  {
    _attributes.clear();
  }

  void UEvent::setAttribute(const String& name, const String& value)
  {
    USBGUARD_LOG(Trace) << "Setting attribute: " << name << "=" << value;
    _attributes[name] = value;
  }

  String UEvent::getAttribute(const String& name) const
  {
    auto it = _attributes.find(name);
    if (it == _attributes.end()) {
      return String();
    } else {
      return it->second;
    }
  }

  bool UEvent::hasAttribute(const String& name) const
  {
    return _attributes.count(name) == 1;
  }

  String UEvent::getHeaderLine() const
  {
    if (!hasAttribute("ACTION") ||
        !hasAttribute("DEVPATH")) {
      throw std::runtime_error("uevent: missing required header line values");
    }

    String header_line;

    header_line.append(getAttribute("ACTION"));
    header_line.append(1, '@');
    header_line.append(getAttribute("DEVPATH"));

    return header_line;
  }

  String UEvent::toString(char separator) const
  {
    String uevent_string = getHeaderLine();

    uevent_string.append(1, separator);

    for (auto const& kv_pair : _attributes) {
      uevent_string.append(kv_pair.first);
      uevent_string.append(1, '=');
      uevent_string.append(kv_pair.second);
      uevent_string.append(1, separator);
    }

    return uevent_string;
  }

  bool UEvent::hasRequiredAttributes() const
  {
    for (const String name : { "ACTION", "DEVPATH", "SUBSYSTEM" }) {
      if (!hasAttribute(name)) {
        return false;
      }
    }
    return true;
  }
} /* namespace usbguard */

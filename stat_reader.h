#pragma once

#include <iosfwd>
#include <string_view>

#include "transport_catalogue.h"

namespace Statistic {

void ParseAndPrintStat(const Catalogue::TransportCatalogue& tansport_catalogue,
    std::string_view request, std::ostream& output);

void PrintBus(const Catalogue::TransportCatalogue& tansport_catalogue,
    std::string_view request, std::ostream& output);

void PrintStop(const Catalogue::TransportCatalogue& tansport_catalogue,
    std::string_view request, std::ostream& output);

} // namespace Statistic
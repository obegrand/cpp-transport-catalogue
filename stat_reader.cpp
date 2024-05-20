#include "stat_reader.h"

namespace Statistic {

void ParseAndPrintStat(const Catalogue::TransportCatalogue& tansport_catalogue,
	std::string_view request, std::ostream& output) {
	std::string_view command = request.substr(0, request.find(' '));
	output << request << ": ";
	request.remove_prefix(request.find(' ') + 1);

	if (command == "Bus") {
		PrintBus(tansport_catalogue, request, output);
	}
	else if (command == "Stop") {
		PrintStop(tansport_catalogue, request, output);
	}
}

void PrintBus(const Catalogue::TransportCatalogue& tansport_catalogue,
	std::string_view request, std::ostream& output) {
	if (!tansport_catalogue.ContainsBus(request)) {
		output << "not found" << std::endl;
	}
	else {
		Catalogue::Bus& bus = tansport_catalogue.GetBus(request);
		output << bus.stop_names.size() << " stops on route, ";
		std::set<std::string_view> countUniqueStops;
		for (auto stop : bus.stop_names) {
			countUniqueStops.insert(stop->name);
		}
		output << countUniqueStops.size() << " unique stops, "
			<< tansport_catalogue.ComputeStopsDistance(bus.stop_names)
			<< " route length" << std::endl;
	}
}

void PrintStop(const Catalogue::TransportCatalogue& tansport_catalogue,
	std::string_view request, std::ostream& output) {
	if (!tansport_catalogue.ContainsStop(request)) {
		output << "not found" << std::endl;
	}
	else {
		std::set<std::string_view> buses = tansport_catalogue.GetBusesByStop(request);
		if (buses.empty()) {
			output << "no buses" << std::endl;
		}
		else {
			output << "buses";
			for (const auto& bus : buses) {
				output << " " << bus;
			}
			output << std::endl;
		}
	}
}

} // namespace Statistic
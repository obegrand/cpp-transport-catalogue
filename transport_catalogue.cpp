#include "transport_catalogue.h"
#include <string_view>

namespace Catalogue {

void TransportCatalogue::Add(Stop stop) {
	stops_storage_.push_back(stop);

	stops_[stops_storage_.back().name] = &stops_storage_.back();
	buses_through_stop_[stops_storage_.back().name];
}

void TransportCatalogue::Add(std::string_view number, std::vector<std::string_view> stops) {
	Bus newBus = { .number = std::string(number),.stop_names = {} };

	for (const auto& stop : stops) {
		for (const auto& [key,val] : stops_) {
			if (stop == key) {
				newBus.stop_names.push_back(stops_[key]);
			}
		}
	}

	buses_storage_.push_back(newBus);
	buses_[buses_storage_.back().number] = &buses_storage_.back();

	for (const auto& stop : stops) {
		buses_through_stop_[stop].insert(buses_storage_.back().number);
	}
}


Bus& TransportCatalogue::GetBus(std::string_view number) const {
	return buses_.contains(number) ? *buses_.at(number) : throw std::runtime_error("Bus not found");
}

Stop& TransportCatalogue::GetStop(std::string_view name) const {
	return stops_.contains(name) ? *stops_.at(name) : throw std::runtime_error("Stop not found");
}

std::set<std::string_view> TransportCatalogue::GetBusesByStop(const std::string_view name) const {
	return buses_through_stop_.at(name);
}

bool TransportCatalogue::ContainsBus(std::string_view number)const {
	return buses_.contains(number);
}

bool TransportCatalogue::ContainsStop(std::string_view name)const {
	return stops_.contains(name);
}

double TransportCatalogue::ComputeStopsDistance(const std::vector<Stop*> stop_names) const {
	double result = 0.0;

	for (size_t i = 1; i < stop_names.size(); i++) {
		Geo::Coordinates from = stop_names[i - 1]->coordinates;
		Geo::Coordinates to = stop_names[i]->coordinates;

		result += Geo::ComputeDistance(std::move(from), std::move(to));
	}
	return result;
}

} // namespace TransportCatalogue
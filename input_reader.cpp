#include "input_reader.h"

#include <algorithm>
#include <cassert>
#include <iterator>

namespace InputCommand {

// ������ ������ ���� "10.123,  -30.1837" � ���������� ���� ��������� (������, �������)
Geo::Coordinates ParseCoordinates(std::string_view str) {
	static const double nan = std::nan("");

	auto not_space = str.find_first_not_of(' ');
	auto comma = str.find(',');

	if (comma == str.npos) {
		return { nan, nan };
	}

	auto not_space2 = str.find_first_not_of(' ', comma + 1);

	double lat = std::stod(std::string(str.substr(not_space, comma - not_space)));
	double lng = std::stod(std::string(str.substr(not_space2)));

	return { lat, lng };
}

// ������� ������� � ������ � ����� ������
std::string_view Trim(std::string_view string) {
	const auto start = string.find_first_not_of(' ');
	if (start == string.npos) {
		return {};
	}
	return string.substr(start, string.find_last_not_of(' ') + 1 - start);
}

// ��������� ������ string �� n �����, � ������� ���������� �������-����������� delim
std::vector<std::string_view> Split(std::string_view string, char delim) {
	std::vector<std::string_view> result;

	size_t pos = 0;
	while ((pos = string.find_first_not_of(' ', pos)) < string.length()) {
		auto delim_pos = string.find(delim, pos);
		if (delim_pos == string.npos) {
			delim_pos = string.size();
		}
		if (auto substr = Trim(string.substr(pos, delim_pos - pos)); !substr.empty()) {
			result.push_back(substr);
		}
		pos = delim_pos + 1;
	}

	return result;
}

// ������ �������.
// ��� ���������� �������� (A>B>C>A) ���������� ������ �������� ��������� [A,B,C,A]
// ��� ������������ �������� (A-B-C-D) ���������� ������ �������� ��������� [A,B,C,D,C,B,A]
std::vector<std::string_view> ParseRoute(std::string_view route) {
	if (route.find('>') != route.npos) {
		return Split(route, '>');
	}

	auto stops = Split(route, '-');
	std::vector<std::string_view> results(stops.begin(), stops.end());
	results.insert(results.end(), std::next(stops.rbegin()), stops.rend());

	return results;
}

CommandDescription ParseCommandDescription(std::string_view line) {
	auto colon_pos = line.find(':');
	if (colon_pos == line.npos) {
		return {};
	}

	auto space_pos = line.find(' ');
	if (space_pos >= colon_pos) {
		return {};
	}

	auto not_space = line.find_first_not_of(' ', space_pos);
	if (not_space >= colon_pos) {
		return {};
	}

	return { std::string(line.substr(0, space_pos)),
			std::string(line.substr(not_space, colon_pos - not_space)),
			std::string(line.substr(colon_pos + 1)) };
}



std::unordered_map<std::string, double> ParseDistance(std::string_view string){
	size_t first = string.find_first_of(',');
	size_t last = string.find_last_of(',');
	if (last == first) {
		return {};
	}
	else {
		string = string.substr(string.find_first_of(',', first + 1)+2, string.size() - string.find_first_of(',', first + 1));
		
		std::unordered_map<std::string, double> result;
		while (!string.empty()) {
			double distance = static_cast<double>(std::stod(std::string(string.substr(0,string.find_first_of('m')))));
			std::string_view stop_name = string.substr(string.find_first_of('m')+5, string.find_first_of(',')- string.find_first_of('m') -5);
			result.insert({ std::string(stop_name),distance });
			string.find_first_of(',') < last ? string = string.substr(string.find_first_of(',') + 1, string.size() - string.find_first_of(',') + 1) : string = {};
		}

		return result;
	}
}

// ������ ������ � ��������� CommandDescription � ��������� ��������� � commands_
void InputReader::ParseLine(std::string_view line) {
	auto command_description = ParseCommandDescription(line);
	if (command_description) {
		commands_.push_back(std::move(command_description));
	}
}

// ��������� ������� ������������ ����������, ��������� ������� �� commands_
void InputReader::ApplyCommands(Catalogue::TransportCatalogue& catalogue) const {
	for (CommandDescription command : commands_) {
		if (command.command == "Stop") {
			catalogue.Add({ (command.id), ParseCoordinates(command.description),ParseDistance(command.description) });
		}
	}
	for (CommandDescription command : commands_) {
		if (command.command == "Bus") {
			catalogue.Add( command.id, ParseRoute(command.description));
		}
	}
}


} // namespace inputCommand
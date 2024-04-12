#include "core/detail/packet_parser.h"

namespace dn {
	namespace detail {
		std::ostream& operator<<(std::ostream& o, const DNPacketParsed& _p) {
			o << "--- PACKET INFO ---\n";
			o << "FROM : " << _p.SrcHost << "\n";
			o << "TO   : " << _p.DstHost << "\n";
			o << "MSG  : " << _p.Message << "\n";
			for (const auto& pr : _p.DataMap) {
				o << pr.first << " : ";
				std::visit([&](const auto& x) { o << x; }, pr.second);
				o << "\n";
			}
			o << "-------------------\n";
			return o;
		}

		DNPacketParsed DNPacketParser::parse(const DNPacket& __in _packet) {
			std::string _dataSrc{ _packet.getData() };
			return parse(_dataSrc);
		}

		DNPacketParsed DNPacketParser::parse(const std::string& __in _msg) {
			DNPacketParsed result{};
			size_t index = 0;

			static auto strSplit = [](std::string input, char delimiter) {
				std::vector<std::string> answer;
				std::stringstream ss(input);
				std::string temp;
				while (getline(ss, temp, delimiter)) {
					answer.push_back(temp);
				}
				return answer;
				};

			std::vector<std::string> _dataLines = strSplit(_msg, '\n');

			for (auto& _line : _dataLines) {
				std::vector<std::string> _elements = strSplit(_line, ' ');
				assert(_elements.size() > 0);
				if (_elements[0] == "SRCDST") {
					assert(_elements.size() == 3);
					result.SrcHost = _elements[1];
					result.DstHost = _elements[2];
				}
				else if (_elements[0] == "MSG") {
					assert(_elements.size() > 1);
					for (size_t i = 1; i < _elements.size(); i++) {
						result.Message += _elements[i] + (i + 1 == _elements.size() ? "" : " ");
					}
				}
				else if (_elements[0] == "float") {
					assert(_elements.size() == 3);
					std::string name = _elements[1];
					float value = std::stof(_elements[2]);
					result.DataMap[name] = value;
				}
				else if (_elements[0] == "size") {
					assert(_elements.size() == 3);
					std::string name = _elements[1];
					size_t value = std::stoull(_elements[2]);
					result.DataMap[name] = value;
				}
				else if (_elements[0] == "int") {
					assert(_elements.size() == 3);
					std::string name = _elements[1];
					int value = std::stoi(_elements[2]);
					result.DataMap[name] = value;
				}
				else if (_elements[0] == "double") {
					assert(_elements.size() == 3);
					std::string name = _elements[1];
					double value = std::stod(_elements[2]);
					result.DataMap[name] = value;
				}
				else if (_elements[0] == "char") {
					assert(_elements.size() == 3);
					std::string name = _elements[1];
					assert(_elements[2].size() > 0);
					char value = _elements[2][0];
					result.DataMap[name] = value;
				}
				else if (_elements[0] == "string") {
					assert(_elements.size() == 3);
					std::string name = _elements[1];
					std::string value = _elements[2];
					result.DataMap[name] = value;
				}
			}
			return result;
		}
	}
}
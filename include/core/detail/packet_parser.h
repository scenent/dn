#ifndef DN_PACKET_PARSER_H
#define DN_PACKET_PARSER_H

#include "core/packet/packet.h"

#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <variant>

namespace dn {
	namespace detail {
		using DNPacketParsedDataType = std::variant<int, float, double, size_t, char, std::string>;

		struct DNPacketParsed {
			std::string SrcHost;
			std::string DstHost;
			std::string Message;
			std::map<std::string, DNPacketParsedDataType> DataMap;
		};

		std::ostream& operator<<(std::ostream& o, const DNPacketParsed& _p);

		class DNPacketParser {
		public:
			static DNPacketParsed parse(const DNPacket& __in _packet);
			static DNPacketParsed parse(const std::string& __in _msg);
		};
	}
}









#endif
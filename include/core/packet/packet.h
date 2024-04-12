#ifndef DN_PACKET_H
#define DN_PACKET_H

#include <enet/enet.h>

#include <cstdlib>
#include <cassert>

namespace dn {

	class DNPacket final {
	private:
		char* m_data = nullptr;
		size_t m_dataSize = 0;
	public:
		DNPacket() = delete;
		explicit DNPacket(const ENetPacket* const _src);
		~DNPacket();
		DNPacket(const DNPacket&);
		DNPacket(DNPacket&&);
		DNPacket& operator=(const DNPacket&) noexcept;
		DNPacket& operator=(DNPacket&&) noexcept;
	public:
		const char* const getData() const;
		const size_t getDataSize() const;
	};





}

#endif
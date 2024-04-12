#include "core/packet/packet.h"

namespace dn {
	DNPacket::DNPacket(const ENetPacket* const _src) {
		assert(m_data == nullptr);
		m_data = new char[_src->dataLength];
		strcpy_s(m_data, _src->dataLength, reinterpret_cast<const char*>(_src->data));
		m_dataSize = _src->dataLength;
	}

	DNPacket::~DNPacket() {
		assert(m_data != nullptr);
		delete[] m_data;
	}
	DNPacket::DNPacket(const DNPacket& _other) {
		this->m_dataSize = _other.m_dataSize;
		if (m_data != nullptr) {
			delete[] m_data;
		}
		m_data = new char[m_dataSize];
		strcpy_s(this->m_data, m_dataSize, _other.m_data);
	}
	
	DNPacket::DNPacket(DNPacket&& _other) {
		this->m_dataSize = _other.m_dataSize;
		if (m_data != nullptr) {
			delete[] m_data;
		}
		m_data = new char[m_dataSize];
		strcpy_s(this->m_data, m_dataSize, _other.m_data);
	}

	DNPacket& DNPacket::operator=(const DNPacket& _other) noexcept {
		this->m_dataSize = _other.m_dataSize;
		if (m_data != nullptr) {
			delete[] m_data;
		}
		m_data = new char[m_dataSize];
		strcpy_s(this->m_data, m_dataSize, _other.m_data);

		return *this;
	}

	DNPacket& DNPacket::operator=(DNPacket&& _other) noexcept {
		this->m_dataSize = _other.m_dataSize;
		if (m_data != nullptr) {
			delete[] m_data;
		}
		m_data = new char[m_dataSize];
		strcpy_s(this->m_data, m_dataSize, _other.m_data);

		return *this;
	}

	const char* const DNPacket::getData() const {
		return m_data;
	}

	const size_t DNPacket::getDataSize() const {
		return m_dataSize;
	}
}
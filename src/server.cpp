#include "core/server/server.h"
#include "core/packet/packet.h"

namespace dn {
	DNServerBase::~DNServerBase() {
		enet_host_destroy(m_host);
	}

	bool DNServerBase::init(const DNServerConfig& _config) {
		m_config = _config;

		m_address.host = ENET_HOST_ANY;
		m_address.port = m_config.PortNumber;

		m_host = enet_host_create(&m_address, m_config.MaxPeerCount, 1, 0, 0);

		if (m_host == nullptr) {
#if DN_DEBUG==1
			std::cerr << "DN : Failed To Create Server Host.\n";
#endif
			return false;
		}
#if DN_DEBUG==1
		std::cout << "DN : Server Host Created.\n";
		std::cout << "DN : Port Number = " << m_config.PortNumber << "\n";
#endif
		return true;
	}

	void DNServerBase::run() {
		m_isRunning.store(true);
		m_updateThread = std::thread(std::bind(&DNServerBase::onUpdateStarted, this));
		m_serverThread = std::thread(std::bind(&DNServerBase::serverThread, this));
	}

	void DNServerBase::quit() {
		m_isRunning.store(false);
		if (m_serverThread.joinable()) {
			m_serverThread.join();
		}
		if (m_updateThread.joinable()) {
			m_updateThread.join();
		}
	}

	bool DNServerBase::active() const {
		return m_isRunning.load();
	}

	void DNServerBase::sendMessage(const size_t& _id, const char* _content) {
		assert(_id > 0);
		std::cout << "SERVER : SEND MESSAGE : " << _id << "\n";
		ENetPacket* _packet = enet_packet_create(_content, strlen(_content) + 1, ENET_PACKET_FLAG_RELIABLE);
		if (_packet == nullptr) {
#if DN_DEBUG==1
			std::cerr << "DN : Failed To Create Packet.\n";
#endif
			return;
		}
		if (enet_peer_send(&m_host->peers[_id - 1], 0, _packet) != 0) {
#if DN_DEBUG==1
			std::cerr << "DN : Failed To Send Packet.\n";
#endif
		}
	}
	
	void DNServerBase::sendMessageAll(const char* _content) {
		for (size_t i = 0; i < m_peerIDCount - 1; i++) {
			ENetPacket* _packet = enet_packet_create(_content, strlen(_content) + 1, ENET_PACKET_FLAG_RELIABLE);
			if (enet_peer_send(&m_host->peers[i], 0, _packet) != 0) {
#if DN_DEBUG==1
				std::cerr << "DN : Failed To Send Packet : PeerID = " << i << "\n";
#endif
			}
		}
	}

	void DNServerBase::serverThread() {
		while (m_isRunning.load()) {
			std::lock_guard<std::mutex> lg(m_serverMutex);
			while (enet_host_service(m_host, &m_event, m_config.ServiceDelay) > 0) {
				switch (m_event.type) {
				case ENET_EVENT_TYPE_CONNECT:
					onClientConnected(m_peerIDCount++);
					break;
				case ENET_EVENT_TYPE_RECEIVE:
					onPacketReceived(DNPacket(m_event.packet));
					enet_host_flush(m_host);
					enet_packet_destroy(m_event.packet);
					break;
				case ENET_EVENT_TYPE_DISCONNECT:
					onClientDisconnected(--m_peerIDCount);
					//m_event.peer->data = nullptr;
					break;
				}
			}
		}
	}
}
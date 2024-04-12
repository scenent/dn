#include "core/client/client.h"
#include "core/packet/packet.h"

namespace dn {
	DNClientBase::~DNClientBase() {
		enet_peer_reset(m_peer);
		enet_host_destroy(m_host);
	}

	bool DNClientBase::init(const DNClientConfig& _config) {
		m_config = _config;

		m_host = enet_host_create(nullptr, 1, 1, 0, 0);

		if (m_host == nullptr) {
#if DN_DEBUG==1
			std::cerr << "DN : Failed To Create Client Host.\n";
#endif
			return false;
		}

		if (enet_address_set_host(&m_address, m_config.HostName) != 0) {
#if DN_DEBUG==1
			std::cerr << "DN : Failed To Set Host Name Of Address.\n";
#endif
			return false;
		}
		
		m_address.port = m_config.PortNumber;
		
		m_peer = enet_host_connect(m_host, &m_address, 1, 0);

		if (m_peer == nullptr) {
#if DN_DEBUG==1
			std::cout << "DN : Failed To Get Peer.\n";
#endif
			return false;
		}

#if DN_DEBUG==1
		std::cout << "DN : Client Created.\n";
		std::cout << "DN : Port Number = " << m_config.PortNumber << "\n";
#endif

		return true;
	}

	void DNClientBase::run() {
		m_isRunning = true;
		m_clientThread = std::thread(std::bind(&DNClientBase::clientThread, this));
		m_updateThread = std::thread(std::bind(&DNClientBase::onUpdateStarted, this));
	}

	void DNClientBase::quit() {
		m_isRunning = false;
		if (m_clientThread.joinable()) {
			m_clientThread.join();
		}
		if (m_updateThread.joinable()) {
			m_updateThread.join();
		}
	}

	bool DNClientBase::active() const {
		return m_isRunning;
	}

	void DNClientBase::sendMessage(const char* _content) {
		assert(m_isConnectedToServer == true);
		ENetPacket* _packet = enet_packet_create(_content, strlen(_content) + 1, ENET_PACKET_FLAG_RELIABLE);
		if (_packet == nullptr) {
#if DN_DEBUG==1
			std::cerr << "DN : Failed To Create Packet.\n";
#endif
			return;
		}
		if (enet_peer_send(m_peer, 0, _packet) != 0) {
#if DN_DEBUG==1
			std::cerr << "DN : Failed To Send Packet.\n";
#endif
		}
	}

	void DNClientBase::clientThread() {
		while (m_isRunning) {
			std::lock_guard<std::mutex> lg(m_clientMutex);
			while (enet_host_service(m_host, &m_event, m_config.ServiceDelay) > 0) {
				switch (m_event.type) {
				case ENET_EVENT_TYPE_CONNECT:
					m_isConnectedToServer = true;
					onServerConnected();
					break;
				case ENET_EVENT_TYPE_RECEIVE: {
					onPacketReceived(DNPacket(m_event.packet));
					enet_host_flush(m_host);
					enet_packet_destroy(m_event.packet);
					break;
				}
				case ENET_EVENT_TYPE_DISCONNECT:
					m_isConnectedToServer = false;
					onServerDisconnected();
					//m_event.peer->data = nullptr;
					break;
				}
			}
		}
	}
}
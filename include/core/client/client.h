#ifndef DN_CLIENT_H
#define DN_CLIENT_H

#include <enet/enet.h>

#include <iostream>
#include <thread>
#include <mutex>
#include <atomic>
#include <functional>

#include "config.h"


namespace dn {
	
	class DNPacket;

	struct DNClientConfig {
		const char* HostName = DN_DEFAULT_HOST_NAME;
		uint32_t    PortNumber = DN_DEFAULT_PORT_NUMBER;
		uint32_t    ServiceDelay = DN_DEFAULT_SERVICE_DELAY;
	};

	class DNClientBase {
	private:
		std::thread      m_clientThread;
		std::mutex       m_clientMutex;
		std::thread      m_updateThread;
	protected:
		std::mutex       m_updateMutex;
	protected:
		DNClientConfig   m_config{};
		ENetHost*        m_host = nullptr;
		ENetPeer*        m_peer = nullptr;
		ENetAddress      m_address{};
		ENetEvent        m_event{};
		std::atomic_bool m_isRunning = false;
		std::atomic_bool m_isConnectedToServer = false;
	public:
		DNClientBase() = default;
		DNClientBase(const DNClientBase&) = default;
		DNClientBase(DNClientBase&&) = default;
		DNClientBase& operator=(const DNClientBase&) = default;
		DNClientBase& operator=(DNClientBase&&) = default;
		virtual ~DNClientBase();
	public:
		bool init(const DNClientConfig& _config = {});
		void run();
		void quit();
		bool active() const;
	public:
		void sendMessage(const char* _content);
	public:
		virtual void onPacketReceived(const DNPacket _packet) = 0;
		virtual void onServerConnected() = 0;
		virtual void onServerDisconnected() = 0;
		virtual void onUpdateStarted() = 0;
	private:
		void clientThread();
	};

}

#endif
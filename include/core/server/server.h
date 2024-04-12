#ifndef DN_SERVER_H
#define DN_SERVER_H

#include <enet/enet.h>

#include <iostream>
#include <thread>
#include <mutex>
#include <atomic>
#include <functional>

#include "config.h"


namespace dn {

	class DNPacket;
	
	struct DNServerConfig {
		uint32_t PortNumber   = DN_DEFAULT_PORT_NUMBER;
		uint32_t MaxPeerCount = DN_DEFAULT_MAX_PEER_COUNT;
		uint32_t ServiceDelay = DN_DEFAULT_SERVICE_DELAY;
	};

	class DNServerBase {
	private:
		std::thread      m_serverThread;
		std::mutex       m_serverMutex;
		std::thread      m_updateThread;
	protected:
		std::mutex       m_updateMutex;
	protected:
		DNServerConfig   m_config {};
		ENetAddress      m_address {};
		ENetHost*        m_host = nullptr;
		ENetEvent        m_event {};
		std::atomic_bool m_isRunning = false;
		size_t           m_peerIDCount = 1;
	public:
		DNServerBase() = default;
		DNServerBase(const DNServerBase&) = default;
		DNServerBase(DNServerBase&&) = default;
		DNServerBase& operator=(const DNServerBase&) = default;
		DNServerBase& operator=(DNServerBase&&) = default;
		virtual ~DNServerBase();
	public:
		bool init(const DNServerConfig& _config = {});
		void run();
		void quit();
		bool active() const;
	public:
		void sendMessage(const size_t& _id, const char* _content);
		void sendMessageAll(const char* _content);
	public:
		virtual void onPacketReceived(const DNPacket _packet) = 0;
		virtual void onClientConnected(const size_t& _id) = 0;
		virtual void onClientDisconnected(const size_t& _id) = 0;
		virtual void onUpdateStarted() = 0;
	private:
		void serverThread();
	};


}
#endif
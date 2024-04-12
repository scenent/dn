#include "dn.h"

#undef min
#undef max
#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

struct Vector2 {
	float x, y;
	template <typename T>
	Vector2(const T& _x, const T& _y) : x(_x), y(_y) {

	}
};

class MyGameObject {
public:
	uint32_t ID;
	Vector2 position;
	float radius;
	MyGameObject(const uint32_t& _id, const Vector2& _pos, const float& _radius)
		: ID(_id), position(_pos), radius(_radius)
	{

	}
};


class MyServer : public dn::DNServerBase {
private:
	friend class MyGame;
	std::vector<MyGameObject*> m_gameObjects{};
	MyGameObject* m_thisObject = nullptr;
	const size_t m_ID = 0;
public:
	MyServer() {
		m_thisObject = new MyGameObject(m_ID, { 200, 200 }, 30.0);
		m_gameObjects.push_back(m_thisObject);
	}
	const std::vector<MyGameObject*>& getGameObjects() const {
		return m_gameObjects;
	}
	void onPacketReceived(const dn::DNPacket _packet) override {
		std::cout << "Got A Message From Client : \n";
		dn::detail::DNPacketParsed data = dn::detail::DNPacketParser::parse(_packet);
		std::cout << data << "\n";
		if (data.Message == "SetPosX"){
			size_t clientID = std::get<size_t>(data.DataMap["ID"]);
			m_gameObjects[clientID]->position.x = std::get<float>(data.DataMap["NewPosX"]);
		}
		else if (data.Message == "SetPosY") {
			size_t clientID = std::get<size_t>(data.DataMap["ID"]);
			m_gameObjects[clientID]->position.y = std::get<float>(data.DataMap["NewPosY"]);
		}
	}

	void onClientConnected(const size_t& _id) override {
		std::cout << "New Client Connected : ID = " << _id << "\n";
		m_gameObjects.push_back(new MyGameObject(_id, {200, 200}, 30.0f));
		std::string tempContent =
			"SRCDST SERVER CLIENT" + std::to_string(_id) + "\n" +
			"MSG SendID\n"
			"size NewID " + std::to_string(_id);
		sendMessage(_id, tempContent.c_str());

		tempContent =
			"SRCDST SERVER ALLCLIENT\n"
			"MSG SetServerPos\n"
			"float NewPosX " + std::to_string(m_thisObject->position.x) + "\n" + 
			"float NewPosY " + std::to_string(m_thisObject->position.y);
		sendMessageAll(tempContent.c_str());
	}

	void onClientDisconnected(const size_t& _id) override {
		std::cout << "Client Disconnected : ID = " << _id << "\n";
		for (size_t i = 0; i < m_gameObjects.size(); i++) {
			if (m_gameObjects[i]->ID == _id) {
				delete *(m_gameObjects.begin() + i);
				m_gameObjects.erase(m_gameObjects.begin() + i);
				break;
			}
		}
	}
	void onUpdateStarted() override {

	}
};

class MyClient : public dn::DNClientBase {
private:
	friend class MyGame;
	std::vector<MyGameObject*> m_gameObjects{};
	MyGameObject* m_thisObject = nullptr;
	size_t m_ID = -1;
public:
	const std::vector<MyGameObject*>& getGameObjects()const {
		return m_gameObjects;
	}
public:
	MyClient() {
		MyGameObject* serverObject = new MyGameObject(0, { 200, 200 }, 30.0);
		m_thisObject = new MyGameObject(m_ID, {200, 200}, 30.0);
		m_gameObjects.push_back(serverObject);
		m_gameObjects.push_back(m_thisObject);
	}
	~MyClient() {
		for (auto& o : m_gameObjects) {
			delete o;
			o = nullptr;
		}
	}
	void onPacketReceived(const dn::DNPacket _packet) override {
		std::cout << "Got A Message From Server : \n";
		dn::detail::DNPacketParsed data = dn::detail::DNPacketParser::parse(_packet);
		std::cout << data << "\n";
		if (data.Message == "SendID") {
			m_ID = std::get<size_t>(data.DataMap["NewID"]);
		}
		else if (data.Message == "SetServerPosX") {
			static constexpr const size_t serverID = 0;
			m_gameObjects[serverID]->position.x = std::get<float>(data.DataMap["NewPosX"]);
		}
		else if (data.Message == "SetServerPosY") {
			static constexpr const size_t serverID = 0;
			m_gameObjects[serverID]->position.y = std::get<float>(data.DataMap["NewPosY"]);
		}
		else if (data.Message == "SetServerPos") {
			static constexpr const size_t serverID = 0;
			m_gameObjects[serverID]->position.x = std::get<float>(data.DataMap["NewPosX"]);
			m_gameObjects[serverID]->position.y = std::get<float>(data.DataMap["NewPosY"]);
		}
	}

	void onServerConnected() override {
		std::cout << "Connected To Server\n";

	}

	void onServerDisconnected() override {
		std::cout << "Disconnected From Server\n";
	}

	void onUpdateStarted() override {

	}
};

enum class MyGameHostType {
	Server, Client
};

class MyGame final : public olc::PixelGameEngine {
private:
	dn::DNContext* m_context = nullptr;
	MyServer* m_server = nullptr;
	MyClient* m_client = nullptr;
	MyGameHostType m_hostType = MyGameHostType::Server;
public:
	MyGame() {
		sAppName = "MyGame";
	}
public:
	void setHostType(const MyGameHostType& _value) {
		m_hostType = _value;
	}

	bool OnUserCreate() override {
		m_context = new dn::DNContext();
		m_context->init();

		if (m_hostType == MyGameHostType::Server) {
			m_server = new MyServer();
			if (!m_server->init()) {
				return false;
			}
			m_server->run();
		}

		else if (m_hostType == MyGameHostType::Client) {
			m_client = new MyClient();
			if (!m_client->init()) {
				return false;
			}
			m_client->run();
		}

		return true;
	}
	bool OnUserDestroy() override {
		if (m_server != nullptr) {
			m_server->quit();
			delete m_server;
		}
		if (m_client != nullptr) {
			m_client->quit();
			delete m_client;
		}
		m_context->terminate();
		delete m_context;
		return true;
	}
	bool OnUserUpdate(float fElapsedTime) override {
		Clear(olc::Pixel(0, 0, 0));
		if (GetKey(olc::Key::D).bHeld) {
			if (m_hostType == MyGameHostType::Server) {
				m_server->m_thisObject->position.x += 1;
				std::string tempContent =
					"SRCDST SERVER ALLCLIENT\n"
					"MSG SetServerPosX\n"
					"float NewPosX " + std::to_string(m_server->m_thisObject->position.x);
				m_server->sendMessageAll(tempContent.c_str());
			}
			else {
				m_client->m_thisObject->position.x += 1;
				std::string tempContent =
					"SRCDST CLIENT" + std::to_string(m_client->m_ID) + " SERVER\n"
					"MSG SetPosX\n"
					"float NewPosX " + std::to_string(m_client->m_thisObject->position.x) + "\n"
					"size ID " + std::to_string(m_client->m_ID);
				m_client->sendMessage(tempContent.c_str());
			}
		}
		if (GetKey(olc::Key::A).bHeld) {
			if (m_hostType == MyGameHostType::Server) {
				m_server->m_thisObject->position.x -= 1;
				std::string tempContent =
					"SRCDST SERVER ALLCLIENT\n"
					"MSG SetServerPosX\n"
					"float NewPosX " + std::to_string(m_server->m_thisObject->position.x);
				m_server->sendMessageAll(tempContent.c_str());
			}
			else {
				m_client->m_thisObject->position.x -= 1;
				std::string tempContent =
					"SRCDST CLIENT" + std::to_string(m_client->m_ID) + " SERVER\n"
					"MSG SetPosX\n"
					"float NewPosX " + std::to_string(m_client->m_thisObject->position.x) + "\n"
					"size ID " + std::to_string(m_client->m_ID);
				m_client->sendMessage(tempContent.c_str());
			}
		}
		if (GetKey(olc::Key::W).bHeld) {
			if (m_hostType == MyGameHostType::Server) {
				m_server->m_thisObject->position.y -= 1;
				std::string tempContent =
					"SRCDST SERVER ALLCLIENT\n"
					"MSG SetServerPosY\n"
					"float NewPosY " + std::to_string(m_server->m_thisObject->position.y);
				m_server->sendMessageAll(tempContent.c_str());
			}
			else {
				m_client->m_thisObject->position.y -= 1;
				std::string tempContent =
					"SRCDST CLIENT" + std::to_string(m_client->m_ID) + " SERVER\n"
					"MSG SetPosY\n"
					"float NewPosY " + std::to_string(m_client->m_thisObject->position.y) + "\n"
					"size ID " + std::to_string(m_client->m_ID);
				m_client->sendMessage(tempContent.c_str());
			}
		}
		if (GetKey(olc::Key::S).bHeld) {
			if (m_hostType == MyGameHostType::Server) {
				m_server->m_thisObject->position.y += 1;
				std::string tempContent =
					"SRCDST SERVER ALLCLIENT\n"
					"MSG SetServerPosY\n"
					"float NewPosY " + std::to_string(m_server->m_thisObject->position.y);
				m_server->sendMessageAll(tempContent.c_str());
			}
			else {
				m_client->m_thisObject->position.y += 1;
				std::string tempContent =
					"SRCDST CLIENT" + std::to_string(m_client->m_ID) + " SERVER\n"
					"MSG SetPosY\n"
					"float NewPosY " + std::to_string(m_client->m_thisObject->position.y) + "\n"
					"size ID " + std::to_string(m_client->m_ID);
				m_client->sendMessage(tempContent.c_str());
			}
		}
		if (m_hostType == MyGameHostType::Server) {
			for (const auto& obj : m_server->getGameObjects()) {
				DrawCircle(olc::vi2d(obj->position.x, obj->position.y), obj->radius);
			}
		}
		else {
			for (const auto& obj : m_client->getGameObjects()) {
				DrawCircle(olc::vi2d(obj->position.x, obj->position.y), obj->radius);
			}
		}
		return true;
	}
};


int main() {
	MyGame mg;
	mg.setHostType(MyGameHostType::Server);
	if (mg.Construct(400, 400, 1, 1)) {
		mg.Start();
	}
}
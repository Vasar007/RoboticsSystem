#include "TestServer.h"
#include "ModClient.h"
#include "ClientTest.h"


namespace clientTests
{

/**              
 * \brief                   Initialize server and launches it in detached thread.
 * \param[out] testServer   Thread to launch.
 * \param[in] numberOfTimes Number of times to allow connections.
 */
void initTestServer(TestServer& testServer, const int numberOfTimes = 1)
{
	testServer.launch();
	testServer.receiveDataNTimes(numberOfTimes);
}

/**
 * \brief                   Initialize client, sends data to server and launch receiving
 *                          thread for client.
 * \tparam Functor          Type of functor object.
 * \param[out] client       Client to launch.
 * \param[in] funcToCall    Functor object to call, it sends data to server.
 * \param[in] numberOfTimes Number of times to allow connections.
 */
template <typename Functor>
void initClient(ModClient& client, Functor&& funcToCall, const int numberOfTimes = 1)
{
	client.launch();

	std::thread reciveThread = client.spawn(numberOfTimes);
	reciveThread.detach();

	funcToCall();
}


void ClientTest::sendReceiveAbility()
{
	TestServer testServer;

	constexpr long long kStandartDelay      = 25LL;
	constexpr int kReceiveCoordinateType    = 1;
	constexpr int kReceiveCoordinates       = 1;

	constexpr int kNumberOfTimesToReceiveData = kReceiveCoordinateType + kReceiveCoordinates;

	std::thread serverThread(initTestServer, std::ref(testServer), kNumberOfTimesToReceiveData);
	serverThread.detach();
	
	ModClient client(ModClient::SERVER_PORT_SENDING, ModClient::SERVER_PORT_RECEIVING,
					 ModClient::SERVER_IP, vasily::Client::WorkMode::STRAIGHTFORWARD);
	initClient(client, [&client, &kStandartDelay]()
	{	
		client.sendCoordinateSystem(vasily::CoordinateSystem::WORLD);
		std::this_thread::sleep_for(std::chrono::milliseconds(kStandartDelay));

		client.sendCoordinatesMod({ 985'000, 0, 940'000, -180'000, 0, 0, 10, 2, 0 });
	}, kReceiveCoordinates);


	do
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(kStandartDelay));
	}
	while (!testServer.hasFinished.load() && !client.hasFinished.load());

	Assert::IsTrue(testServer.hasConnected, L"Bad connection!");

	Assert::IsFalse(testServer.storage.empty(), L"There are not sent coordinates on server!");
	Assert::IsFalse(client.storage.empty(), L"There are not sent coordinates on client!");
	
	Assert::AreEqual(client.storage.at(0u), testServer.storage.at(0u), L"Incorrect sent data!");
}

void ClientTest::dangerusZone()
{
	TestServer testServer;

	constexpr long long kStandartDelay      = 25LL;
	constexpr int kReceiveCoordinateType    = 1;
	constexpr int kReceiveCoordinates       = 0;

	constexpr int kNumberOfTimesToReceiveData = kReceiveCoordinateType + kReceiveCoordinates;

	std::thread serverThread(initTestServer, std::ref(testServer), kNumberOfTimesToReceiveData);
	serverThread.detach();

	ModClient client(ModClient::SERVER_PORT_SENDING, ModClient::SERVER_PORT_RECEIVING,
					 ModClient::SERVER_IP, vasily::Client::WorkMode::STRAIGHTFORWARD);
	initClient(client, [&client, &kStandartDelay]()
	{
		client.sendCoordinateSystem(vasily::CoordinateSystem::WORLD);
		std::this_thread::sleep_for(std::chrono::milliseconds(kStandartDelay));

		client.sendCoordinatesMod({ 829'000,	0,			940'000, -180'000, 0, 0, 10, 2, 0 });
		client.sendCoordinatesMod({ 985'000,   -401'000,	940'000, -180'000, 0, 0, 10, 2, 0 });
		client.sendCoordinatesMod({ 985'000,	0,			538'000, -180'000, 0, 0, 10, 2, 0 });
		client.sendCoordinatesMod({ 1'321'000,  0,			940'000, -180'000, 0, 0, 10, 2, 0 });
		client.sendCoordinatesMod({ 985'000,	401'000,	940'000, -180'000, 0, 0, 10, 2, 0 });
		client.sendCoordinatesMod({ 985'000,	0,			961'000, -180'000, 0, 0, 10, 2, 0 });

		client.sendCoordinatesMod({ 829'999,	0,			940'000, -180'000, 0, 0, 10, 2, 0 });
		client.sendCoordinatesMod({ 985'000,   -400'001,	940'000, -180'000, 0, 0, 10, 2, 0 });
		client.sendCoordinatesMod({ 985'000,	0,			538'999, -180'000, 0, 0, 10, 2, 0 });
		client.sendCoordinatesMod({ 1'320'001,  0,			940'000, -180'000, 0, 0, 10, 2, 0 });
		client.sendCoordinatesMod({ 985'000,	400'001,	940'000, -180'000, 0, 0, 10, 2, 0 });
		client.sendCoordinatesMod({ 985'000,	0,			960'001, -180'000, 0, 0, 10, 2, 0 });
	}, kReceiveCoordinates);


	do
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(kStandartDelay));
	}
	while (!testServer.hasFinished.load() && !client.hasFinished.load());

	Assert::IsTrue(testServer.hasConnected, L"Bad connection!");

	Assert::IsTrue(testServer.storage.empty(), L"There are not accepted coordinates on server!");
	Assert::IsTrue(client.storage.empty(), L"There are not accepted coordinates on client!");
}

void ClientTest::circlicMovement()
{
	TestServer testServer;
	
	constexpr long long kStandartDelay      = 25LL;
	constexpr int kReceiveCoordinateType    = 1;
	constexpr int kReceiveCordinatesFirst   = 5;
	constexpr int kReceiveCordinatesSecond  = 1;

	constexpr int kReceiveCoordinates = kReceiveCordinatesFirst + kReceiveCordinatesSecond;
	constexpr int kNumberOfTimesToReceiveData = kReceiveCoordinateType + kReceiveCoordinates;

	std::thread serverThread(initTestServer, std::ref(testServer), kNumberOfTimesToReceiveData);
	serverThread.detach();
	
	ModClient client(ModClient::SERVER_PORT_SENDING, ModClient::SERVER_PORT_RECEIVING,
					 ModClient::SERVER_IP, vasily::Client::WorkMode::STRAIGHTFORWARD);
	initClient(client, [&client, &kStandartDelay]()
	{
		client.sendCoordinateSystem(vasily::CoordinateSystem::WORLD);
		std::this_thread::sleep_for(std::chrono::milliseconds(kStandartDelay));
	
		constexpr int kNumberOfTimes = 2;
		client.circlicMovementMod({ 985'000, 0,			940'000, -180'000, 0,	0, 10, 2, 0 },
								  { 900'000, 100'000,	800'000, -150'000, 10,	0, 10, 2, 0 }, 
								  kNumberOfTimes);

		client.circlicMovementMod({ 985'000, 0,			940'000, -180'000, 0,	0, 10, 2, 0 },
								  { 800'000, 100'000,	800'000, -150'000, 10,	0, 10, 2, 0 },
								  kNumberOfTimes);
	}, kReceiveCoordinates);
	
	
	do
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(kStandartDelay));
	}
	while (!testServer.hasFinished.load() && !client.hasFinished.load());
	
	Assert::IsTrue(testServer.hasConnected, L"Bad connection!");
	
	Assert::IsFalse(testServer.storage.empty(), L"There are not sent coordinates on server!");
	Assert::IsFalse(client.storage.empty(), L"There are not sent coordinates on client!");
	
	std::wstring message = L"Incorrect data at 0!";
	constexpr std::size_t kNumberPlace = 18u;
	for (std::size_t i = 0u; i < client.storage.size(); ++i)
	{
		message.replace(kNumberPlace, 1u, std::to_wstring(i + 1u));
		Assert::AreEqual(client.storage.at(i), testServer.storage.at(i), message.c_str());
	}
}

void ClientTest::partialMovement()
{
	TestServer testServer;

	constexpr long long kStandartDelay      = 25LL;
	constexpr int kReceiveCoordinateType   = 1;
	constexpr int kReceiveCordinatesFirst  = 6;
	constexpr int kReceiveCordinatesSecond = 5;

	constexpr int kReceiveCoordinates = kReceiveCordinatesFirst + kReceiveCordinatesSecond;
	constexpr int kNumberOfTimesToReceiveData = kReceiveCoordinateType + kReceiveCoordinates;

	std::thread serverThread(initTestServer, std::ref(testServer), kNumberOfTimesToReceiveData);
	serverThread.detach();

	ModClient client(ModClient::SERVER_PORT_SENDING, ModClient::SERVER_PORT_RECEIVING,
					 ModClient::SERVER_IP, vasily::Client::WorkMode::STRAIGHTFORWARD);
	initClient(client, [&client, &kStandartDelay]()
	{
		client.sendCoordinateSystem(vasily::CoordinateSystem::WORLD);
		std::this_thread::sleep_for(std::chrono::milliseconds(kStandartDelay));

		constexpr int kNumberOfSteps = 5;
		client.partialMovementMod({ 985'000, 0,			940'000, -180'000, 0,	0, 10, 2, 0 },
								  { 900'000, 100'000,	800'000, -150'000, 10,	0, 10, 2, 0 },
								  kNumberOfSteps);

		client.partialMovementMod({ 985'000, 0,			940'000, -180'000, 0,	0, 10, 2, 0 },
								  { 800'000, 100'000,	800'000, -150'000, 10,	0, 10, 2, 0 },
								  kNumberOfSteps);
	}, kReceiveCoordinates);


	do
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(kStandartDelay));
	}
	while (!testServer.hasFinished.load() && !client.hasFinished.load());

	Assert::IsTrue(testServer.hasConnected, L"Bad connection!");

	Assert::IsFalse(testServer.storage.empty(), L"There are not sent coordinates on server!");
	Assert::IsFalse(client.storage.empty(), L"There are not sent coordinates on client!");

	std::wstring message = L"Incorrect data at 0!";
	constexpr std::size_t kNumberPlace = 18u;
	for (std::size_t i = 0u; i < client.storage.size(); ++i)
	{
		message.replace(kNumberPlace, 1u, std::to_wstring(i + 1u));
		Assert::AreEqual(client.storage.at(i), testServer.storage.at(i), message.c_str());
	}
}

} // namespace clientTests

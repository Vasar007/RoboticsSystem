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
void initTestServer(TestServer& testServer, const std::size_t numberOfTimes = 1u)
{
	testServer.init();
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
void initClient(ModClient& client, Functor&& funcToCall, const std::size_t numberOfTimes = 1u)
{
	client.init();
	client.launch();

	std::thread reciveThread = client.spawn(numberOfTimes);
	reciveThread.detach();

	funcToCall();
}

void ClientTest::testMethod1()
{
	TestServer testServer;

	std::thread serverThread(initTestServer, std::ref(testServer), 1u + 1u);
	serverThread.detach();
	
	ModClient client;
	initClient(client, [&client]()
	{	
		client.sendCoordinateType(ModClient::CoordinateSystem::WORLD);
		std::this_thread::sleep_for(std::chrono::milliseconds(25LL));

		client.sendCoordinatesMod({ 985'000, 0, 940'000, -180'000, 0, 0, 10, 2, 0 });
	}, 0u);


	std::this_thread::sleep_for(std::chrono::milliseconds(100LL));

	while (!testServer.mHasFinished && !client.mHasFinished)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(100LL));
	}

	Assert::IsTrue(testServer.mHasConnected, L"Bad connection!");

	Assert::IsFalse(testServer.mStorage.empty(), L"There are not sent coordinates on server!");
	Assert::IsFalse(client.mStorage.empty(), L"There are not sent coordinates on client!");
	
	Assert::AreEqual(client.mStorage.at(0u), testServer.mStorage.at(0u), L"Incorrect sent data!");
}

void ClientTest::testMethod2()
{
	TestServer testServer;

	std::thread serverThread(initTestServer, std::ref(testServer), 1u);
	serverThread.detach();

	ModClient client;
	initClient(client, [&client]()
	{
		client.sendCoordinateType(ModClient::CoordinateSystem::WORLD);
		std::this_thread::sleep_for(std::chrono::milliseconds(25LL));

		client.sendCoordinatesMod({ 829'000,	0,			940'000, -180'000, 0, 0, 10, 2, 0 });
		client.sendCoordinatesMod({ 985'000,   -401'000,	940'000, -180'000, 0, 0, 10, 2, 0 });
		client.sendCoordinatesMod({ 985'000,	0,			538'000, -180'000, 0, 0, 10, 2, 0 });
		client.sendCoordinatesMod({ 1'321'000,  0,			940'000, -180'000, 0, 0, 10, 2, 0 });
		client.sendCoordinatesMod({ 985'000,	318'000,	940'000, -180'000, 0, 0, 10, 2, 0 });
		client.sendCoordinatesMod({ 985'000,	0,			961'000, -180'000, 0, 0, 10, 2, 0 });
		client.sendCoordinatesMod({ 829'999,	0,			940'000, -180'000, 0, 0, 10, 2, 0 });
		client.sendCoordinatesMod({ 1'320'001,  0,			940'000, -180'000, 0, 0, 10, 2, 0 });
		client.sendCoordinatesMod({ 830'000,   -400'001,	538'999, -180'000, 0, 0, 10, 2, 0 });
	}, 0u);


	std::this_thread::sleep_for(std::chrono::milliseconds(100LL));

	while (!testServer.mHasFinished && !client.mHasFinished)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(100LL));
	}

	Assert::IsTrue(testServer.mHasConnected, L"Bad connection!");

	Assert::IsTrue(testServer.mStorage.empty(), L"There are not accepted coordinates on server!");
	Assert::IsTrue(client.mStorage.empty(), L"There are not accepted coordinates on client!");
}

void ClientTest::testMethod3()
{
	TestServer testServer;
	
	std::thread serverThread(initTestServer, std::ref(testServer), 1u + 5u + 1u);
	serverThread.detach();
	
	ModClient client;
	initClient(client, [&client]()
	{
		client.sendCoordinateType(ModClient::CoordinateSystem::WORLD);
		std::this_thread::sleep_for(std::chrono::milliseconds(25LL));
	
		constexpr std::size_t numberOfTimes = 2u;
		client.circlicMovementMod({ 985'000, 0,			940'000, -180'000, 0,	0, 10, 2, 0 },
								  { 900'000, 100'000,	800'000, -150'000, 10,	0, 10, 2, 0 }, 
								  numberOfTimes);

		client.circlicMovementMod({ 985'000, 0,			940'000, -180'000, 0,	0, 10, 2, 0 },
								  { 800'000, 100'000,	800'000, -150'000, 10,	0, 10, 2, 0 },
								  numberOfTimes);
	}, 5u + 1u);
	
	
	std::this_thread::sleep_for(std::chrono::milliseconds(100LL));

	while (!testServer.mHasFinished && !client.mHasFinished)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(100LL));
	}
	
	Assert::IsTrue(testServer.mHasConnected, L"Bad connection!");
	
	Assert::IsFalse(testServer.mStorage.empty(), L"There are not sent coordinates on server!");
	Assert::IsFalse(client.mStorage.empty(), L"There are not sent coordinates on client!");
	
	std::wstring message = L"Incorrect data at 0!";
	for (std::size_t i = 0u; i < client.mStorage.size(); ++i)
	{
		message.replace(18u, 1u, std::to_wstring(i + 1u));
		Assert::AreEqual(client.mStorage.at(i), testServer.mStorage.at(i), message.c_str());
	}
}

void ClientTest::testMethod4()
{
	TestServer testServer;

	std::thread serverThread(initTestServer, std::ref(testServer), 1u + 6u + 5u);
	serverThread.detach();

	ModClient client;
	initClient(client, [&client]()
	{
		client.sendCoordinateType(ModClient::CoordinateSystem::WORLD);
		std::this_thread::sleep_for(std::chrono::milliseconds(25LL));

		constexpr std::size_t numberOfSteps = 5u;
		client.partialMovementMod({ 985'000, 0,			940'000, -180'000, 0,	0, 10, 2, 0 },
								  { 900'000, 100'000,	800'000, -150'000, 10,	0, 10, 2, 0 },
								  numberOfSteps);

		client.partialMovementMod({ 985'000, 0,			940'000, -180'000, 0,	0, 10, 2, 0 },
								  { 800'000, 100'000,	800'000, -150'000, 10,	0, 10, 2, 0 },
								  numberOfSteps);
	}, 6u + 5u);


	std::this_thread::sleep_for(std::chrono::milliseconds(100LL));

	while (!testServer.mHasFinished && !client.mHasFinished)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(100LL));
	}

	Assert::IsTrue(testServer.mHasConnected, L"Bad connection!");

	Assert::IsFalse(testServer.mStorage.empty(), L"There are not sent coordinates on server!");
	Assert::IsFalse(client.mStorage.empty(), L"There are not sent coordinates on client!");

	std::wstring message = L"Incorrect data at 0!";
	for (std::size_t i = 0u; i < client.mStorage.size(); ++i)
	{
		message.replace(18u, 1u, std::to_wstring(i + 1u));
		Assert::AreEqual(client.mStorage.at(i), testServer.mStorage.at(i), message.c_str());
	}
}

}

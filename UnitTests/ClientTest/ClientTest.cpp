#include "TestServer.h"
#include "Client.h"
#include "ClientTest.h"


namespace vasily
{

void receiveClient(Client& client, const std::size_t numberOfTimes)
{
	for (std::size_t step = 0u; step < numberOfTimes; ++step)
	{
		client.receiveData(client._socketReceive);

		if (client._isNeedToWait)
		{
			client._isNeedToWait = false;
			switch (client._circlicState)
			{
				case Client::CirclicState::SEND_FIRST:
					break;

				case Client::CirclicState::WAIT_FIRST_ANSWER:
					client._circlicState = Client::CirclicState::SEND_SECOND;
					break;

				case Client::CirclicState::SEND_SECOND:
					break;

				case Client::CirclicState::WAIT_SECOND_ANSWER:
					client._circlicState = Client::CirclicState::SEND_FIRST;
					break;

				default:
					break;;
			}
		}
	}
}

}

namespace clientTests
{

void initTestServer(TestServer& testServer, const std::size_t numberOfTimes = 1u)
{
	testServer.init();
	testServer.launch();

	testServer.receiveDataNTimes(numberOfTimes);
}

void init(vasily::Client& client, const std::function<void()> funcToCall, 
		  const std::size_t numberOfTimes = 1u)
{
	client.init();
	client.launch();

	std::thread reciveThread(vasily::receiveClient, std::ref(client), numberOfTimes);
	reciveThread.detach();

	funcToCall();
}

void ClientTest::testMethod1()
{
	TestServer testServer;

	std::thread serverThread(initTestServer, std::ref(testServer), 2u);
	serverThread.detach();
	
	vasily::Client client{ TestServer::SERVER_PORT_SENDING, TestServer::SERVER_PORT_RECEIVING,
						   TestServer::SERVER_IP };
	init(client, [&client]()
	{	
		client.sendCoordinateType(vasily::Client::CoordinateType::WORLD);
		constexpr std::atomic_int64_t waitingTime = 100LL;
		std::this_thread::sleep_for(std::chrono::milliseconds(waitingTime));

		client.sendCoordinates({ 985'000, 0, 940'000, -180'000, 0, 0, 10, 2, 0 });
	}, 0u);


	constexpr std::atomic_int64_t waitingTime = 1000LL;
	std::this_thread::sleep_for(std::chrono::milliseconds(waitingTime));

	while (!testServer.mHasFinished)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(waitingTime));
	}

	Assert::IsTrue(testServer.mHasConnected, L"Bad connection!");

	Assert::IsFalse(testServer.mStorage.empty(), L"There are not sent coordinates!");
	
	std::vector<std::string> expectAnswer = { "985000 0 940000 -180000 0 0 10 2 0 " };
	Assert::AreEqual(expectAnswer.at(0u), testServer.mStorage.at(0u), L"Incorrect data sending!");

	std::this_thread::sleep_for(std::chrono::milliseconds(waitingTime));
}

void ClientTest::testMethod2()
{
	TestServer testServer;

	std::thread serverThread(initTestServer, std::ref(testServer), 1u);
	serverThread.detach();

	vasily::Client client{ TestServer::SERVER_PORT_SENDING, TestServer::SERVER_PORT_RECEIVING,
						   TestServer::SERVER_IP };
	init(client, [&client]()
	{
		client.sendCoordinateType(vasily::Client::CoordinateType::WORLD);
		constexpr std::atomic_int64_t waitingTime = 100LL;
		std::this_thread::sleep_for(std::chrono::milliseconds(waitingTime));

		client.sendCoordinates({ 829'000,	0,			940'000, -180'000, 0, 0, 10, 2, 0 });
		client.sendCoordinates({ 985'000,	-401'000,	940'000, -180'000, 0, 0, 10, 2, 0 });
		client.sendCoordinates({ 985'000,	0,			538'000, -180'000, 0, 0, 10, 2, 0 });
		client.sendCoordinates({ 1'321'000, 0,			940'000, -180'000, 0, 0, 10, 2, 0 });
		client.sendCoordinates({ 985'000,	318'000,	940'000, -180'000, 0, 0, 10, 2, 0 });
		client.sendCoordinates({ 985'000,	0,			961'000, -180'000, 0, 0, 10, 2, 0 });
		client.sendCoordinates({ 829'999,	0,			940'000, -180'000, 0, 0, 10, 2, 0 });
		client.sendCoordinates({ 1'320'001, 0,			940'000, -180'000, 0, 0, 10, 2, 0 });
		client.sendCoordinates({ 830'000,	-400'001,	538'999, -180'000, 0, 0, 10, 2, 0 });
	}, 0u);


	constexpr std::atomic_int64_t waitingTime = 1000LL;
	std::this_thread::sleep_for(std::chrono::milliseconds(waitingTime));

	while (!testServer.mHasFinished)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(waitingTime));
	}

	Assert::IsTrue(testServer.mHasConnected, L"Bad connection!");

	Assert::IsTrue(testServer.mStorage.empty(), L"There are not accepted coordinates!");

	std::this_thread::sleep_for(std::chrono::milliseconds(waitingTime));
}

void ClientTest::testMethod3()
{
	TestServer testServer;
	
	std::thread serverThread(initTestServer, std::ref(testServer), 6u);
	serverThread.detach();
	
	vasily::Client client{ TestServer::SERVER_PORT_SENDING, TestServer::SERVER_PORT_RECEIVING,
		TestServer::SERVER_IP };
	init(client, [&client]()
	{
		client.sendCoordinateType(vasily::Client::CoordinateType::WORLD);
		constexpr std::atomic_int64_t waitingTime = 100LL;
		std::this_thread::sleep_for(std::chrono::milliseconds(waitingTime));
	
		constexpr std::atomic_uint NUMBER_OF_TIMES = 2u;
		client.circlicMovement({ 985'000, 0,		940'000, -180'000, 0,	0, 10, 2, 0 },
							   { 900'000, 100'000,	800'000, -150'000, 10,	0, 10, 2, 0 }, 
							   NUMBER_OF_TIMES);
	}, 5u);
	
	
	constexpr std::atomic_int64_t waitingTime = 1000LL;
	std::this_thread::sleep_for(std::chrono::milliseconds(waitingTime));

	while (!testServer.mHasFinished)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(waitingTime));
	}
	
	Assert::IsTrue(testServer.mHasConnected, L"Bad connection!");
	
	Assert::IsFalse(testServer.mStorage.empty(), L"There are not sent coordinates!");
	
	std::vector<std::string> expectAnswers = { "985000 0 940000 -180000 0 0 10 2 0 ",
											   "900000 100000 800000 -150000 10 0 10 2 0 ",
											   "985000 0 940000 -180000 0 0 10 2 0 ",
											   "900000 100000 800000 -150000 10 0 10 2 0 ",
											   "985000 0 940000 -180000 0 0 10 2 0 " };
	Assert::AreEqual(expectAnswers.at(0u), testServer.mStorage.at(0u), L"Incorrect data at 1st!");
	Assert::AreEqual(expectAnswers.at(1u), testServer.mStorage.at(1u), L"Incorrect data at 2nd!");
	Assert::AreEqual(expectAnswers.at(2u), testServer.mStorage.at(2u), L"Incorrect data at 3rd!");
	Assert::AreEqual(expectAnswers.at(3u), testServer.mStorage.at(3u), L"Incorrect data at 4th!");
	Assert::AreEqual(expectAnswers.at(4u), testServer.mStorage.at(4u), L"Incorrect data at 5th!");
	
	std::this_thread::sleep_for(std::chrono::milliseconds(waitingTime));
}

void ClientTest::testMethod4()
{
	TestServer testServer;

	std::thread serverThread(initTestServer, std::ref(testServer), 7u);
	serverThread.detach();

	vasily::Client client{ TestServer::SERVER_PORT_SENDING, TestServer::SERVER_PORT_RECEIVING,
		TestServer::SERVER_IP };
	init(client, [&client]()
	{
		client.sendCoordinateType(vasily::Client::CoordinateType::WORLD);
		constexpr std::atomic_int64_t waitingTime = 100LL;
		std::this_thread::sleep_for(std::chrono::milliseconds(waitingTime));

		constexpr std::atomic_uint NUMBER_OF_STEPS = 5u;
		client.partialMovement({ 985'000, 0,		940'000, -180'000, 0,	0, 10, 2, 0 },
							   { 900'000, 100'000,	800'000, -150'000, 10,	0, 10, 2, 0 },
							   NUMBER_OF_STEPS);
	}, 6u);


	constexpr std::atomic_int64_t waitingTime = 1000LL;
	std::this_thread::sleep_for(std::chrono::milliseconds(waitingTime));

	while (!testServer.mHasFinished)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(waitingTime));
	}

	Assert::IsTrue(testServer.mHasConnected, L"Bad connection!");

	Assert::IsFalse(testServer.mStorage.empty(), L"There are not sent coordinates!");

	std::vector<std::string> expectAnswers = { "985000 0 940000 -180000 0 0 10 2 0 ",
											   "968000 20000 912000 -174000 2 0 10 2 0 ",
											   "951000 40000 884000 -168000 4 0 10 2 0 ",
											   "934000 60000 856000 -162000 6 0 10 2 0 ",
											   "917000 80000 828000 -156000 8 0 10 2 0 ",
											   "900000 100000 800000 -150000 10 0 10 2 0 " };
	Assert::AreEqual(expectAnswers.at(0u), testServer.mStorage.at(0u), L"Incorrect data at 1st!");
	Assert::AreEqual(expectAnswers.at(1u), testServer.mStorage.at(1u), L"Incorrect data at 2nd!");
	Assert::AreEqual(expectAnswers.at(2u), testServer.mStorage.at(2u), L"Incorrect data at 3rd!");
	Assert::AreEqual(expectAnswers.at(3u), testServer.mStorage.at(3u), L"Incorrect data at 4th!");
	Assert::AreEqual(expectAnswers.at(4u), testServer.mStorage.at(4u), L"Incorrect data at 5th!");
	Assert::AreEqual(expectAnswers.at(5u), testServer.mStorage.at(5u), L"Incorrect data at 6th!");

	std::this_thread::sleep_for(std::chrono::milliseconds(waitingTime));
}

}

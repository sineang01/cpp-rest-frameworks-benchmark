#include <Poco/Net/ServerSocket.h>
#include <Poco/Net/HTTPServer.h>
#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPRequestHandlerFactory.h>
#include <Poco/Net/HTTPResponse.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>
#include <Poco/Util/ServerApplication.h>
#include <vector>
#include <string>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

class HelloRequestHandler : public Poco::Net::HTTPRequestHandler
{
public:
	virtual void handleRequest(Poco::Net::HTTPServerRequest &req, Poco::Net::HTTPServerResponse &resp)
	{
		resp.setStatus(Poco::Net::HTTPResponse::HTTP_OK);

		char _id[20];
		
		rapidjson::StringBuffer JSONBuffer;
		rapidjson::Writer<rapidjson::StringBuffer> JSONWriter(JSONBuffer);
		JSONWriter.StartArray();

		for (int i = 0; i < 10000; ++i) 
		{
			JSONWriter.StartObject();
			
			snprintf(_id, sizeof(_id), "item-%d", i);
			JSONWriter.Key("id"); JSONWriter.String(_id);
			JSONWriter.Key("name"); JSONWriter.String("Hello World");
			JSONWriter.Key("type"); JSONWriter.String("application");
			
			JSONWriter.EndObject();
		}
		
		JSONWriter.EndArray();

		std::ostream& out = resp.send();
		out << JSONBuffer.GetString();
		out.flush();
	}
};

class HelloRequestHandlerFactory : public Poco::Net::HTTPRequestHandlerFactory
{
public:
	virtual Poco::Net::HTTPRequestHandler* createRequestHandler(const Poco::Net::HTTPServerRequest &)
	{
		return new HelloRequestHandler;
	}
};

class HelloServerApp : public Poco::Util::ServerApplication
{
protected:
	int main(const std::vector<std::string> &)
	{
		Poco::Net::ServerSocket socket(Poco::Net::SocketAddress(Poco::Net::AddressFamily::IPv4, 9000));
		Poco::Net::HTTPServer s(new HelloRequestHandlerFactory, socket, new Poco::Net::HTTPServerParams);

		s.start();
		waitForTerminationRequest();
		s.stop();

		return Application::EXIT_OK;
	}
};

int main(int argc, char** argv)
{
	HelloServerApp app;
	return app.run(argc, argv);
}

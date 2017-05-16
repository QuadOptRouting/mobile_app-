//#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <utility> // std::pair
#include <boost/asio.hpp>
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

using boost::asio::ip::tcp;

enum { max_length = 1024 };

std::string prepareJsonString(std::pair<double, double> coordinates, std::vector<std::pair<unsigned, unsigned>> list)
{
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    writer.StartObject();
    writer.Key("Coordinates");
        writer.StartObject();
        writer.String("Longitude");
        writer.Double(std::get<0>(coordinates));
        writer.String("Latitude");
        writer.Double(std::get<1>(coordinates));
        writer.EndObject();


        writer.Key("List");
        writer.StartArray();// new array for list grugs
         // new object under key "list"
        for(auto it = list.begin(); it < list.end(); ++it)
        {
            writer.StartObject();
            writer.Key("Id");
            writer.Uint(std::get<0>(*it));
            writer.Key("Count");
            writer.Uint(std::get<1>(*it));
            writer.EndObject();
        }
        writer.EndArray();
//        writer.EndObject(); // end object "list"
    writer.EndObject();
    std::string json_list = buffer.GetString();

    return json_list;
}

int main(int argc, char* argv[])
{
    //setlocale(LC_ALL, "");
    std::pair<double, double> coordinates{55.1234, 34.1234};

    std::vector<std::pair<unsigned, unsigned>> list;
    list.push_back(std::pair<unsigned, unsigned> {1, 1});
    list.push_back(std::pair<unsigned, unsigned> {333, 88});
    for(auto it = list.begin(); it < list.end(); ++it)
        std::cout << std::get<0>(*it) << " " << std::get<1>(*it) << std::endl;
        
    std::string json_list = prepareJsonString(coordinates, list);
    std::cout << json_list << std::endl;

  try
  {
    if (argc != 3)
    {
      std::cerr << "Usage: blocking_tcp_echo_client <host> <port>\n";
      return 1;
    }

    boost::asio::io_service io_service;

    tcp::socket s(io_service);
    tcp::resolver resolver(io_service);
    boost::asio::connect(s, resolver.resolve({argv[1], argv[2]}));

    s.send(boost::asio::buffer(json_list, json_list.length()));
    std::cout << "write: " << json_list << std::endl;

    std::string reply(max_length, '\0');
    size_t reply_length = s.read_some(boost::asio::buffer((char*)reply.c_str(), max_length));
    std::cout << "Reply is: " << reply << ", size: " << reply.length() << std::endl;

  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}

//
// Created by daddison on 09/05/18.
//

#include <string>
#include <iostream>
#include <fstream>

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <json/json.h>

#include "bpistats_app.hpp"

namespace bpo = boost::program_options;

using namespace dma;

namespace
{
    const std::string DEFAULT_URL_TO_FETCH = "https://api.coindesk.com/v1/bpi/historical/close.json?start=2018-01-01&end=2018-01-20";
}

bpistats_app::bpistats_app(int argc, char *argv[])
{
    parse_options(argc, argv);
}
bool
bpistats_app::parse_options(int argc, char *argv[])
{

    std::string app_name = boost::filesystem::basename(argv[0]);

    bpo::options_description usage("Options");

    std::string filename;
    std::string begin_date;
    std::string end_date;

    usage.add_options()
            ("help,h", "usage for this application (this info).")
            ("file,f", bpo::value< std::string >(),
                    "the input file name in the format specified by https://api.coindesk.com/v1/bpi/historical/close.json?start=2018-01-01&end=2018-01-20")
            ("begin,b", bpo::value< std::string >(),
                    "the inclusive begin date to analyse, in the format yyyy-mm-dd.  If not present, will use first date of fetched file/url.")
            ("end,e", bpo::value< std::string >(),
                    "the inclusive end date to analyse, in the format yyyy-mm-dd.  Defaults to last date of fetched file/url.");

    bpo::positional_options_description dateRange;
    dateRange.add("begin", 1);
    dateRange.add("end", 1);

    bpo::variables_map vmap;

    try
    {
        bpo::store(bpo::command_line_parser(argc, argv).options(usage)
                   .positional(dateRange).run(), vmap);

        if (vmap.count("help"))
        {
            std::cout << "Usage:" << std::endl;
            std::cout << "    " << app_name << " [options | filename | begin_date end_date]" << std::endl;
            std::cout << std::endl;
//            std::cout << "    " << app_name << " filename            - " << "analyzes entire file" << std::endl;
//            std::cout << "    " << app_name << " begin_date end_date - " << "analyzes canned url between dates (inclusive):  " << std::endl
//                      << "        " << DEFAULT_URL_TO_FETCH << std::endl;
//            std::cout << "    " << app_name << " [options]             " << std::endl;

            std::cout << usage << std::endl;

        }

        bpo::notify(vmap);
    }
    catch (bpo::required_option& e)
    {
        std::cerr << "Error:  " << e.what() << std::endl;

    }
    catch (bpo::error& e)
    {
        std::cerr << "Error:  " << e.what() << std::endl;
    }

    return true;

}

//std::optional<Json::Value> read_file_to_Json(const std::string filepath)
//{
//    Json::Value result;
//    std::ifstream bpifile(filepath, std::ifstream::binary);
//
//    bpifile >> result;
//
//    return std::optional<Json::Value>result;
//}
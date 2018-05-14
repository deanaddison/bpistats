//
// Created by daddison on 09/05/18.
//

#include <memory>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <stdexcept>

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <boost/date_time.hpp>
#include <jsoncpp/json/json.h>

#include "coindesk_analyser.hpp"

#include "bpistats_app.hpp"

namespace bpo = boost::program_options;
namespace bg = boost::gregorian;

using namespace dma;

namespace
{
    const std::string DEFAULT_URL_TO_FETCH = "https://api.coindesk.com/v1/bpi/historical/close.json?start=2018-01-01&end=2018-01-20";
    const std::string DEFAULT_CMDLINE_DATE_FORMAT = "%Y-%m-%d";
}

bpistats_app::bpistats_app(int argc, char *argv[]) :
        app_name( argv[0] ? boost::filesystem::basename(argv[0]) : "app" ),  // argv[argc] == nullptr
        json_path(),
        json_url(DEFAULT_URL_TO_FETCH),
        beg_date(),
        end_date()
{
    parse_options(argc, argv);
}

bpistats_app::~bpistats_app()
{

}

bool
bpistats_app::parse_options(int argc, char *argv[])
{

    bpo::options_description usage("Options");

    usage.add_options()
            ("help,h", "usage for this application (this info).")
            ("pop", bpo::value< std::vector< std::string > >(),
             "[fetch_path | begin_date end_date] Either the BPI input file/url or the start/end dates to fetch:  https://api.coindesk.com/v1/bpi/historical/close.json?start=2018-01-01&end=2018-01-20");

    bpo::positional_options_description posn_opt;
    posn_opt.add("pop", -1);

    bpo::variables_map vm;

    try
    {
        bpo::store(bpo::command_line_parser(argc, argv).options(usage)
                   .positional(posn_opt).run(), vm);

        bpo::notify(vm);

        if (vm.count("help"))
        {
            std::cout << "Usage:" << std::endl;
            std::cout << "    " << app_name << " [options | path | {begin_date end_date}]" << std::endl;
            std::cout << std::endl;
            std::cout << usage << std::endl;

        } else {

            std::vector< std::string > popts(vm["pop"].as< std::vector< std::string > >());
            const unsigned int num_popts = popts.size();

            // This is the simplest implementation to satisfy the tests.
            // Future expansion would likely involve named params in addition to 3 positional params
            if (num_popts == 2) {

                beg_date = bg::from_simple_string(popts[0]);
                end_date = bg::from_simple_string(popts[1]);

                std::cout << "Analysis [beginDate, endDate]:  [" << beg_date << "," << end_date << "]"
                          << std::endl;

            } else if (num_popts == 1) {

                json_path = popts[0];
                std::cout << "Analysis of file:  " << json_path << std::endl;

            } else {

                throw std::invalid_argument(
                        "Number of arguments specified not 1 or 2:  Number was " + std::to_string(vm.count("pop"))
                        + ".\nUse '" + app_name + " --help' for assistance.");

            }

        }
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

/**
 * Read the JSON object from a local file.
 * @param filepath
 * @return
 */
std::unique_ptr< Json::Value >
bpistats_app::read_data_from_file(const std::string& filepath)
{
    auto presult = std::make_unique< Json::Value >();
    std::ifstream bpifile(filepath, std::ifstream::binary);

    if(presult && bpifile)
    {
        bpifile >> *presult;
    }

    return std::move(presult);
}

/**
 * Fetch the JSON object from a remote URL
 * @param url
 * @return
 */
//std::unique_ptr< Json::Value >
//bpistats_app::fetch_data_from_url(const std::string& url)
//{
//    auto presult = std::make_unique< Json::Value >();
//
//    return std::move(presult);
//}

int
bpistats_app::exec()
{
    int result = 0;

    std::unique_ptr< Json::Value > papi_data;
    // Create new json parser and parse file or fetch and parse url
    if(!json_path.empty())
    {
        // Read file into Json::Value
        papi_data = read_data_from_file(json_path);
    } else {
        // Get Json::Value from URL
//        papi_data = fetch_data_from_url(json_url);
    }

    // Create parser and analyser to read in all the data.
    coindesk_analyser analyser(papi_data);

    // Perform analysis and generate report
    auto report = analyser.generate_report();

    std::cout << std::endl;

    if(report)
    {
        std::cout << "Analysis report:" << std::endl;
        std::cout << *report << std::endl;
    }

    return result;
}


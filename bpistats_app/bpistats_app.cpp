//
// Created by daddison
//

#include <memory>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>
#include <stdexcept>

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <boost/date_time.hpp>

#include <curl/curl.h>
#include <jsoncpp/json/json.h>

#include "coindesk_analyser.hpp"

#include "bpistats_app.hpp"

namespace bpo = boost::program_options;
namespace bg = boost::gregorian;          // from <boost/date_time.hpp>

using namespace dma;

namespace
{
    const std::string DEFAULT_URL_TO_FETCH = "https://api.coindesk.com/v1/bpi/historical/close.json";
    const std::string URL_QUERY_PARAM_BEG  = "start=";
    const std::string URL_QUERY_PARAM_END  = "end=";

    std::size_t callback(
            const char* in,
            std::size_t size,
            std::size_t num,
            std::string* out)
    {
        const std::size_t totalBytes(size * num);
        out->append(in, totalBytes);
        return totalBytes;
    }

}

/**
 * Instantiate the class and parse the command-line options.
 * @param argc the command-line option count
 * @param argv the C-array of char* string from the options on the command-line
 */
bpistats_app::bpistats_app(int argc, char *argv[]) :
        app_name( argv[0] ? boost::filesystem::basename(argv[0]) : "app" ),  // argv[argc] == nullptr
        json_path(),
        json_url(DEFAULT_URL_TO_FETCH),
        beg_date(),
        end_date(),
        httpCode(0)
{
    if (!parse_options(argc, argv))
    {
       exit(0);
    }

}

/**
 * Parses the command-line options.
 * The three modes that have been defined are the minimum to satisfy the unit tests.
 *  # --help will display the usage instructions, otherwise
 *  # 1 positional parameter will be interpreted as local filename, which will be read in and parsed as a JSON object.
 *  # 2 positional parameters will be interpreted as a begin and end date, which will be used as URL parameters to fetch the JSON object.
 * @param argc the command-line option count
 * @param argv the C-array of char* string from the options on the command-line
 * @return true if the application should continue, false otherwise (e.g. --help)
 */
bool
bpistats_app::parse_options(int argc, char *argv[])
{
    bool result(true);

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

            result = false;

        } else {

            std::vector< std::string > popts(vm["pop"].as< std::vector< std::string > >());
            const unsigned int num_popts = popts.size();

            // This is the simplest implementation to satisfy the tests.
            // Future expansion would likely involve named params in addition to 3 positional params
            if (num_popts == 2) {

                beg_date = bg::from_simple_string(popts[0]);
                end_date = bg::from_simple_string(popts[1]);

                json_url = DEFAULT_URL_TO_FETCH;
                if(beg_date.is_not_a_date() || end_date.is_not_a_date())
                {
                    result = false;

                } else {

                    json_url.append("?");
                    json_url.append(URL_QUERY_PARAM_BEG);
                    json_url.append(bg::to_iso_extended_string(beg_date));
                    json_url.append("&");
                    json_url.append(URL_QUERY_PARAM_END);
                    json_url.append(bg::to_iso_extended_string(end_date));

                    std::cout << "Analysis from URL:   [beginDate, endDate] = [" << bg::to_iso_extended_string(beg_date) << ", "
                              << bg::to_iso_extended_string(end_date) << "]"
                              << std::endl;
                    std::cout << "Url to fetch:  " << json_url << std::endl;

                }


            } else if (num_popts == 1) {

                json_path = popts[0];
                std::cout << "Analysis of file.  filename = " << json_path << std::endl;

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

    return result;

}

/**
 * Read the JSON object from a local file.
 * @param filepath the path to a local file.
 * @return a std::unique_ptr< Json::Value > containing the JSON object from the file
 */
std::unique_ptr< Json::Value >
bpistats_app::read_data_from_file(const std::string& filepath)
{
    auto presult = std::make_unique< Json::Value >();
    std::ifstream bpifile(filepath, std::ifstream::binary);

    if(presult && bpifile)
    {
        bpifile >> *presult;
    } else {
        throw std::invalid_argument("File not found:  " + filepath);
    }

    return std::move(presult);
}

/**
 * Fetch the JSON object from a remote URL.
 * This is a synchronous call that will timeout after 10 seconds
 * @param url fully qualified url that will return the JSON object.
 * @return a std::unique_ptr< Json::Value > containing the JSON object, as fetched
 */
std::unique_ptr< Json::Value >
bpistats_app::fetch_data_from_url(const std::string& url)
{
    auto presult = std::make_unique< Json::Value >();

    // In lieu of boost::beast, use libcurl
    CURL *curl = curl_easy_init();

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10);

    // Create a string buffer to hold the info (data appended in callback, above)
    std::unique_ptr< std::string > httpData(new std::string());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, callback);   // Callback , above
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, httpData.get()); //

    curl_easy_perform(curl);  // Blocking call with timeout.

    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);
    curl_easy_cleanup(curl);

    Json::Reader jsonReader;
    if (httpCode != 200 || !jsonReader.parse(*httpData, *presult))
    {
        std::stringstream ss;
        ss << "ERROR fetching data from URL:  " << url << ", httpCode == " << std::to_string(httpCode);
        throw std::invalid_argument( ss.str() );
    }

    return std::move(presult);
}

/**
 * Executes the main logic of the application.  In this case:
 *  # fetch the JSON object containing the bitcoin price data from file or url
 *  # load the JSON object into the analyser
 *  # generate and display the report from the analyser
 * @return the exit code of the app
 */
int
bpistats_app::exec()
{
    int result = 0;

    // Create new json parser and parse file or fetch and parse url
    std::unique_ptr< Json::Value > papi_data;
    if(!json_path.empty())
    {
        // Read file into Json::Value
        papi_data = read_data_from_file(json_path);
    } else {
        // Get Json::Value from URL
        papi_data = fetch_data_from_url(json_url);
    }

    // Create analyser and load it with the JSON object.
    coindesk_analyser analyser(papi_data);

    // Perform analysis and generate report
    auto report = analyser.generate_report();

    if(report)
    {
        std::cout << "Analysis report:" << std::endl;
        std::cout << *report << std::endl;
    }

    return result;
}


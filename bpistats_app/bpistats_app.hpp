//
// Created by daddison
//

#ifndef BPISTATS_BPISTATS_HPP
#define BPISTATS_BPISTATS_HPP

#include <memory>
#include <iostream>
#include <string>

#include <boost/date_time.hpp>
#include <boost/property_tree/ptree_fwd.hpp>


// Forward Declaration
namespace Json { class Value; }

namespace dma
{
    /**
     * Application which takes arguments from the command-line,
     * reads in file/url, performs analysis, and generates a report based on those arguments.
     */
    class bpistats_app final
    {
    public:
        bpistats_app(int argc, char *argv[]);
        ~bpistats_app() = default;

        int exec();

        bool parse_options(int argc, char* argv[]);

        std::unique_ptr< boost::property_tree::ptree > read_data_from_file(const std::string& filepath);
        std::unique_ptr< boost::property_tree::ptree > fetch_data_from_url(const std::string& url);

    private:

        bool valid_init;       //!< Command-line params parsed correctly.

        std::string app_name;  //!< Captured command-line name of app for use in reports

        std::string json_path;  //!< Path to file
        std::string json_url;   //!< Url to API

        boost::gregorian::date beg_date;  //!< Inclusive starting data for analysis
        boost::gregorian::date end_date;  //!< Inclusive ending date for analysis

        int httpCode;
    };
}

#endif //BPISTATS_BPISTATS_HPP

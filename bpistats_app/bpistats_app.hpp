//
// Created by daddison
//

#ifndef BPISTATS_BPISTATS_HPP
#define BPISTATS_BPISTATS_HPP

#include <memory>
#include <iostream>
#include <string>

#include <boost/date_time.hpp>


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
        ~bpistats_app();

        int exec();

        bool parse_options(int argc, char* argv[]);

        void fetch_complete(std::unique_ptr< Json::Value > data);

        std::unique_ptr< Json::Value > read_data_from_file(const std::string& filepath);
//        std::unique_ptr< Json::Value > fetch_data_from_url(const std::string& url);

    private:

        std::string app_name;  //!< Captured command-line name of app for use in reports

        std::string json_path;  //!< Path to file
        std::string json_url;   //!< Url to API

        boost::gregorian::date beg_date;  //!< Inclusive starting data for analysis
        boost::gregorian::date end_date;  //!< Inclusive ending date for analysis

    };
}

#endif //BPISTATS_BPISTATS_HPP

//
// Created by daddison.
//

#ifndef DMA_COINDESK_RESULT_HPP
#define DMA_COINDESK_RESULT_HPP

#include <memory>
#include <string>

#include <boost/date_time.hpp>

// Forward Declarations
namespace Json { class Value; }

namespace dma {

    typedef std::vector< std::pair< boost::gregorian::date, float > > price_data_t;

/**
 * Simple class to parse JSON data from the Coindesk v1 historical close API.
 * Will analyse the data in a JSON object and generate a JSON report with calculated zeroth-, first-, and second-order statistical values.
 */
    class coindesk_analyser
    {
        static const std::string SOURCE_KEY_JSON_PRICE_OBJECT;  //!< incoming key for object containing date, price tuples.
        static const std::string SOURCE_KEY_JSON_TIME_OBJECT;   //!< incoming key for object containing update times.
        static const std::string SOURCE_KEY_JSON_TIME_UPDATE;   //!< incoming key for desired update time.

    public:
        explicit coindesk_analyser(std::unique_ptr< Json::Value >& papi_data);

        virtual ~coindesk_analyser() {}

        std::unique_ptr< Json::Value > generate_report();

    protected:

        price_data_t parse_price_data(std::unique_ptr< Json::Value >& papi_data) const;

    private:
        price_data_t close_data;
        boost::gregorian::date m_updated;

    };

} // namespace

#endif //DMA_COINDESK_RESULT_HPP

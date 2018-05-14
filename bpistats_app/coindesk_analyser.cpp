//
// Created by daddison
//

#include <memory>
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include <cmath>

#include <boost/date_time.hpp>
#include <jsoncpp/json/json.h>

#include "coindesk_analyser.hpp"


using namespace dma;

namespace bg = boost::gregorian;

const std::string coindesk_analyser::SOURCE_KEY_JSON_PRICE_OBJECT = "bpi";
const std::string coindesk_analyser::SOURCE_KEY_JSON_TIME_OBJECT  = "time";
const std::string coindesk_analyser::SOURCE_KEY_JSON_TIME_UPDATE  = "updatedISO";

coindesk_analyser::coindesk_analyser(std::unique_ptr< Json::Value >& papi_data) :
    close_data(),
    m_updated()
{
    if(papi_data)
    {
//        m_updated = bg::from_simple_string((*papi_data)[SOURCE_KEY_JSON_TIME_OBJECT][SOURCE_KEY_JSON_TIME_UPDATE].asString());

        price_data_t data(parse_price_data(papi_data));

        std::copy(data.begin(), data.end(), std::back_inserter(close_data));

    }
}

/**
 * Will convert all timeseries data from JSON object and return it.
 * @param papi_data the std::unique_ptr< Json::Value > containing the timeseries data with the key SOURCE_KEY_JSON_PRICE_OBJECT
 * @return a std::vector< std::pair< boost::gregorian::data, float > > containing the unsorted values from api_data
 */
price_data_t
coindesk_analyser::parse_price_data(std::unique_ptr< Json::Value >& papi_data) const
{
    auto result = price_data_t();

    if(papi_data)
    {

        auto const data = (*papi_data)[SOURCE_KEY_JSON_PRICE_OBJECT];
        auto keys = data.getMemberNames();

        std::transform(keys.begin(), keys.end(), std::back_inserter(result), [&data](auto& key)
        {
            return std::make_pair< bg::date, float >(bg::from_simple_string(key), data[key].asFloat());

        });

    }

    return result;
}

/**
 * Generate a report for:
 *  - low point:  date and price
 *  - high point: date and price
 *  - count:  number of prices
 *  - average:  calculation of the mean for the range;
 *  - median:  median price for the range
 * @return
 */
std::unique_ptr< Json::Value >
coindesk_analyser::generate_report()
{
    auto presult = std::make_unique< Json::Value >();
    return std::move(presult);
}


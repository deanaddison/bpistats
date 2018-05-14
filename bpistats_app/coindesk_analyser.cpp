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
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "coindesk_analyser.hpp"


using namespace dma;

namespace bg = boost::gregorian;      // from <boost/date_time.hpp>
namespace pt = boost::property_tree;  // from <boost/property_tree/ptree.hpp>

const std::string coindesk_analyser::SOURCE_KEY_JSON_PRICE_OBJECT = "bpi";
const std::string coindesk_analyser::SOURCE_KEY_JSON_TIME_OBJECT  = "time";
const std::string coindesk_analyser::SOURCE_KEY_JSON_TIME_UPDATE  = "updatedISO";

const std::string coindesk_analyser::REPORT_KEY_PRICE_LOW    = "price_low";
const std::string coindesk_analyser::REPORT_KEY_PRICE_HIGH   = "price_high";
const std::string coindesk_analyser::REPORT_KEY_PRICE_COUNT  = "price_count";
const std::string coindesk_analyser::REPORT_KEY_PRICE_AVG    = "price_average";
const std::string coindesk_analyser::REPORT_KEY_PRICE_MED    = "price_median";
const std::string coindesk_analyser::REPORT_KEY_PRICE_STDDEV = "price_stddev";

coindesk_analyser::coindesk_analyser(std::unique_ptr< pt::ptree >& papi_data) :
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
coindesk_analyser::parse_price_data(std::unique_ptr< pt::ptree >& papi_data) const
{
    auto result = price_data_t();

    if(papi_data && !papi_data->empty())
    {

        const pt::ptree& data = papi_data->get_child(SOURCE_KEY_JSON_PRICE_OBJECT);

        if(!data.empty())
        {
            std::transform(data.begin(), data.end(), std::back_inserter(result),
                           [](const std::pair<std::string, pt::ptree> &elem)
                           {
                               return std::make_pair<bg::date, float>(bg::from_simple_string(elem.first),
                                                                      elem.second.get_value<float>());

                           });
        }

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
std::unique_ptr< pt::ptree >
coindesk_analyser::generate_report()
{
    auto presult = std::make_unique< pt::ptree >();

    const unsigned int count = close_data.size();
    if(count > 0 && presult)
    {
        std::pair<bg::date, float>& firstelem{close_data[0]};
        std::pair<bg::date, float> lowpoint{firstelem};
        std::pair<bg::date, float> highpoint{firstelem};

        const float firstprice{firstelem.second};
        float sum{firstprice};
        std::vector< float > prices{firstprice};

        // min, max, and sum in one loop, unrolled by one, for efficiency
        // create prices vector for median and stddev calcs, as well.
        std::for_each(std::next(close_data.begin()), close_data.end(), [&lowpoint, &highpoint, &sum, &prices](const auto& elem)
        {
            const float price{elem.second};
            if(price < lowpoint.second)
            {
                lowpoint = elem;
            }
            if(price > highpoint.second)
            {
                highpoint = elem;
            }

            sum += price;

            prices.push_back(price);
        });

        const float mean = sum / count;  // count is never 0

        float variance = 0;
        // variance using prices vector
        std::for_each(prices.begin(), prices.end(), [&variance, mean](const auto& elem)
        {
            const float diff = elem - mean;
            variance += diff * diff;
        });

        variance /= count > 1 ? (count - 1) : count;  // variance of single point is / N as mean is assumed known a priori

        const float stddev = std::sqrt(variance);  // variance is always >= 0 due to diff*diff

        // median calculation
        // Get the lowest (n/2 + 1) elems.  If odd, take middle value, else if even, estimate from mean of middle two elems.
        const bool even = count % 2 == 0;
        const unsigned int leftmiddle = count / 2;

        std::partial_sort(prices.begin(), std::next(prices.begin(), leftmiddle + 1), prices.end());      // ... + 1 for num to sort
        float median = even ? (prices[leftmiddle] + prices[leftmiddle + 1]) / 2.0f : prices[leftmiddle];  // find mean (if even) or value (if odd)

        const std::string lowdate(REPORT_KEY_PRICE_LOW+"."+bg::to_iso_extended_string(lowpoint.first));
        presult->add(lowdate, lowpoint.second);
        const std::string highdate(REPORT_KEY_PRICE_HIGH+"."+bg::to_iso_extended_string(highpoint.first));
        presult->add(highdate, highpoint.second);
        presult->add(REPORT_KEY_PRICE_COUNT, count);
        presult->add(REPORT_KEY_PRICE_AVG, mean);
        presult->add(REPORT_KEY_PRICE_MED, median);
        presult->add(REPORT_KEY_PRICE_STDDEV, stddev);
    }
    return std::move(presult);
}


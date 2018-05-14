//
// Created by daddison
//

#include <memory>
#include <string>
#include <sstream>
#include <fstream>
#include <limits>

#include <jsoncpp/json/json.h>
#include "bpistats_app/coindesk_analyser.hpp"
#include "gtest/gtest.h"

using namespace ::testing;

using namespace dma;

namespace
{
    std::unique_ptr< Json::Value > pemptyJson = std::make_unique< Json::Value >();

    std::string sampleJsonString1("{\"bpi\":{\"2018-04-10\":6830.9038,\"2018-04-11\":6939.55,\"2018-04-12\":7916.3725,\"2018-04-13\":7889.2313,\"2018-04-14\":8003.6813,\"2018-04-15\":8357.0375,\"2018-04-16\":8051.345,\"2018-04-17\":7890.15,\"2018-04-18\":8163.69,\"2018-04-19\":8273.7413,\"2018-04-20\":8863.5025,\"2018-04-21\":8917.5963,\"2018-04-22\":8792.83,\"2018-04-23\":8938.3038,\"2018-04-24\":9652.1563,\"2018-04-25\":8864.0875,\"2018-04-26\":9278.9975,\"2018-04-27\":8978.3325,\"2018-04-28\":9342.4713,\"2018-04-29\":9392.0313,\"2018-04-30\":9244.3225,\"2018-05-01\":9067.715,\"2018-05-02\":9219.8638,\"2018-05-03\":9734.675,\"2018-05-04\":9692.7175,\"2018-05-05\":9826.5975,\"2018-05-06\":9619.1438,\"2018-05-07\":9362.5338,\"2018-05-08\":9180.1588,\"2018-05-09\":9306,\"2018-05-10\":9014.615},\"disclaimer\":\"This data was produced from the CoinDesk Bitcoin Price Index. BPI value data returned as USD.\",\"time\":{\"updated\":\"May 11, 2018 00:03:00 UTC\",\"updatedISO\":\"2018-05-11T00:03:00+00:00\"}}\"");
    std::stringstream sampleJsonSS1(sampleJsonString1);
    std::string sampleJsonFilePath("test.json");

    const float average = 8793.6888f;
    const float median  = 8978.3320f;
    const float stddev  = 777.29031f;

    const boost::gregorian::date low_date{boost::gregorian::from_simple_string("2018-04-10")};
    const float low_price = 6830.9038f;

    const boost::gregorian::date high_date{boost::gregorian::from_simple_string("2018-05-05")};
    const float high_price = 9826.5975f;

} // namespace

TEST(CoindeskAnalyser, NoThrowWithEmptyJson)
{
    EXPECT_NO_THROW(coindesk_analyser ca(pemptyJson));

}

TEST(CoindeskAnalyser, NoThrowWithSampleJson)
{
    std::ifstream sample(sampleJsonFilePath, std::ifstream::binary);
    std::unique_ptr< Json::Value > psampleJson1 = std::make_unique< Json::Value >();
    sample >> *psampleJson1;
    EXPECT_NO_THROW(coindesk_analyser ca(psampleJson1));

    coindesk_analyser cda(psampleJson1);
    EXPECT_NO_THROW(std::unique_ptr< Json::Value > generatedReport1{cda.generate_report()});

}

TEST(CoindeskAnalyser, CompareSampleReportValuesToSpreadsheetCalcs)
{
    std::ifstream sample(sampleJsonFilePath, std::ifstream::binary);
    std::unique_ptr< Json::Value > psampleJson1 = std::make_unique< Json::Value >();
    sample >> *psampleJson1;

    coindesk_analyser cda(psampleJson1);

    std::unique_ptr< Json::Value > generatedReport1 = cda.generate_report();

    EXPECT_EQ(  (*generatedReport1)["price_count"].asInt(), 31);
    EXPECT_NEAR((*generatedReport1)["price_average"].asFloat(), average, average * std::numeric_limits<float>::epsilon());
    EXPECT_NEAR((*generatedReport1)["price_median"].asFloat(),   median,  median * std::numeric_limits<float>::epsilon());
    EXPECT_NEAR((*generatedReport1)["price_stddev"].asFloat(),   stddev,  stddev * std::numeric_limits<float>::epsilon());

    EXPECT_NEAR((*generatedReport1)["price_low"][boost::gregorian::to_iso_extended_string(low_date)].asFloat(),   low_price,  low_price  * std::numeric_limits<float>::epsilon());
    EXPECT_NEAR((*generatedReport1)["price_high"][boost::gregorian::to_iso_extended_string(high_date)].asFloat(), high_price, high_price * std::numeric_limits<float>::epsilon());
}

class coindesk_analyser_test : public ::testing::Test
{
protected:
    coindesk_analyser_test() {}
    virtual ~coindesk_analyser_test() {}

    virtual void SetUp() {}
    virtual void TearDown() {}
};


//int main(int argc, char *argv[])
//{
//    InitGoogleTest(&argc, argv);
//    return RUN_ALL_TESTS();
//}

//
// Created by daddison
//

#include <memory>
#include <string>
#include <sstream>
#include <fstream>


#include <jsoncpp/json/json.h>
#include "bpistats_app/bpistats_app.hpp"
#include "gtest/gtest.h"

using namespace ::testing;
using namespace dma;

namespace
{
    int argc_empty(1);
    char * argv_empty[]{"bpistats_app", nullptr};

    int argc_onefile(2);
    char * argv_onefile[]{"bpistats_app", "test.json", nullptr};

    int argc_dates(3);
    char * argv_dates[]{"bpistats_app", "2018-10-04", "2018-05-10", nullptr};

    int argc_help(3);
    char *argv_help[]{"bpistats_app", "--help", "2018-10-04", nullptr};

    int argc_missingfile(2);
    char *argv_missingfile[]{"bpistats_app", "2018-10-04", nullptr};

} // namespace

TEST(BpiStatsApp, EmptyCmdLine)
{
    EXPECT_NO_THROW(bpistats_app bsa(argc_empty, argv_empty));
}

TEST(BpiStatsApp, HelpCmdLine)
{
    EXPECT_NO_THROW(bpistats_app bsa(argc_help, argv_help));
}

TEST(BpiStatsApp, GoodCmdLineFileDates)
{
    EXPECT_NO_THROW(bpistats_app bsa(argc_onefile, argv_onefile));
    EXPECT_NO_THROW(bpistats_app bsa(argc_dates, argv_dates));
}

TEST(BpiStatsApp, BadCmdLineMissingFile)
{
    EXPECT_NO_THROW(bpistats_app bsa(argc_missingfile, argv_missingfile));
}

int main(int argc, char *argv[])
{
    InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

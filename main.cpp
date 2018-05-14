//
// Created by daddison on 09/05/18.
//

#include "bpistats_app/bpistats_app.hpp"


int main(int argc, char *argv[])
{
    int result = -1;

    try
    {
        dma::bpistats_app app(argc, argv);

        result = app.exec();
    }
    catch(std::exception& e)
    {
        std::cerr << "ERROR:  Unhandled exception on startup:  " << e.what() << std::endl;
    }

    return result;

}

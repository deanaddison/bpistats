//
// Created by daddison on 09/05/18.
//

#ifndef BPISTATS_BPISTATS_HPP
#define BPISTATS_BPISTATS_HPP

#include <iostream>


namespace dma
{
    class bpistats_app final
    {
    public:
        bpistats_app(int argc, char *argv[]);
        ~bpistats_app() {}

        int exec() { return 0; }

        bool parse_options(int argc, char* argv[]);

//        void read_file(const std::string filepath);
    };
}

#endif //BPISTATS_BPISTATS_HPP

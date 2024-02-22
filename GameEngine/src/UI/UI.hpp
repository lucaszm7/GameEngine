#pragma once

#include <nfd.h>
#include <string>
#include <iostream>

namespace UI
{
    class FileDialog
    {
    public:
        static std::string Get()
        {
            nfdchar_t* outPath = NULL;
            nfdresult_t result = NFD_OpenDialog(NULL, NULL, &outPath);

            if (result == NFD_OKAY)
                return std::string(outPath);
            else if (result == NFD_CANCEL)
                return std::string();
            else
                std::cout << "Error: " << NFD_GetError() << std::endl;
            return std::string();
        }
    };
}


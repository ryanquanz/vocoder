//
//  FileHandler.hpp
//  Vocoder - ConsoleApp
//
//  Created by Ryan Quanz on 2019-04-01.
//  Copyright © 2019 Ryan Quanz and Austin Hardy. All rights reserved.
//

#ifndef FileHandler_hpp
#define FileHandler_hpp
#include "../JuceLibraryCode/JuceHeader.h"

class FileHandler {
public:
    std::vector<float> readSamplesFromFile(const std::string &);
    void writeSamplesToFile(std::vector<float> &, const std::string &, int, int);
};

#endif /* FileHandler_hpp */

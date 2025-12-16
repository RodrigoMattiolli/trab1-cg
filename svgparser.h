#ifndef SVGPARSER_H
#define SVGPARSER_H

#include <string>
#include <vector>

struct SvgCircle {
    float cx;
    float cy;
    float r;
    std::string fill;
};

class SvgParser {
public:
    static bool ParseFile(const std::string &filepath, std::vector<SvgCircle> &out);
};

#endif

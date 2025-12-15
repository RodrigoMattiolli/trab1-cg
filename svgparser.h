#ifndef SVGPARSER_H
#define SVGPARSER_H

#include <string>
#include <vector>

struct SvgCircle {
    float cx;
    float cy;
    float r;
    std::string fill; // color like "#rrggbb" or "rgb(...)" or "red"
};

class SvgParser {
public:
    // parse minimal SVG with circle elements. filepath: path to .svg
    static bool ParseFile(const std::string &filepath, std::vector<SvgCircle> &out);
};

#endif

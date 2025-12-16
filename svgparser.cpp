#include "svgparser.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>

static std::string trim(const std::string &s){
    size_t a = s.find_first_not_of(" \t\r\n");
    if (a==std::string::npos) return "";
    size_t b = s.find_last_not_of(" \t\r\n");
    return s.substr(a, b-a+1);
}

static std::string getAttr(const std::string &tag, const std::string &attr) {
    // procura attr= value
    size_t p = tag.find(attr);
    if (p==std::string::npos) return "";
    size_t eq = tag.find('=', p);
    if (eq==std::string::npos) return "";
    size_t q1 = tag.find_first_of("\"'", eq);
    if (q1==std::string::npos) return "";
    char quote = tag[q1];
    size_t q2 = tag.find(quote, q1+1);
    if (q2==std::string::npos) return "";
    return tag.substr(q1+1, q2-q1-1);
}

static float toFloat(const std::string &s){
    try {
        return std::stof(s);
    } catch(...) { return 0.0f; }
}

bool SvgParser::ParseFile(const std::string &filepath, std::vector<SvgCircle> &out){
    std::ifstream ifs(filepath);
    if (!ifs.is_open()) return false;
    std::stringstream buf;
    buf << ifs.rdbuf();
    std::string content = buf.str();
    std::string low = content;
    std::transform(low.begin(), low.end(), low.begin(), ::tolower);

    size_t pos = 0;
    while (true){
        size_t start = low.find("<circle", pos);
        if (start == std::string::npos) break;
        size_t end = low.find('>', start);
        if (end == std::string::npos) break;
        std::string tag = content.substr(start, end-start+1);
        SvgCircle c;
        std::string cxs = getAttr(tag, "cx");
        std::string cys = getAttr(tag, "cy");
        std::string rs = getAttr(tag, "r");
        std::string fill = getAttr(tag, "fill");
        if (cxs.empty() && (low.find("cx=", start)==std::string::npos)) {
            cxs = "0";
        }
        c.cx = toFloat(cxs);
        c.cy = toFloat(cys);
        c.r = toFloat(rs);
        c.fill = trim(fill);
        out.push_back(c);
        pos = end+1;
    }
    return true;
}

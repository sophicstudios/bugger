#ifndef INCLUDED_ATTA_UTIL_CONVERT_H
#define INCLUDED_ATTA_UTIL_CONVERT_H

#include <CoreFoundation/CFString.h>
#include <CoreFoundation/CFURL.h>
#include <aftu_url.h>
#include <string>

namespace util {

struct Convert
{
    static std::string toString(CFStringRef str);
    
    static CFStringRef copyToCFString(std::string const& str);
    
    static aftu::URL toURL(CFURLRef url);
    
    static CFURLRef copyToCFURL(aftu::URL const& url);
};

} // namespace

#endif // INCLUDED

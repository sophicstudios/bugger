#include <util_convert.h>

namespace util {

std::string Convert::toString(CFStringRef str)
{
    CFStringEncoding encoding = CFStringGetSystemEncoding();
    char const* buffer = CFStringGetCStringPtr(str, encoding);
    if (buffer) {
        return std::string(buffer);
    }
    else {
        CFIndex bufferSize = CFStringGetLength(str) + 1;
        char* temp = new char[bufferSize];
        CFStringGetCString(str, temp, bufferSize, encoding);
        std::string result(temp);
        delete [] temp;
        return result;
    }
}

CFStringRef Convert::copyToCFString(std::string const& str)
{
    return CFStringCreateWithCString(NULL, str.c_str(), CFStringGetSystemEncoding());
}

aftu::URL Convert::toURL(CFURLRef url)
{
    CFURLRef absoluteUrl = CFURLCopyAbsoluteURL(url);
    CFStringRef urlStr;
    std::string temp;
    if (absoluteUrl) {
        urlStr = CFURLGetString(absoluteUrl);
        temp = toString(urlStr);
        CFRelease(absoluteUrl);
    } else {
        urlStr = CFURLGetString(url);
        temp = toString(urlStr);
    }

    return aftu::URL(temp);
}

CFURLRef Convert::copyToCFURL(aftu::URL const& url)
{
    CFStringRef urlStr = copyToCFString(url.canonical().c_str());
    CFURLRef cfUrl = CFURLCreateWithString(NULL, urlStr, NULL);
    CFRelease(urlStr);
    return cfUrl;
}

} // namespace

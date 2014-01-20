#ifndef INCLUDED_ATTA_UTIL_IMAGEPNG_H
#define INCLUDED_ATTA_UTIL_IMAGEPNG_H

#include <aftfs_filesystem.h>
#include <aftu_url.h>
#include <vector>
#include <png.h>

namespace util {

class ImagePNG
{
    friend void infoCallback_c(png_structp, png_infop);
    friend void rowCallback_c(png_structp, png_bytep, png_uint_32, int);
    friend void endCallback_c(png_structp, png_infop);

public:
    ImagePNG(aftfs::Filesystem& filesystem, aftu::URL const& filename);
    
    ~ImagePNG();
    
    int width() const;
    
    int height() const;
    
    std::vector<char> const& data() const;
    
private:
    void infoCallback(png_structp pngData, png_infop pngInfo);
    void rowCallback(png_structp pngData, png_bytep pngRow, png_uint_32 rowNum, int pass);
    void endCallback(png_structp pngData, png_infop pngInfo);
    
    int m_width;
    int m_height;
    int m_colorType;
    int m_bitDepth;
    int m_channels;
    bool m_isInterlaced;
    int m_numberOfPasses;
    size_t m_rowbytes;
    std::vector<char> m_image;
};

} // namespace

#endif // INCLUDED
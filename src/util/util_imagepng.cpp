#include <util_imagepng.h>
#include <iostream>

namespace util {

namespace {

const char* colorTypeToString(int colorType)
{
    switch (colorType) {
    case PNG_COLOR_TYPE_GRAY: return "PNG_COLOR_TYPE_GRAY"; break;
    case PNG_COLOR_TYPE_GRAY_ALPHA: return "PNG_COLOR_TYPE_GRAY_ALPHA"; break;
    case PNG_COLOR_TYPE_PALETTE: return "PNG_COLOR_TYPE_PALETTE"; break;
    case PNG_COLOR_TYPE_RGB: return "PNG_COLOR_TYPE_RGB"; break;
    case PNG_COLOR_TYPE_RGB_ALPHA: return "PNG_COLOR_TYPE_RGB_ALPHA"; break;
    default: return "Unknown"; break;
    }
}

} // namespace

void infoCallback_c(png_structp pngData, png_infop pngInfo)
{
    ImagePNG* png = reinterpret_cast<ImagePNG*>(png_get_progressive_ptr(pngData));
    png->infoCallback(pngData, pngInfo);
}

void rowCallback_c(png_structp pngData, png_bytep pngRow, png_uint_32 rowNum, int pass)
{
    ImagePNG* png = reinterpret_cast<ImagePNG*>(png_get_progressive_ptr(pngData));
    png->rowCallback(pngData, pngRow, rowNum, pass);
}

void endCallback_c(png_structp pngData, png_infop pngInfo)
{
    ImagePNG* png = reinterpret_cast<ImagePNG*>(png_get_progressive_ptr(pngData));
    png->endCallback(pngData, pngInfo);
}

ImagePNG::ImagePNG(aftfs::Filesystem& filesystem, aftu::URL const& filename)
: m_width(0),
  m_height(0),
  m_image(NULL)
{
    png_uint_32 pngVersion = png_access_version_number();
    std::cout << "ImagePNG: libpng version is " << pngVersion << std::endl;
    
    aftfs::Filesystem::DirectoryEntryPtr entry = filesystem.directoryEntry(filename);
    aftfs::Filesystem::FileReaderPtr fileReader = filesystem.openFileReader(filename);

    char* buffer = new char[entry->size()];
    size_t bytesRead = 0;
    fileReader->read(buffer, entry->size(), &bytesRead);

    if (png_sig_cmp(reinterpret_cast<png_const_bytep>(buffer), 0, 8) != 0) {
        // not a PNG file
        std::cerr << "ImagePNG: Not a PNG file" << std::endl;
        throw std::exception();
    }
    
    png_structp pngData = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!pngData) {
        std::cerr << "ImagePNG: Could not create read struct" << std::endl;
        throw std::exception();
    }
    
    png_infop pngInfo = png_create_info_struct(pngData);
    if (!pngInfo) {
        std::cerr << "ImagePNG: Could not create info struct" << std::endl;
        png_destroy_read_struct(&pngData, NULL, NULL);
        throw std::exception();
    }
    
    if (setjmp(png_jmpbuf(pngData))) {
        std::cerr << "ImagePNG: setjmp returned non-zero" << std::endl;
        png_destroy_read_struct(&pngData, &pngInfo, NULL);
        throw std::exception();
    }
    
    png_set_progressive_read_fn(pngData, this, infoCallback_c, rowCallback_c, endCallback_c);
    
    png_process_data(pngData, pngInfo, reinterpret_cast<png_bytep>(buffer), entry->size());
    
    delete [] buffer;
}

ImagePNG::~ImagePNG()
{
}

int ImagePNG::width() const
{
    return m_width;
}

int ImagePNG::height() const
{
    return m_height;
}

std::vector<char> const& ImagePNG::data() const
{
    return m_image;
}

void ImagePNG::infoCallback(png_structp pngData, png_infop pngInfo)
{
    m_width = png_get_image_width(pngData, pngInfo);
    m_height = png_get_image_height(pngData, pngInfo);
    m_colorType = static_cast<int>(png_get_color_type(pngData, pngInfo));
    m_bitDepth = static_cast<int>(png_get_bit_depth(pngData, pngInfo));
    m_channels = static_cast<int>(png_get_channels(pngData, pngInfo));
    m_isInterlaced = png_get_interlace_type(pngData, pngInfo) != PNG_INTERLACE_NONE;

    if (m_isInterlaced) {
        m_numberOfPasses = png_set_interlace_handling(pngData);
    } else {
        m_numberOfPasses = 1;
    }

    std::cout << "ImagePNG::infoCallback (initial) ["
        << " width: " << m_width
        << " height: " << m_height
        << " colorType: " << colorTypeToString(m_colorType)
        << " bitDepth: " << m_bitDepth
        << " channels: " << m_channels
        << " isInterlaced: " << m_isInterlaced
        << " numberOfPasses: " << m_numberOfPasses
        << " ]" << std::endl;

    png_set_expand(pngData);
    png_set_gray_to_rgb(pngData);

    m_isInterlaced = png_get_interlace_type(pngData, pngInfo) != PNG_INTERLACE_NONE;
    if (m_isInterlaced) {
        m_numberOfPasses = png_set_interlace_handling(pngData);
    } else {
        m_numberOfPasses = 1;
    }
    
    png_read_update_info(pngData, pngInfo);
    
    m_colorType = static_cast<int>(png_get_color_type(pngData, pngInfo));
    m_bitDepth = static_cast<int>(png_get_bit_depth(pngData, pngInfo));
    m_channels = static_cast<int>(png_get_channels(pngData, pngInfo));
    
    std::cout << "ImagePNG::infoCallback (revised) ["
        << " width: " << m_width
        << " height: " << m_height
        << " colorType: " << colorTypeToString(m_colorType)
        << " bitDepth: " << m_bitDepth
        << " channels: " << m_channels
        << " isInterlaced: " << m_isInterlaced
        << " numberOfPasses: " << m_numberOfPasses
        << " ]" << std::endl;
    
    m_rowbytes = png_get_rowbytes(pngData, pngInfo);
    m_image.resize(m_rowbytes * m_height);
}

void ImagePNG::rowCallback(png_structp pngData, png_bytep pngRow, png_uint_32 rowNum, int pass)
{
    std::cout << "ImagePNG::rowCallback ["
        << " rowNum: " << rowNum
        << " pass: " << pass
        << " ]" << std::endl;

    if (m_isInterlaced && pass != 0) {
        png_bytep oldRow = reinterpret_cast<png_bytep>(&m_image[rowNum * m_rowbytes]);
        png_progressive_combine_row(pngData, oldRow, pngRow);
    }
    
    // copy the data
    std::memcpy(&m_image[rowNum * m_rowbytes], pngRow, m_rowbytes);
}

void ImagePNG::endCallback(png_structp pngData, png_infop pngInfo)
{
    std::cout << "ImagePNG::endCallback" << std::endl;
}

} // namespace

#include <util_bundlefilesystem.h>
#include <util_convert.h>
#include <aftio_reader.h>
#include <aftu_exception.h>
#include <dirent.h>
#include <sys/stat.h>

namespace util {

namespace {

long getPathMaxLength(std::string const& path)
{
    long name_max = pathconf(path.c_str(), _PC_NAME_MAX);
    if (name_max == -1) { // limit not defined, or error
        name_max = 1024;
    }
    
    return name_max;
}

} // nmaespace

struct BundleFilesystem::Priv
{
    CFBundleRef bundle;
    CFURLRef rootUrl;
    CFURLRef currentUrl;
};

class BundleDirectoryEntry : public aftfs::DirectoryEntry
{
public:
    explicit BundleDirectoryEntry(aftu::URL const& url, struct stat const& info);
    
    virtual ~BundleDirectoryEntry();
    
    virtual aftu::URL url();
    
    virtual aftio::Permissions permissions();
    
    virtual Type type() const;
    
    virtual bool isFile() const;
    
    virtual bool isDirectory() const;
    
    virtual bool isLink() const;
    
    virtual size_t size() const;
    
private:
    aftu::URL m_url;
    struct stat m_info;
};

class BundleFileReader : public aftio::Reader
{
public:
    BundleFileReader(FILE* fileHandle);
    
    virtual ~BundleFileReader();
    
    virtual aftio::Reader::Status read(char* buffer, size_t bytes, size_t* actualBytes);
    
    virtual aftio::Reader::Status read(std::vector<char>& buffer, size_t bytes, size_t* actualBytes);
    
    virtual aftio::Reader::Status reset();
    
    virtual bool eof(aftio::Reader::Status* status = NULL);
    
    virtual aftio::Reader::Status position(off_t position);
    
    virtual off_t position(aftio::Reader::Status* status = NULL);

private:
    FILE* m_fileHandle;
};


BundleFilesystem::BundleFilesystem()
: m_priv(new BundleFilesystem::Priv())
{
    m_priv->bundle = CFBundleGetMainBundle();
    if (!m_priv->bundle) {
        throw aftu::Exception("Unable to load main bundle!");
    }
    
    m_priv->rootUrl = CFBundleCopyBundleURL(m_priv->bundle);
    if (!m_priv->rootUrl) {
        throw aftu::Exception("Unable to get bundle root URL!");
    }
}

BundleFilesystem::BundleFilesystem(Root root)
: m_priv(new BundleFilesystem::Priv())
{
    m_priv->bundle = CFBundleGetMainBundle();
    if (!m_priv->bundle) {
        throw aftu::Exception("Unable to load main bundle!");
    }
    
    m_priv->rootUrl = CFBundleCopyResourcesDirectoryURL(m_priv->bundle);
    if (!m_priv->rootUrl) {
        throw aftu::Exception("Unable to get bundle root URL!");
    }
}

BundleFilesystem::~BundleFilesystem()
{
    if (m_priv->rootUrl) {
        CFRelease(m_priv->rootUrl);
    }
}

aftu::URL BundleFilesystem::getCurrentDirectory(aftfs::Filesystem::Status* status)
{
    aftu::URL result = Convert::toURL(m_priv->rootUrl);
    if (status) {
        *status = aftfs::Filesystem::Status_OK;
    }
    return result;
}

aftfs::Filesystem::Status BundleFilesystem::setCurrentDirectory(aftu::URL const& url)
{
    return aftfs::Filesystem::Status_UNKNOWN;
}

aftfs::Filesystem::Status BundleFilesystem::listDirectory(std::vector<aftu::URL>& results)
{
    aftu::URL baseUrl = Convert::toURL(m_priv->rootUrl);
    std::string path = baseUrl.path();
    
    DIR* dir = opendir(path.empty() ? "/" : path.c_str());
    if (!dir) {
        return Filesystem::Status_DIRECTORY_NOT_FOUND;
    }

    long name_max = getPathMaxLength(path);
    long len = offsetof(struct dirent, d_name) + name_max + 1;
    struct dirent* entry = (struct dirent*)malloc(len);
    
    int result = 0;
    struct dirent* entryResult = NULL;
    while ((result = readdir_r(dir, entry, &entryResult) == 0) && entryResult != NULL) {
        results.push_back(aftu::URL(baseUrl.canonical() + std::string(entry->d_name, entry->d_namlen)));
    }
    
    if (result != 0) {
        return Filesystem::Status_ERROR;
    }
    
    free(entry);
    
    return Filesystem::Status_OK;
}

aftfs::Filesystem::Status BundleFilesystem::listDirectory(std::vector<aftu::URL>& results, aftu::URL const& url)
{
    return aftfs::Filesystem::Status_UNKNOWN;
}

aftfs::Filesystem::DirectoryEntryPtr BundleFilesystem::directoryEntry(aftu::URL const& url, aftfs::Filesystem::Status* status)
{
    std::string path = url.path();
    if (path.empty()) {
        path = "/";
    }
    
    struct stat info;
    std::memset(&info, 0, sizeof(struct stat));
    
    stat(path.c_str(), &info);

    return Filesystem::DirectoryEntryPtr(new BundleDirectoryEntry(url, info));
}

aftfs::Filesystem::FileReaderPtr BundleFilesystem::openFileReader(aftu::URL const& url, aftfs::Filesystem::Status* status)
{
    std::string path = url.path();
    FILE* fileHandle = fopen(path.c_str(), "r");
    if (!fileHandle) {
        if (status) {
            *status = Filesystem::Status_FILE_NOT_FOUND;
        }

        return Filesystem::FileReaderPtr();
    }
    
    return aftfs::Filesystem::FileReaderPtr(new BundleFileReader(fileHandle));
}

BundleDirectoryEntry::BundleDirectoryEntry(aftu::URL const& url, struct stat const& info)
: m_url(url)
{
    std::memcpy(&m_info, &info, sizeof(struct stat));
}
    
BundleDirectoryEntry::~BundleDirectoryEntry()
{
}

aftu::URL BundleDirectoryEntry::url()
{
    return m_url;
}
    
aftio::Permissions BundleDirectoryEntry::permissions()
{
    return aftio::Permissions();
}
    
aftfs::DirectoryEntry::Type BundleDirectoryEntry::type() const
{
    return DirectoryEntry::Type_UNKNOWN;
}
    
bool BundleDirectoryEntry::isFile() const
{
    return S_ISREG(m_info.st_mode);
}
    
bool BundleDirectoryEntry::isDirectory() const
{
    return S_ISDIR(m_info.st_mode);
}
    
bool BundleDirectoryEntry::isLink() const
{
    return S_ISLNK(m_info.st_mode);
}

size_t BundleDirectoryEntry::size() const
{
    return static_cast<size_t>(m_info.st_size);
}

BundleFileReader::BundleFileReader(FILE* fileHandle)
: m_fileHandle(fileHandle)
{}

BundleFileReader::~BundleFileReader()
{
    if (m_fileHandle) {
        fclose(m_fileHandle);
    }
}

aftio::Reader::Status BundleFileReader::read(char* buffer, size_t bytes, size_t* actualBytes)
{
    size_t bytesRead = fread(buffer, sizeof(char), bytes, m_fileHandle);
    if (actualBytes) {
        *actualBytes = bytesRead;
    }
    
    aftio::Reader::Status status = aftio::Reader::Status_OK;

    if (bytesRead < bytes) {
        if (feof(m_fileHandle) > 0) {
            status = aftio::Reader::Status_EOF;
        }
        else if (ferror(m_fileHandle) > 0) {
            status = aftio::Reader::Status_ERROR;
        }
    }
    
    return status;
}

aftio::Reader::Status BundleFileReader::read(std::vector<char>& buffer, size_t bytes, size_t* actualBytes)
{
    size_t originalSize = buffer.size();
    buffer.resize(originalSize + bytes);
    size_t bytesRead = fread(&buffer[buffer.size()], sizeof(char), bytes, m_fileHandle);
    
    if (actualBytes) {
        *actualBytes = bytesRead;
    }
    
    aftio::Reader::Status status = aftio::Reader::Status_OK;
    
    if (bytesRead < bytes) {
        if (feof(m_fileHandle) > 0) {
            status = aftio::Reader::Status_EOF;
        }
        else if (ferror(m_fileHandle) > 0) {
            status = aftio::Reader::Status_ERROR;
        }

        buffer.resize(originalSize + bytesRead);
    }
    
    return status;
}

aftio::Reader::Status BundleFileReader::reset()
{
    clearerr(m_fileHandle);
    return aftio::Reader::Status_OK;
}

bool BundleFileReader::eof(aftio::Reader::Status* status)
{
    bool result = feof(m_fileHandle) > 0 ? true : false;
    if (status) {
        *status = aftio::Reader::Status_OK;
    }
    
    return result;
}

aftio::Reader::Status BundleFileReader::position(off_t position)
{
    aftio::Reader::Status status = aftio::Reader::Status_OK;

    int result = fseeko(m_fileHandle, position, 0);
    if (result != 0) {
        status = aftio::Reader::Status_ERROR;
    }
    
    return status;
}

off_t BundleFileReader::position(aftio::Reader::Status* status)
{
    off_t result = ftello(m_fileHandle);
    if (result < 0) {
        if (status) {
            *status = aftio::Reader::Status_ERROR;
        }

        return 0;
    }
    else {
        if (status) {
            *status = aftio::Reader::Status_OK;
        }

        return result;
    }
}

} // namespace

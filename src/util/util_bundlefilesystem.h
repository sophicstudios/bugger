#ifndef INCLUDED_ATTA_UTIL_BUNDLEFILESYSTEM_H
#define INCLUDED_ATTA_UTIL_BUNDLEFILESYSTEM_H

#include <CoreFoundation/CoreFoundation.h>
#include <aftfs_filesystem.h>
#include <aftu_url.h>
#include <vector>

namespace util {

class BundleFilesystem : public aftfs::Filesystem
{
public:
    enum Root {
        Root_MAIN,
        Root_RESOURCES,
        Root_SUPPORT
    };
    
    BundleFilesystem();

    BundleFilesystem(Root root);
    
    virtual ~BundleFilesystem();
    
    virtual aftu::URL getCurrentDirectory(aftfs::Filesystem::Status* status = NULL);
    
    virtual aftfs::Filesystem::Status setCurrentDirectory(aftu::URL const& url);
    
    virtual aftfs::Filesystem::Status listCurrentDirectory(std::vector<aftu::URL>& results);
    
    virtual aftfs::Filesystem::Status listDirectory(std::vector<aftu::URL>& results, aftu::URL const& url);
    
    virtual aftfs::Filesystem::DirectoryEntryPtr directoryEntry(aftu::URL const& url, aftfs::Filesystem::Status* status = NULL);
    
    virtual aftfs::Filesystem::FileReaderPtr openFileReader(aftu::URL const& url, aftfs::Filesystem::Status* status = NULL);
    
private:
    struct Priv;
    Priv* m_priv;
};

} // namespace

#endif

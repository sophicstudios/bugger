#ifndef INCLUDED_BUGGER_ATTA_UTIL_BUNDLEFILESYSTEM_H
#define INCLUDED_BUGGER_ATTA_UTIL_BUNDLEFILESYSTEM_H

#include <CoreFoundation/CoreFoundation.h>
#include <aftfs_filesystem.h>
#include <aftu_url.h>
#include <vector>

namespace util {

class BundleFileSystem : public aftfs::FileSystem
{
public:
    enum Root {
        Root_MAIN,
        Root_RESOURCES,
        Root_SUPPORT
    };
    
    BundleFileSystem();

    BundleFileSystem(Root root);
    
    virtual ~BundleFileSystem();
    
    virtual aftu::URL getCurrentDirectory(aftfs::FileSystem::Status* status = NULL);
    
    virtual aftfs::FileSystem::Status setCurrentDirectory(aftu::URL const& url);
    
    virtual aftfs::FileSystem::Status listCurrentDirectory(std::vector<aftu::URL>& results);
    
    virtual aftfs::FileSystem::Status listDirectory(std::vector<aftu::URL>& results, aftu::URL const& url);
    
    virtual aftfs::FileSystem::DirectoryEntryPtr directoryEntry(aftu::URL const& url, aftfs::FileSystem::Status* status = NULL);
    
    virtual aftfs::FileSystem::FileReaderPtr openFileReader(aftu::URL const& url, aftfs::FileSystem::Status* status = NULL);
    
private:
    struct Priv;
    Priv* m_priv;
};

} // namespace

#endif

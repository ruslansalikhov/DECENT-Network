/* (c) 2016, 2017 DECENT Services. For details refers to LICENSE.txt */

#pragma once

#include "detail.hpp"

#include <decent/package/package.hpp>
#include <ipfs_cache/client.h>
#include <event2/event.h>

#include <memory>


namespace decent { namespace package {


    class IPFSTransferEngine;


    class IPFSDownloadPackageTask : public detail::PackageTask {
    public:
        explicit IPFSDownloadPackageTask(PackageInfo& package);
        virtual ~IPFSDownloadPackageTask();

    protected:
        virtual void task() override;

    private:
        uint64_t ipfs_recursive_get_size(const std::string &url);
        void     ipfs_recursive_get(const std::string &url, const boost::filesystem::path &dest_path);

        struct event_base * _evbase;
    };


    class IPFSStartSeedingPackageTask : public detail::PackageTask {
    public:
        explicit IPFSStartSeedingPackageTask(PackageInfo& package);

    protected:
        virtual void task() override;

    private:
        struct event_base * _evbase;
    };


    class IPFSStopSeedingPackageTask : public detail::PackageTask {
    public:
        explicit IPFSStopSeedingPackageTask(PackageInfo& package);

    protected:
        virtual void task() override;

    private:
        struct event_base * _evbase;
    };


    class IPFSTransferEngine : public TransferEngineInterface {
    public:
        virtual std::shared_ptr<detail::PackageTask> create_download_task(PackageInfo& package) override;
        virtual std::shared_ptr<detail::PackageTask> create_start_seeding_task(PackageInfo& package) override;
        virtual std::shared_ptr<detail::PackageTask> create_stop_seeding_task(PackageInfo& package) override;
    };
    
    
} } // namespace decent::package


#ifndef DECENT_IPFSWRAPPER_H
#define DECENT_IPFSWRAPPER_H

#include <string>
#include <mutex>

namespace ipfs {

class IpfsWrapper
{
private:
    explicit IpfsWrapper();

public:
    IpfsWrapper(const IpfsWrapper&) = delete;
    IpfsWrapper& operator=(const IpfsWrapper&) = delete;
    ~IpfsWrapper();

    static IpfsWrapper& instance() {
        static IpfsWrapper the_IPFS_manager;
        static std::mutex ipfs_mutex;

        std::lock_guard<std::mutex> lock(ipfs_mutex);
        if (!the_IPFS_manager.IsStarted()) {
            if (!the_IPFS_manager.Initialize()) {
                throw std::runtime_error("IPFS initialization failed!");
            }
        }

        return the_IPFS_manager;
    }

    /**
     * @return returns IPFS peer ID
     */
    std::string ipfs_id();

    /**
     * @brief adds file to IPFS and returns CID
     * @param filename filename of the file to add
     * @param ipfs_url ipfs url name
     * @param cid resulting CID
     * @return returns TRUE if success otherwise false
     */
    bool ipfs_file_add(const std::string& filename, const std::string& ipfs_url, std::string& cid);


    bool ipfs_files_add_wrapped(const std::string& json_param, std::string& cid);

    /**
     * @brief IPFS ls on given cid
     * @param cid - CID
     * @param json_info resulting Json information
     * @return returns TRUE if success otherwise false
     */
    bool ipfs_ls(const std::string& cid, std::string& json_info);

    /**
     * @brief performs IPFS get on given cid
     * @param cid - CID
     * @param filename - filename to store content
     * @return returns TRUE if success otherwise false
     */
    bool ipfs_get(const std::string& cid, const std::string& filename);

    /**
     * @brief performs IPFS pin add on given cid
     * @param cid - CID
     * @param recursive resulting CID
     * @return returns TRUE if success otherwise false
     */
    bool ipfs_pin_add(const std::string& cid, bool recursive);

    /**
     * @brief performs IPFS pin rem on given cid
     * @param cid - CID
     * @param recursive resulting CID
     * @return returns TRUE if success otherwise false
     */
    bool ipfs_pin_rem(const std::string& cid, bool recursive);

    /**
     * @brief performs IPFS bitswap ledger on given peer_id
     * @param peer_id peer ID to get information
     * @param json_info resulting Json information
     * @return returns TRUE if success otherwise false
     */
    bool ipfs_bitswap_ledger(const std::string& peer_id, std::string& json_info);

private:
    /**
     * @brief Initialize IPFS
     * @param repo_path - path to IPFS repository or nullptr to get default
     */
    bool Initialize(const char* repo_path = nullptr);

    bool IsStarted() const { return m_started; }

private:
    bool m_started;

};


namespace detail {

    /** HTTP file upload. */
    struct FileUpload {
        /** The type of the `data` member. */
        enum class Type {
            /** The file contents, put into a string by the caller. For small files. */
                    kFileContents,
            /** File whose contents is streamed to the web server. For big files. */
                    kFileName,
        };

        /** File name to pretend to the web server. */
        const std::string path;

        /** The type of the `data` member. */
        Type type;

        /** The data to be added. Either a file name from which to read the data or
         * the contents itself. */
        const std::string data;
    };

}; //namespace detail

}; //namespace ipfs




#endif //DECENT_IPFSWRAPPER_H

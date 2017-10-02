#ifndef DECENT_IPFSWRAPPER_H
#define DECENT_IPFSWRAPPER_H

#include <string>

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
        return the_IPFS_manager;
    }

    /**
     * @brief Initialize IPFS
     */
    bool Initialize(const char* repo_path);

    bool IsStarted() const { return m_started; }

    /**
     * @return returns IPFS peer ID
     */
    std::string ipfs_id();

    /**
     * @brief adds file to IPFS and returns CID
     * @param filename filename of the file to add
     * @param cid resulting CID
     * @return returns TRUE if success otherwise false
     */
    bool ipfs_file_add(const std::string& filename, std::string& cid);

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
    bool m_started;

};


#endif //DECENT_IPFSWRAPPER_H

#ifndef DECENT_IPFSWRAPPER_H
#define DECENT_IPFSWRAPPER_H

#include <string>

class IpfsWrapper
{
public:
    IpfsWrapper();
    ~IpfsWrapper();

    bool Initialize(const char* repo_path);

    std::string ipfs_id();

    bool ipfs_file_add(const std::string& filename, std::string& cid);
    bool ipfs_ls(const std::string& cid, std::string& json_info);

    bool ipfs_pin_add(const std::string& cid, bool recursive);
    bool ipfs_pin_rem(const std::string& cid, bool recursive);

    bool ipfs_bitswap_ledger(const std::string& peer_id, std::string& json_info);


private:


};


#endif //DECENT_IPFSWRAPPER_H

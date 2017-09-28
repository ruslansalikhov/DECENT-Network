
#include "ipfs_wrapper.h"
#include <ipfs_bindings.h>
#include <cstdlib>

IpfsWrapper::IpfsWrapper()
{

}

IpfsWrapper::~IpfsWrapper()
{

}

bool IpfsWrapper::Initialize(const char* repo_path)
{
    return false;
}

std::string IpfsWrapper::ipfs_id()
{
    char* peer_id = go_ipfs_cache_id();
    if (peer_id == nullptr) {
        return std::string();
    }

    std::string result(peer_id); free(peer_id);
    return result;
}

bool IpfsWrapper::ipfs_file_add(const std::string& filename, std::string& cid)
{
    return false;
}

bool IpfsWrapper::ipfs_ls(const std::string& cid, std::string& json_info)
{
    return false;
}

bool IpfsWrapper::ipfs_pin_add(const std::string& cid, bool recursive)
{
    return false;
}

bool IpfsWrapper::ipfs_pin_rem(const std::string& cid, bool recursive)
{
    return false;
}

bool IpfsWrapper::ipfs_bitswap_ledger(const std::string& peer_id, std::string& json_info)
{
    return false;
}




#if 0
std::string filename("/Users/milanfranc/test/foo");
go_ipfs_cache_file_add_return ret_val = go_ipfs_cache_file_add((char*) filename.data());
if (! (bool)ret_val.r0) {
throw std::runtime_error("add: Failed..");
}

std::cout << "Res:" << std::string(ret_val.r1) << std::endl;
free(ret_val.r1);

char* peer_id = go_ipfs_cache_id();
if (peer_id == NULL) {
throw std::runtime_error("go_ipfs_cache_id: Failed..");
}
std::string my_peer_id(peer_id); free(peer_id);

std::cout << "ID:" << my_peer_id << std::endl;

go_ipfs_cache_bitswap_ledger_return ledger;
ledger = go_ipfs_cache_bitswap_ledger((char*)my_peer_id.data());
if (! (bool)ledger.r0) {
throw std::runtime_error("go_ipfs_cache_bitswap_ledger: Failed..");
}

std::cout << "data:" << std::string(ledger.r1) << std::endl;


std::string pin_hash("QmXQt6tBJkMVpmiJH57z7gd4GBGTUfCtaygiMKVbDsBmnV");
ret = go_ipfs_cache_pin_add((char*) pin_hash.data(), true);
if (!ret) {
throw std::runtime_error("pin add: Failed..");
}

ret = go_ipfs_cache_pin_rem((char*) pin_hash.data(), true);
if (!ret) {
throw std::runtime_error("pin rem: Failed..");
}

ret = go_ipfs_cache_get((char*) pin_hash.data(), (char*) "/Users/milanfranc/test/abc");
if (!ret) {
throw std::runtime_error("get: Failed..");
}

pin_hash = "QmRewxxZybFMwx6GphFusSEnwSM5vM225iAJHCpCnx2kzK";
ret = go_ipfs_cache_ls((char*) pin_hash.data());
if (!ret) {
throw std::runtime_error("ls: Failed..");
}
#endif





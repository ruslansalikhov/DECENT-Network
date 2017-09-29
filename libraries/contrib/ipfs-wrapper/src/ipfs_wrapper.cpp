
#include "ipfs_wrapper.h"
#include <ipfs_bindings.h>
#include <cstdlib>

IpfsWrapper::IpfsWrapper() : m_started(false)
{
}

IpfsWrapper::~IpfsWrapper()
{
    if (m_started) {
        go_ipfs_cache_stop();
    }
}

bool IpfsWrapper::Initialize(const char* repo_path)
{
    if (m_started) {
        go_ipfs_cache_stop();
        m_started = false;

        //NOTE: maybe some sleep...
    }

    bool ret = go_ipfs_cache_start(const_cast<char*>(repo_path) );
    if (!ret) {
        return false;
    }

    //NOTE: wait for start daemon ?
    m_started = true;
    return true;
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
    go_ipfs_cache_file_add_return ret_val;
    ret_val = go_ipfs_cache_file_add(const_cast<char*>(filename.c_str()) );
    if (! (bool)ret_val.r0) {
        return false;
    }

    cid = std::string(ret_val.r1); free(ret_val.r1);
    return true;
}

bool IpfsWrapper::ipfs_ls(const std::string& cid, std::string& json_info)
{
    go_ipfs_cache_ls_return ret;
    ret = go_ipfs_cache_ls(const_cast<char*>(cid.c_str()) );
    if (! (bool)ret.r0) {
        return false;
    }

    json_info = std::string(ret.r1); free(ret.r1);
    return true;
}

bool IpfsWrapper::ipfs_pin_add(const std::string& cid, bool recursive)
{
    return go_ipfs_cache_pin_add(const_cast<char*>(cid.c_str()), recursive);
}

bool IpfsWrapper::ipfs_pin_rem(const std::string& cid, bool recursive)
{
    return go_ipfs_cache_pin_rem(const_cast<char*>(cid.c_str()), recursive);
}

bool IpfsWrapper::ipfs_bitswap_ledger(const std::string& peer_id, std::string& json_info)
{
    go_ipfs_cache_bitswap_ledger_return ledger;
    ledger = go_ipfs_cache_bitswap_ledger(const_cast<char*>(peer_id.c_str()));
    if (! (bool)ledger.r0) {
        return false;
    }

    json_info = std::string(ledger.r1); free(ledger.r1);
    return true;
}

